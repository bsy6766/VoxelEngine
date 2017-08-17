#include "DataTree.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

using namespace Voxel;

std::unordered_map<std::string, bool> DataTree::boolAliases = {
	{"yes", true},
	{"no", false},
	{"y", true},
	{"n", false},
	{"true", true},
	{"false", false}
};

char DataTree::commentSymbol = '#';

DataTree::DataTree(const std::string& key, const std::string& value) : key(key), value(value)
{}

DataTree::~DataTree()
{
	this->clear();
}

DataTree* DataTree::create()
{
	return new DataTree(std::string(), std::string());
}

DataTree* DataTree::create(const std::string& fileName)
{
	if (fileName.empty())
	{
		return nullptr;
	}

	std::string fileData = std::string();
	std::ifstream file(fileName);

	if (file.is_open())
	{
		// Simply using while loop and getline to read file instead of using file size method because file size wasn't correct.
		std::string line;
		while (std::getline(file, line))
		{
			fileData += (line + "\n");
		}
	}

	if (fileData.empty() || fileData.size() <= 0)
	{
		//Data is empty or size is 0. 
		return nullptr;
	}

	DataTree* data = new DataTree("ROOT_KEY", "ROOT_VALUE");
	bool result = data->parse(fileData);

	if (result == false)
	{
		//Failed to parse
		delete data;
		return nullptr;
	}

	return data;
}

bool DataTree::parse(const std::string& fileData)
{
	// check one more time just because
	if (fileData.empty())
	{
		return false;
	}

	// Parse data
	int startingIndex = 0;
	return this->parseData(fileData, startingIndex, -1);
}

bool DataTree::parseData(const std::string& fileData, int& curIndex, int curDepth)
{
	// Success flag
	bool success = true;

	// Return if current index is bigger than file size
	if (curIndex >= fileData.size()) { return true; }

	// Get index where next line starts
	int nextLineIndex = this->findNewLineIndex(fileData, curIndex) + 1;

	// Return if next line index is current index
	if (nextLineIndex == curIndex){ return true; }

	// Find hwere comment index starts
	int commentIndex = findCommentSymbolOrNewLineIndex(fileData, curIndex);
	// Find the first index where non whitespace starts
	int nonWhiteSpaceIndex = findFirstNonWhitespaceIndex(fileData, curIndex);

	// If comment index is before the first non whitesapce character, the entire line is comment
	if (commentIndex <= nonWhiteSpaceIndex)
	{
		// Skip to next line
		curIndex = nextLineIndex;
		return parseData(fileData, curIndex, curDepth);
	}
	
	// If current line isn't comment, parse it.
	int parsingDepth = getCurrentDepth(fileData, curIndex);
	// Get the size of data
	const int size = fileData.size();

	//Key and Value
	std::string newKey;
	std::string newValue;

	// Check depth
	if (parsingDepth > curDepth)
	{
		// Parsing dpeth is deeper than current depth. So advance.
		curDepth = parsingDepth;

		// Run while recursion ends with same depth
		while (curDepth <= parsingDepth)
		{
			// Get index where next line starts
			nextLineIndex = findNewLineIndex(fileData, curIndex) + 1;
			// Parse the key
			newKey = parseKey(fileData, size, parsingDepth + curIndex, nextLineIndex, curIndex);

			// Only if key is valid
			if (newKey.size() > 0 && newKey.empty() == false)
			{
				// Parse value
				newValue = parseValue(fileData, size, curIndex);
			}

			// Add new key and value to children.
			this->children[newKey] = new DataTree(newKey, newValue);
			// Advance current index to next line
			curIndex = nextLineIndex;
			// Recursively parse data on new child
			success = this->children[newKey]->parseData(fileData, curIndex, parsingDepth);

			// Update parsing depth
			parsingDepth = getCurrentDepth(fileData, curIndex);
		}
	}

	// Really no point of using bool as return. Used for debugging.
	return success;
}

