//
// SConfigFileParser.h
//

#ifndef SCONFIGFILEPARSER_H
#define SCONFIGFILEPARSER_H

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

namespace Sung
{

	class ConfigFileParser
	{
	protected:
		std::string myDelimiter;
		std::string myComment;
		std::string mySentry;
		std::map<std::string,std::string> myContents;

		typedef std::map<std::string,std::string>::iterator mapi;
		typedef std::map<std::string,std::string>::const_iterator mapci;

		// Methods
	public:
		ConfigFileParser(std::string filename, std::string delimiter = "=",
			std::string comment = "#", std::string sentry = "EndConfigFile");
		ConfigFileParser();

		// Search for key and read value or optional default value
		template<class T> T read(const std::string& key) const;  // call as read<T>
		template<class T> T read(const std::string& key, const T& value) const;
		template<class T> bool readInto(T& var, const std::string& key) const;
		template<class T>
		bool readInto(T& var, const std::string& key, const T& value) const;

		// Modify keys and values
		template<class T> void add(std::string key, const T& value);
		void remove(const std::string& key);

		// Check whether key exists in configuration
		bool keyExists(const std::string& key) const;

		// Check or change configuration syntax
		std::string getDelimiter() const { return myDelimiter; }
		std::string getComment() const { return myComment; }
		std::string getSentry() const { return mySentry; }
		std::string setDelimiter(const std::string& s)
		{ std::string old = myDelimiter;  myDelimiter = s;  return old; }  
		std::string setComment(const std::string& s)
		{ std::string old = myComment;  myComment = s;  return old; }

		// Write or read configuration
		friend std::ostream& operator<<(std::ostream& os, const ConfigFileParser& cf);
		friend std::istream& operator>>(std::istream& is, ConfigFileParser& cf);

	protected:
		template<class T> static std::string T_as_string(const T& t);
		template<class T> static T string_as_T(const std::string& s);
		static void trim(std::string& s);


		// Exception types
	public:
		struct file_not_found {
			std::string filename;
			file_not_found(const std::string& filename_ = std::string())
				: filename(filename_) {} };
			struct key_not_found {  // thrown only by T read(key) variant of read()
				std::string key;
				key_not_found(const std::string& key_ = std::string())
					: key(key_) {} };
	};

	template<class T>
	std::string ConfigFileParser::T_as_string(const T& t) 
	{
		std::ostringstream ost;
		ost << t;
		return ost.str();
	}

	template<class T>
	T ConfigFileParser::string_as_T(const std::string& s) 
	{
		T t;
		std::istringstream ist(s);
		ist >> t;
		return t;
	}

	template<>
	inline std::string ConfigFileParser::string_as_T<std::string>(const std::string& s) 
	{
		return s;
	}

	template<>
	inline bool ConfigFileParser::string_as_T<bool>(const std::string& s) 
	{
		bool b = true;
		std::string sup = s;

		for( std::string::iterator p = sup.begin(); p != sup.end(); ++p )
			*p = toupper(*p);  // make std::string all caps

		if (sup==std::string("FALSE") || sup==std::string("F") ||
			sup==std::string("NO") || sup==std::string("N") ||
			sup==std::string("0") || sup==std::string("NONE"))
			b = false;

		return b;
	}


	template<class T>
	T ConfigFileParser::read(const std::string& key) const 
	{
		mapci p = myContents.find(key);
		if (p == myContents.end())
			throw key_not_found(key);
		
		return string_as_T<T>(p->second);
	}


	template<class T>
	T ConfigFileParser::read(const std::string& key, const T& value) const
	{
		mapci p = myContents.find(key);
		if (p == myContents.end())
			return value;
		return string_as_T<T>(p->second);
	}


	template<class T>
	bool ConfigFileParser::readInto(T& var, const std::string& key) const
	{
		mapci p = myContents.find(key);
		bool found = (p != myContents.end());
		if (found)
			var = string_as_T<T>(p->second);

		return found;
	}


	template<class T>
	bool ConfigFileParser::readInto(T& var, const std::string& key, const T& value) const 
	{
		mapci p = myContents.find(key);
		bool found = (p != myContents.end());
		if (found)
			var = string_as_T<T>(p->second);
		else
			var = value;

		return found;
	}


	template<class T>
	void ConfigFileParser::add(std::string key, const T& value)
	{
		std::string v = T_as_string(value);
		trim(key);
		trim(v);
		myContents[key] = v;
		return;
	}

}

#endif