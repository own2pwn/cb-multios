from os import listdir
import platform
import os

#reads in old makefile from folder
#parses for compiler arguments 
#creates cmake lists file with parsed arguments as parent-scope variables


def replaceWin32Flags(flags):
	win32_map = {
		"-O0": "",
		"-O1": "/O1",
		"-O2": "/O2",
		"-g": "/Zi",
		"-Wno-overlength-strings": "",
		"-Wno-packed" : "",
		"-fno-stack-protector" : "/GS-",
		"-fno-rtti" : "/GR-",
		"-Wall": "/Wall",
		"-Wextra": "",
		"-Wno-missing-field-initializers": "",
		"-Wno-pointer-sign": "",
		"-Wno-sign-compare": "",
		"-Wno-unused-function": "",
		"-mfpmath=sse": "/arch:SSE",
		"-msse2": "/arch:SSE2",
		"-Werror": "/WX",
		}

	for k,v in win32_map.iteritems():
		if k in flags:
			flags = flags.replace(k, v)

	return flags


def readAndMake(folder):
	inStream = open(os.path.join(folder, "Makefile"), "r")
	oldMake = inStream.readlines()
	inStream.close()

	makeVars = {}
	for i in oldMake:
		i = i.replace("\n", "").replace("\t", "") 
		if "=" in i:
			var,_,val = i.partition("=")
			var = var.strip()
			val = val.strip()
			makeVars[var] = val
			#makeVars += ['set( ' + var + ' "' + val + '")']

	if "CFLAGS" not in makeVars:
		makeVars["CFLAGS"] = "-g"

	if "-Werror" in makeVars["CFLAGS"] and platform.system() != "Windows":
		makeVars["CFLAGS"] = makeVars["CFLAGS"].replace("-Werror", "")

	if platform.system() == "Windows":
		makeVars["CFLAGS"] += " /wd4244 /wd4101 /wd4018 /wd4102 /wd4820 /wd4133 /wd4129"

	newMake = "\n".join(['set( {} "{}")'.format(k,v) for k,v in makeVars.iteritems()])
	newMake += "\n"

	if platform.system() == "Windows":
		newMake =  replaceWin32Flags(newMake)
	
	#newMake += 'MESSAGE("Building!!! ")\n'
	newMake += "buildCB(${CFLAGS})"


	outStream = open(os.path.join(folder, "CMakeLists.txt"), "w")
	outStream.write(newMake)
	#print newMake	
	outStream.close()

#write makeFiles for all folders in path
def doAll(path):
	dirs = listdir(path)
	for folder in dirs:
		folder = os.path.join(path, folder)
		#print folder
		if "00" in folder:
			print folder
			readAndMake(folder)


path = os.path.join("..", "cqe-challenges")
doAll(path)   ##path should be folder containing multiple challenge binaries and nothing else. 
#readAndMake(os.path.join("..", "cqe-challenges", "NRFIN_00010"))  #path should be folder of a single challenge binary
#readAndMake(os.path.join("..", "cqe-challenges",  "NRFIN_00013"))