const std::string DataTree::parseKey(const std::string& fileData, const int size, const int start, const int end, int& curIndex)
{
	// Parsing depth is on same level with current dpeth. 
	// Start from index after depth
	// If starting pos is larger than next line index, skip
	std::string newKey = std::string();
	if (start < end)
	{
		// Ignore whitespace after dpeth
		curIndex = findWhitespace(fileData, start + 1);
		newKey = fileData.substr(start, curIndex < size ? curIndex - start : std::string::npos);
	}

	return newKey;
}

const std::string DataTree::parseValue(const std::string& fileData, const int size, int& curIndex)
{
	// Advance till we find valid character as key
	curIndex = findFirstNonWhitespaceIndex(fileData, curIndex);
	// End is until we hind comment or new line
	const int end = findCommentSymbolOrNewLineIndex(fileData, curIndex);

	std::string newValue = std::string();

	if (curIndex < size && end > curIndex)
	{
		newValue = fileData.substr(curIndex, end - curIndex);
	}
	else
	{
		newValue = std::string();
	}

	return trim(newValue);
}

int DataTree::findNewLineIndex(const std::string& fileData, int curIndex)
{
	// Get size of data
	unsigned int dataSize = fileData.size();
	unsigned int i = 0;
	// Loop until it finds new line char
	for (i = curIndex; i < dataSize && !isNewLineChar(fileData[i]); i++)
	{
		if (isNewLineChar(fileData[i]))
		{
			return i;
		}
	}

	return i;
}

