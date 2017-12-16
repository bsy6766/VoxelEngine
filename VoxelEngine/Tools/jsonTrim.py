#!/usr/bin/python
import sys
import json
import glob

jsonList = glob.glob("SpriteSheets/*.json")

for jsonPath in jsonList:

	print "Trimming \"" + jsonPath + "\""

	rFile = open(jsonPath, 'r')
	data = json.load(rFile)
	if "meta" in data:
		texturePath = data["meta"]["image"]
		textureWidth = data["meta"]["size"]["w"]
		textureHeight = data["meta"]["size"]["h"]

		del data["meta"]

		textureDict = {"path":texturePath, "width":textureWidth, "height":textureHeight}
		data["texture"] = textureDict

	index = 0;

	for elem in data["frames"]:
		if "rotated" in elem:
			del data["frames"][index]["rotated"]
		if "trimmed" in elem:
			del data["frames"][index]["trimmed"]
		if "sourceSize" in elem:
			del data["frames"][index]["sourceSize"]
		if "spriteSourceSize" in elem:
			del data["frames"][index]["spriteSourceSize"]
		if "pivot" in elem:
			del data["frames"][index]["pivot"]
		index += 1

	rFile.close()

	wFile = open(jsonPath, 'w')
	json.dump(data, wFile, indent=4)
	wFile.close()