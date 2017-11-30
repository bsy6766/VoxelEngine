#ifndef DATATREE_H
#define DATATREE_H

// cpp
#include <string>
#include <unordered_map>

namespace Voxel
{
	/**
	*	@class DataTree
	*
	*	@brief A instance with data including parser itself.
	*
	*	This class stores data from a file it parses.
	*	If parse fails, this instances does nothing.
	*/
	class DataTree
	{
	private:
		//* Private default constructor. Initializes instance.
		DataTree(const std::string& key, const std::string& value);

		// Comment symbol. '#' by default
		static char commentSymbol;

		// A key
		std::string key;

		// A value
		std::string value;

		// Aliases for bool type
		static std::unordered_map<std::string, bool> boolAliases;

		// children
		std::unordered_map<std::string, DataTree*> children;

		/**
		*	Parse the file data
		*	@param fileData A string file data
		*	@return True if successfully loads data.
		*/
		bool parse(const std::string& fileData);

		/**
		*	Helper function for parse function. It's recursive.
		*	@param fileData A string file data
		*	@param curIndex A int reference for index that currently parser pointing to on string data.
		*	@param curDepth Current depth of parser on each recursive stack.
		*	@return True if successfully loads data.
		*/
		bool parseData(const std::string& fileData, int& curIndex, int curDepth);

		/**
		*	Find index where first new line occurs from current index
		*	@param fileData A string file data
		*	@param curIndex A int reference for index that currently parser pointing to on string data.
		*	@return A index where first new line character is. Returns file size if fails to find any.
		*/
		int findNewLineIndex(const std::string& fileData, int curIndex);

		/**
		*	Find either comment symbol or new line character from current index.
		*	@param fileData A string file data
		*	@param curIndex A int reference for index that currently parser pointing to on string data.
		*	@return A index where either first comment symbol or new line character is. Returns file size if fails to find any.
		*/
		int findCommentSymbolOrNewLineIndex(const std::string& fileData, int curIndex);

		/**
		*	Find first non whitespace character from current index
		*	@param fileData A string file data
		*	@param curIndex A int reference for index that currently parser pointing to on string data.
		*	@return A index where first non whitespace character is. Returns file size if fails to find any.
		*/
		int findFirstNonWhitespaceIndex(const std::string& fileData, int curIndex);

		/**
		*	Find first whitespace character from current index
		*	@param fileData A string file data
		*	@param curIndex A int reference for index that currently parser pointing to on string data.
		*	@return A index where first whitespace character is. Returns file size if fails to find any.
		*/
		int findWhitespace(const std::string& fileData, int curIndex);

		/**
		*	Get current depth base on current index.
		*	@note Tab, space, newline are considered as single depth.
		*	@param fileData A string file data
		*	@param curIndex A int reference for index that currently parser pointing to on string data.
		*	@return Current depth. -1 if there isn't any depth to find.
		*/
		int getCurrentDepth(const std::string& fileData, int curIndex);

		/**
		*	Checks if character is new line character
		*	@note Only checks CR and LF seperately. Doesn't check CR+LF.
		*	@param Character to check.
		*	@return True if character is newline character. Else, false.
		*/
		bool isNewLineChar(const char& c);

		/**
		*	Checks if character is whitespace.
		*	@note Tab, space, newline are considered as single depth.
		*	@param Character to check.
		*	@return True if character is whitespace character. Else, false.
		*/
		bool isWhitespace(const char& c);

		/**
		*	Parse key from the data string.
		*	@param fileData A string file data
		*	@param size Size of data string
		*	@param start An index where parse starts.
		*	@param end An index where parse ends.
		*	@param curIndex Current index where parser is pointing to file data string.
		*	@return A key string. Empty string if key doesn't exists.
		*/
		const std::string parseKey(const std::string& fileData, const int size, const int start, const int end, int& curIndex);

		/**
		*	Parse value from the data string.
		*	@param fileData A string file data
		*	@param size Size of data string
		*	@param curIndex Current index where parser is pointing to file data string.
		*	@return A value string. Empty string if key doesn't exists.
		*/
		const std::string parseValue(const std::string& fileData, const int size, int& curIndex);

		/**
		*	Get key.
		*	@return Key as string.
		*/
		const std::string getKey();

		/**
		*	Get value.
		*	@return Value as string.
		*/
		const std::string getValue();

		/**
		*	Checks if data already has key.
		*	@param splitedKeys A splitted keys to check.
		*	@return True if data has same key. Else, false.
		*/
		const bool hasKey(const std::vector<std::string> splitedKeys);

		/**
		*	Get value from key
		*	@param keys A sequence of key path of value.
		*	@return A value as string.
		*/
		const std::string getValueFromKey(int curIndex, const std::vector<std::string>& keys);

		/**
		*	Split key
		*	@param A key string to split.
		*	@return Key string set as vector.
		*/
		const std::vector<std::string> splitKeys(const std::string& key);

		/**
		*	Convert data to string
		*	@param treeNode A DataTree to convert
		*	@param depth Current depth of data.
		*	@note This is recursive function.
		*	@return String data.
		*/
		const std::string dataToString(DataTree* treeNode, int depth);

		/**
		*	Traverse to specific DataTree instance in tree.
		*	@param splitedKeys A key to find.
		*	@return DataTree instance if exists. Else, nullptr;
		*/
		DataTree* traverse(const std::vector<std::string>& splitedKeys);