bool DataTree::isNewLineChar(const char& c)
{
	// We are ignoring CR+LF here. 
	if (c == '\n' || c == '\r')
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DataTree::isWhitespace(const char& c)
{
	// Consider space, tab, newline as whitespace
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

int DataTree::findCommentSymbolOrNewLineIndex(const std::string& fileData, int curIndex)
{
	// start from current index
	unsigned int i = curIndex;
	// stop at newline
	unsigned int end = findNewLineIndex(fileData, curIndex);

	while (i < end)
	{
		if (fileData[i] == DataTree::commentSymbol)
		{
			break;
		}

		i++;
	}

	return i;
}

int DataTree::findFirstNonWhitespaceIndex(const std::string& fileData, int curIndex)
{
	// start from current index
	unsigned int i = curIndex;
	// end of file
	unsigned int size = fileData.size();

	// Because newline considered as newline, iterate til end
	for (; i < size; i++)
	{
		bool whitespace = isWhitespace(fileData[i]);
		bool newLine = isNewLineChar(fileData[i]);

		if (whitespace && !newLine)
		{
			continue;
		}
		else
		{
			break;
		}
	}

	return i;
}

int DataTree::getCurrentDepth(const std::string& fileData, int curIndex)
{
	unsigned int i = curIndex;
	unsigned int size = fileData.size();

	int counter = 0;

	for (; i < size; i++)
	{
		bool whitespace = isWhitespace(fileData[i]);

		if (!whitespace)
		{
			// Depth identation ends. Get diff
			return i - curIndex;
		}
	}

	// Wasn't able to find any depth. 
	return -1;
}

int DataTree::findWhitespace(const std::string& fileData, int curIndex)
{
	unsigned int i = curIndex;
	unsigned int size = fileData.size();

	for (; i < size; i++)
	{
		bool whitespace = isWhitespace(fileData[i]);
		if (whitespace)
		{
			break;
		}
	}

	return i;
}

const std::string DataTree::getKey()
{
	return this->key;
}

const std::string DataTree::getValue()
{
	return this->value;
}

const std::string DataTree::getValueFromKey(int curIndex, const std::vector<std::string>& keys)
{
	// Reject empty keys
	if (keys.empty()) { return std::string(); };

	const unsigned int size = keys.size();

	// Check size
	if (curIndex >= size) { return std::string(); };

	// Find key
	auto find_it = this->children.find(keys.at(curIndex));
	if (find_it == this->children.end())
	{
		// Key doesn't exists
		return std::string();
	}
	else
	{
		if (curIndex == (size - 1))
		{
			// If index is last, get value
			return this->children[keys.at(curIndex)]->value;
		}
		else
		{
			return this->children[keys.at(curIndex)]->getValueFromKey(curIndex + 1, keys);
		}
	}
}

const std::vector<std::string> DataTree::splitKeys(const std::string& key)
{
	std::vector<std::string> splitKeys;

	std::stringstream ss(key);
	std::string item;
	while (std::getline(ss, item, '.')) 
	{
		splitKeys.push_back(item);
	}

	return splitKeys;
}

const int DataTree::getInt(const std::string& key)
{
	const std::string valueStr = getString(key);
	int valueInteger = 0;
	try
	{
		valueInteger = std::stoi(valueStr);
	}
	catch (...)
	{
		valueInteger = 0;
	}

	return valueInteger;
}

const float DataTree::getFloat(const std::string& key)
{
	const std::string valueStr = getString(key);
	float valueFloat = 0.0f;
	try
	{
		valueFloat = std::stof(valueStr);
	}
	catch (...)
	{
		valueFloat = 0.0f;
	}

	return valueFloat;
}

const double DataTree::getDouble(const std::string& key)
{
	const std::string valueStr = getString(key);
	double valueDouble = 0.0;
	try
	{
		valueDouble = std::stod(valueStr);
	}
	catch (...)
	{
		valueDouble = 0.0;
	}

	return valueDouble;
}

const bool DataTree::getBool(const std::string& key)
{
	std::string valueStr = getString(key); 
	std::transform(valueStr.begin(), valueStr.end(), valueStr.begin(), ::tolower);
	auto find_it = DataTree::boolAliases.find(valueStr);
	if (find_it != DataTree::boolAliases.end())
	{
		return find_it->second;
	}
	else
	{
		return false;
	}
}

const std::string DataTree::getString(const std::string& key)
{
	auto splitedKeys = splitKeys(key);
	std::string value = getValueFromKey(0, splitedKeys);
	return value;
}

const bool DataTree::setInt(const std::string& key, const int value, const bool overwrite)
{
	return setInt(key, std::to_string(value), overwrite);
}

const bool DataTree::setInt(const std::string& key, const std::string& value, const bool overwrite)
{
	return setString(key, value, overwrite);
}

const bool DataTree::setFloat(const std::string& key, const float value, const bool overwrite)
{
	return setFloat(key, std::to_string(value), overwrite);
}

const bool DataTree::setFloat(const std::string& key, const std::string& value, const bool overwrite)
{
	return setString(key, value, overwrite);
}

const bool DataTree::setDouble(const std::string& key, const double value, const bool overwrite)
{
	return setDouble(key, std::to_string(value), overwrite);
}

const bool DataTree::setDouble(const std::string& key, const std::string& value, const bool overwrite)
{
	return setString(key, value, overwrite);
}

const bool DataTree::setBool(const std::string& key, const bool value, const bool overwrite)
{
	std::string boolStr = std::string();
	if (value)
	{
		boolStr = "true";
	}
	else
	{
		boolStr = "false";
	}

	return setBool(key, boolStr, overwrite);
}

const bool DataTree::setBool(const std::string& key, const std::string& value, const bool overwrite)
{
	return setString(key, value, overwrite);
}

const bool DataTree::setString(const std::string& key, const std::string& value, const bool overwrite)
{
	// Empty key is rejected
	if (key.empty()) { return false; }

	// Empty value is accepted. It erases the string value.
	auto splitedKeys = splitKeys(key);

	// Empty splitted keys are rejceted
	if (splitedKeys.empty()) { return false; }

	unsigned int i = 0;
	const unsigned size = splitedKeys.size();

	DataTree* treeNode = this;

	bool newKey = false;

	for (; i < size; i++)
	{
		auto find_it = treeNode->children.find(splitedKeys.at(i));
		if (find_it != treeNode->children.end())
		{
			// Key exists
			treeNode = find_it->second;
			continue;
		}
		else
		{
			// Key doesn't exists. Add new path.
			treeNode->children[splitedKeys.at(i)] = new DataTree(splitedKeys.at(i), std::string());
			treeNode = treeNode->children[splitedKeys.at(i)];
			newKey = true;
		}
	}

	if (newKey)
	{
		// It's a new key. Overwrite doesn't matter.
		treeNode->value = value;
	}
	else
	{
		// Existing key. Check for overwrite
		if (overwrite)
		{
			treeNode->value = value;
		}
		//Else, do nothing.
	}

	return true;
}

const bool DataTree::hasKey(const std::string key)
{
	if (key.empty()) { return false; }

	return hasKey(splitKeys(key));
}

const bool DataTree::hasKey(const std::vector<std::string> splitedKeys)
{
	if (splitedKeys.empty()) { return false; }

	DataTree* treeNode = this;

	unsigned int i = 0;
	const unsigned int size = splitedKeys.size();

	for (; i < size; i++)
	{
		auto find_it = treeNode->children.find(splitedKeys.at(i));
		if (find_it != treeNode->children.end())
		{
			// Key exists
			treeNode = find_it->second;
			continue;
		}
		else
		{
			return false;
		}
	}

	return true;
}

const int DataTree::getChildrenSize()
{
	return this->children.size();
}

const std::vector<std::string> DataTree::getKeySet()
{
	std::vector<std::string> keySet;

	for (auto child : this->children)
	{
		keySet.push_back(child.first);
	}

	return keySet;
}

const std::vector<std::string> DataTree::getKeySet(const std::string& key)
{
	std::vector<std::string> keySet;

	if (key.empty())
	{
		return keySet;
	}

	DataTree* targetTree = traverse(splitKeys(key));
	if (targetTree == nullptr)
	{
		return keySet;
	}

	keySet = targetTree->getKeySet();

	return keySet;
}

const std::string DataTree::dataToString(DataTree* treeNode, int depth)
{
	std::string data = std::string();

	if (treeNode->children.empty())
	{
		return treeNode->value + "\n";
	}
	else
	{
		const std::vector<std::string> keySet = treeNode->getKeySet();

		int count = 0;
		for (const auto key : keySet)
		{
			int childrenCount = treeNode->children[key]->getChildrenSize();
			if (childrenCount != 0)
			{
				data += (key + " " + treeNode->children[key]->value + "\n");
				for (unsigned int i = 0; i <= depth; i++)
				{
					data += "\t";
				}
			}
			else
			{
				data += (key + " ");
			}

			data += dataToString(treeNode->children[key], depth + 1);

			if (count < keySet.size() - 1)
			{
				for (unsigned int i = 0; i < depth; i++)
				{
					data += "\t";
				}
			}

			count++;
		}

		return data;
	}
}

DataTree* DataTree::traverse(const std::vector<std::string>& splitedKeys)
{
	if (splitedKeys.empty())
	{
		return nullptr;
	}

	unsigned int i = 0;
	const unsigned size = splitedKeys.size();

	DataTree* treeNode = this;

	bool newKey = false;

	for (; i < size; i++)
	{
		auto find_it = treeNode->children.find(splitedKeys.at(i));
		if (find_it != treeNode->children.end())
		{
			// Key exists
			treeNode = find_it->second;
			continue;
		}
		else
		{
			// Key doesn't exists. Add new path.
			treeNode = nullptr;
			break;
		}
	}

	return treeNode;
}

const std::string DataTree::trim(const std::string& str)
{
	std::string result = str;

	{
		//Trim whitespace
		const auto strBegin = result.find_first_not_of(' ');
		if (strBegin == std::string::npos)
			return ""; // no content

		const auto strEnd = result.find_last_not_of(' ');
		const auto strRange = strEnd - strBegin + 1;

		result = result.substr(strBegin, strRange);
	}

	{
		//Trim tab
		const auto strBegin = result.find_first_not_of('\t');
		if (strBegin == std::string::npos)
			return ""; // no content

		const auto strEnd = result.find_last_not_of('\t');
		const auto strRange = strEnd - strBegin + 1;

		result = result.substr(strBegin, strRange);
	}

	return result;
}

const std::string DataTree::toString()
{
	return dataToString(this, 0);
}

void DataTree::save(const std::string& fileName)
{
	std::string data = toString();
	std::ofstream out(fileName);
	out << data;
	out.close();
}

void DataTree::clear()
{
	for (const auto& child : this->children)
	{
		if (&child != nullptr)
		{
			delete (&child)->second;
		}
	}
}