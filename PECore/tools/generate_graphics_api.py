# coding=utf8
import sys
import os
import re

regex = r"\s?\w*\s*PE_GRAPHICS_API\s+(\b\S+\b\**)\s+PE_CALL\s(\w+)\((.*)\);"
srcFile = open(os.path.join(sys.argv[1], "public", "PE_graphics.h"));
dstFile = open(os.path.join(sys.argv[1], "private", "PE_generated_graphics_api.h"), "w")
functions = re.finditer(regex, srcFile.read(), re.MULTILINE)
srcFile.close()

fileHeader = """// THIS IS AN AUTO GENERATED FILE DO NOT EDIT
// This file is included multiple times don't add an include guard
#ifndef PE_GENERATED_GRAPHICS_API
#error PE_GENERATED_GRAPHICS_API must be defined when including this file
#endif // !PE_GENERATED_GRAPHICS_API

"""

dstFile.write(fileHeader)
for function in functions:
    returnType, fucName, params = function.groups()
    paramList = params.split(",")
    paramNames = []
    for param in paramList:
        paramNames.append(param.split(" ")[-1])
    dstFile.write("PE_GENERATED_GRAPHICS_API({0}, {1}, ({2}), ({3}), {4})\n".format(returnType, fucName, params, ", ".join(paramNames), "" if returnType == "void" else "return"))
dstFile.close()
