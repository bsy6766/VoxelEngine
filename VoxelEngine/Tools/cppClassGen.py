#!/usr/bin/python
import sys

# Get argument length
argvLen = len(sys.argv)

# ./cppClassGen fileName className (optional)defineName
# This generates fileName.cpp and fileName.h with class name of className.

if argvLen <= 2 :
	print "Try ./python cppClassGen.py [file name] [class name]"
	quit()

fileName = sys.argv[1]
className = sys.argv[2]
defineName = ""

if argvLen is 4 :
	defineName = sys.argv[3] + "_H"
else :
	defineName = className.upper() + "_H"

sourceFileName = fileName + ".cpp"
headerFileName = fileName + ".h"

# open source and header file
sourceFile = open("../Classes/" + sourceFileName, 'w')
headerFile = open("../Classes/" + headerFileName, 'w')

# build str
sourceStr = "#include \"" + headerFileName + "\""
headerStr = "#ifndef " + defineName + "\n#define " + defineName + "\n\nnamespace Voxel\n{\n\n}\n\n#endif"

# write
sourceFile.write(sourceStr)
headerFile.write(headerStr)

#close
sourceFile.close()
headerFile.close()