		/**
		*	Trims extra whitespaces at the end of string
		*	@param str String to trim
		*	@return Trimed string.
		*/
		const std::string trim(const std::string& str);
	public:
		/**
		*	Creates empty DataTree instance.
		*	@return Empty DataTree instance.
		*/
		static DataTree* create();

		/**
		*	Creates DataTree instance.
		*	@param fileName A name of data file to load. Empty string is rejected.
		*	@param filePath A optional path for the file to read. If path is empty, then it uses default path.
		*	@return A DataTree instance. Returns nullptr if rejected.
		*/
		static DataTree* create(const std::string& fileName);

		//* Destructor. 
		~DataTree();

		// Prvents copying and assigning
		DataTree(DataTree const&) = delete;
		void operator=(DataTree const&) = delete;

		/**
		*	Gets integer value from key provided.
		*	@param key A key to query data file.
		*	@return Integer value of key or 0 if key is invalid.
		*/
		const int getInt(const std::string& key);

		/**
		*	Gets float value from key provided.
		*	@param key A key to query data file.
		*	@return Float value of key or 0.0f if key is invalid.
		*/
		const float getFloat(const std::string& key);

		/**
		*	Gets double value from key provided.
		*	@param key A key to query data file.
		*	@return double value of key or 0.0 if key is invalid.
		*/
		const double getDouble(const std::string& key);

		/**
		*	Gets bool value from key provided.
		*	@note There are other aliases for boolean such as 'yes' which is same as true.
		*	@param key A key to query data file.
		*	@return bool value of key or false if key is invalid.
		*/
		const bool getBool(const std::string& key);

		/**
		*	Gets string value from key provided.
		*	@param key A key to query data file.
		*	@return String value of key or empty string if key is invalid
		*/
		const std::string getString(const std::string& key);

		/**
		*	Sets int value to a key in data file
		*	@param key A key to insert data
		*	@param value A int value to insert
		*	@param overwrite True to overwrite if key already exists. Else, false. True by default.
		*/
		const bool setInt(const std::string& key, const int value, const bool overwrite = true);

		/**
		*	Sets int value to a key in data file
		*	@param key A key to insert data
		*	@param value A string int value to insert
		*	@param overwrite True to overwrite if key already exists. Else, false. True by default.
		*/
		const bool setInt(const std::string& key, const std::string& value, const bool overwrite = true);

		/**
		*	Sets float value to a key in data file
		*	@param key A key to insert data
		*	@param value A float value to insert
		*	@param overwrite True to overwrite if key already exists. Else, false. True by default.
		*/
		const bool setFloat(const std::string& key, const float value, const bool overwrite = true);

		/**
		*	Sets float value to a key in data file
		*	@param key A key to insert data
		*	@param value A string float value to insert
		*	@param overwrite True to overwrite if key already exists. Else, false. True by default.
		*/
		const bool setFloat(const std::string& key, const std::string& value, const bool overwrite = true);

		/**
		*	Sets double value to a key in data file
		*	@param key A key to insert data
		*	@param value A double value to insert
		*	@param overwrite True to overwrite if key already exists. Else, false. True by default.
		*/
		const bool setDouble(const std::string& key, const double value, const bool overwrite = true);

		/**
		*	Sets double value to a key in data file
		*	@param key A key to insert data
		*	@param value A string double value to insert
		*	@param overwrite True to overwrite if key already exists. Else, false. True by default.
		*/
		const bool setDouble(const std::string& key, const std::string& value, const bool overwrite = true);

		/**
		*	Sets bool value to a key in data file
		*	@param key A key to insert data
		*	@param value A bool value to insert
		*	@param overwrite True to overwrite if key already exists. Else, false. True by default.
		*/
		const bool setBool(const std::string& key, const bool value, const bool overwrite = true);

		/**
		*	Sets bool value to a key in data file
		*	@param key A key to insert data
		*	@param value A string bool value to insert
		*	@param overwrite True to overwrite if key already exists. Else, false. True by default.
		*/
		const bool setBool(const std::string& key, const std::string& value, const bool overwrite = true);

		/**
		*	Sets string value to key.
		*	@note Empty key is rejected. Empty value will erase value data on corresponding key.
		*	@return True if successfully sets value.
		*/
		const bool setString(const std::string& key, const std::string& value, const bool overwrite = true);

		/**
		*	Checks if data already has key.
		*	@param key A key to check.
		*	@return True if data has same key. Else, false.
		*/
		const bool hasKey(const std::string key);

		/**
		*	Get key set.
		*	@return String vector of key set. Empty if no children exists.
		*/
		const std::vector<std::string> getKeySet();

		/**
		*	Get key set on specific key point
		*	@param key A key to query key set.
		*	@return String vector of key set. Empty if no children exist or key is invalid.
		*/
		const std::vector<std::string> getKeySet(const std::string& key);

		/**
		*	Get children size.
		*	@return Size of children
		*/
		const int getChildrenSize();

		/**
		*	Saves data to file
		*	@note File will be get overwritten if file with same name exists
		*	@param fileName A name of file to save.
		*/
		void save(const std::string& fileName);

		/**
		*	Converts data to string
		*	@return String data.
		*/
		const std::string toString();

		/**
		*	Clears the data.
		*	@note Unsaved changes will be wiped.
		*/
		void clear();
	};
}

#endif