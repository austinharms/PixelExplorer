import sys
from os import listdir
from os.path import isfile, join

def getFiles(path):
	files = []
	for f in listdir(path):
		if isfile(join(path, f)) and f != sys.argv[2]:
			files.append(f)
	return files

files = getFiles(sys.argv[1])
headerFile = open(join(sys.argv[1], sys.argv[2]), "w")
headerFile.write("#ifndef PXENGINE_API_H_\n")
headerFile.write("#define PXENGINE_API_H_\n")
for f in files:
	headerFile.write(f'#include "./{f}"\n')
headerFile.write("#endif //!PXENGINE_API_H_\n")