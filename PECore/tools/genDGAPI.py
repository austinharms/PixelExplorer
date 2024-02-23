import sys
import os
import re

regex = r"PE_EXTERN_C\s+\w*\s*PE_API\s+(\b\S+\b\**)\s+PE_CALL\s(\w+)\((.*)\);"
srcFile = open(os.path.join(sys.argv[1], "public", "PE_graphics.h"));
dstFile = open(os.path.join(sys.argv[1], "private", "PE_dgapi.h"), "w")
functions = re.finditer(regex, srcFile.read(), re.MULTILINE)
srcFile.close()

fileHeader = """// THIS IS AN AUTO GENERATED FILE DO NOT EDIT
// This file is included multiple times don't add an include guard
#ifndef PE_GRAPHICS_API
#error PE_GRAPHICS_API must be defined when including this file
// This is only here to make intellisense happy
#include "PE_defines.h"
#define PE_GRAPHICS_API(rc, fn, params, args, ret) \\
	typedef rc (PE_CALL *PE_DGAPI_##fn) params; \\
	PE_EXTERN_C static rc PE_CALL fn## params;
struct PE_DGAPI_GraphicsJumpTable;
struct SDL_Window;
#endif // !PE_GRAPHICS_API

PE_GRAPHICS_API(int, PE_InitGraphics, (PE_DGAPI_GraphicsJumpTable* jmpTable), (jmpTable), return)
"""

dstFile.write(fileHeader)
for function in functions:
    returnType, fucName, params = function.groups()
    paramList = params.split(",")
    paramNames = []
    for param in paramList:
        paramNames.append(param.split(" ")[-1])
    dstFile.write("PE_GRAPHICS_API({0}, {1}, ({2}), ({3}), {4})\n".format(returnType, fucName, params, ", ".join(paramNames), "" if returnType == "void" else "return"))
dstFile.close()
