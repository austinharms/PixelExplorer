import sys
from os import listdir, makedirs
from os.path import isfile, join, exists

def stripFile(inputPath, outputPath):
	outFile = open(outputPath, "w")
	inFile = open(inputPath, "r")
	depth = 0
	for line in inFile:
		if line.strip().startswith("PXE_PRIVATE_IMPLEMENTATION_START"):
			depth += 1
		elif line.strip().startswith("PXE_PRIVATE_IMPLEMENTATION_END"):
			depth -= 1
			if depth < 0:
				sys.exit(f"Found PXE_PRIVATE_IMPLEMENTATION_END without matching PXE_PRIVATE_IMPLEMENTATION_START in {outputPath}")
		elif depth == 0:
			outFile.write(line)
	inFile.close()
	outFile.close()

def getFiles(path):
	files = []
	for f in listdir(path):
		if isfile(join(path, f)):
			files.append(f)
	return files

if not exists(sys.argv[2]):
    makedirs(sys.argv[2])
files = getFiles(sys.argv[1])
for f in files:
	stripFile(join(sys.argv[1], f), join(sys.argv[2], f))