// ConfigFile.h
// Class for reading named values from configuration files
// Richard J. Wagner  v2.1  24 May 2004  wagnerr@umich.edu

// Copyright (c) 2004 Richard J. Wagner
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

// Typical usage
// -------------
// 
// Given a configuration file "settings.inp":
//   atoms  = 25
//   length = 8.0  # nanometers
//   name = Reece Surcher
// 
// Named values are read in various ways, with or without default values:
//   ConfigFile config( "settings.inp" );
//   int atoms = config.read<int>( "atoms" );
//   double length = config.read( "length", 10.0 );
//   string author, title;
//   config.readInto( author, "name" );
//   config.readInto( title, "title", string("Untitled") );
// 
// See file example.cpp for more examples.

#ifndef SECPLATFORM_COMMON_CONFIGFILE_H
#define SECPLATFORM_COMMON_CONFIGFILE_H

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <locale>
namespace zl { namespace base {

class ConfigFile {
// Data
protected:
	std::string myDelimiter;  // separator between key and value
	std::string myComment;    // separator between value and comments
	std::string mySentry;     // optional string to signal end of file
	std::map<std::string,std::string> myContents;  // extracted keys and values
	
	typedef std::map<std::string,std::string>::iterator mapi;
	typedef std::map<std::string,std::string>::const_iterator mapci;

// Methods
public:
	ConfigFile( std::string filename,
	            std::string delimiter = "=",
	            std::string comment = "#",
				std::string sentry = "EndConfigFile" )
	    : myDelimiter(delimiter), myComment(comment), mySentry(sentry)
    {
	    // Construct a ConfigFile, getting keys and values from given file
        std::locale old_locale = std::locale::global(std::locale(""));
	    std::ifstream in( filename.c_str() );
        std::locale::global(old_locale);
    	
	    if( !in )
			throw file_not_found( filename ); 
    	
	    in >> (*this);
    }

	ConfigFile()
	    : myDelimiter( std::string(1,'=') ), myComment( std::string(1,'#') )
    {
	    // Construct a ConfigFile without a file; empty
    }
	
	// Search for key and read value or optional default value
	template<class T> T read( const std::string& key ) const;  // call as read<T>
	template<class T> T read( const std::string& key, const T& value ) const;
	template<class T> bool readInto( T& var, const std::string& key ) const;
	template<class T>
	bool readInto( T& var, const std::string& key, const T& value ) const;
	
	// Modify keys and values
	template<class T> void add( std::string key, const T& value );
	void remove( const std::string& key )
    {
	    // Remove key and its value
	    myContents.erase( myContents.find( key ) );
	    return;
    }
	// Check whether key exists in configuration
	bool keyExists( const std::string& key ) const
    {
	    // Indicate whether key is found
	    mapci p = myContents.find( key );
	    return ( p != myContents.end() );
    }

	
	// Check or change configuration syntax
	std::string getDelimiter() const { return myDelimiter; }
	std::string getComment() const { return myComment; }
	std::string getSentry() const { return mySentry; }
	std::string setDelimiter( const std::string& s )
		{ std::string old = myDelimiter;  myDelimiter = s;  return old; }  
	std::string setComment( const std::string& s )
		{ std::string old = myComment;  myComment = s;  return old; }
	
	// Write or read configuration
	friend std::ostream& operator<<( std::ostream& os, const ConfigFile& cf );
	friend std::istream& operator>>( std::istream& is, ConfigFile& cf );
	
protected:
	template<class T> static std::string T_as_string( const T& t );
	template<class T> static T string_as_T( const std::string& s );
	static void trim( std::string& s )
    {
	    // Remove leading and trailing whitespace
	    static const char whitespace[] = " \n\t\v\r\f";
	    s.erase( 0, s.find_first_not_of(whitespace) );
	    s.erase( s.find_last_not_of(whitespace) + 1U );
    }


// Exception types
public:
	struct file_not_found {
		std::string filename;
		file_not_found( const std::string& filename_ = std::string() )
			: filename(filename_) {} };
	struct key_not_found {  // thrown only by T read(key) variant of read()
		std::string key;
		key_not_found( const std::string& key_ = std::string() )
			: key(key_) {} };
};


/* static */
template<class T>
std::string ConfigFile::T_as_string( const T& t )
{
	// Convert from a T to a string
	// Type T must support << operator
	std::ostringstream ost;
	ost << t;
	return ost.str();
}


/* static */
template<class T>
T ConfigFile::string_as_T( const std::string& s )
{
	// Convert from a string to a T
	// Type T must support >> operator
	T t;
	std::istringstream ist(s);
	ist >> t;
	return t;
}


/* static */
template<>
inline std::string ConfigFile::string_as_T<std::string>( const std::string& s )
{
	// Convert from a string to a string
	// In other words, do nothing
	return s;
}


/* static */
template<>
inline bool ConfigFile::string_as_T<bool>( const std::string& s )
{
	// Convert from a string to a bool
	// Interpret "false", "F", "no", "n", "0" as false
	// Interpret "true", "T", "yes", "y", "1", "-1", or anything else as true
	bool b = true;
	std::string sup = s;
	for( std::string::iterator p = sup.begin(); p != sup.end(); ++p )
		*p = (char)toupper(*p);  // make string all caps
	if( sup==std::string("FALSE") || sup==std::string("F") ||
	    sup==std::string("NO") || sup==std::string("N") ||
	    sup==std::string("0") || sup==std::string("NONE") )
		b = false;
	return b;
}


template<class T>
T ConfigFile::read( const std::string& key ) const
{
	// Read the value corresponding to key
	mapci p = myContents.find(key);
	if( p == myContents.end() ) throw key_not_found(key);
	return string_as_T<T>( p->second );
}


template<class T>
T ConfigFile::read( const std::string& key, const T& value ) const
{
	// Return the value corresponding to key or given default value
	// if key is not found
	mapci p = myContents.find(key);
	if( p == myContents.end() ) return value;
	return string_as_T<T>( p->second );
}


template<class T>
bool ConfigFile::readInto( T& var, const std::string& key ) const
{
	// Get the value corresponding to key and store in var
	// Return true if key is found
	// Otherwise leave var untouched
	mapci p = myContents.find(key);
	bool found = ( p != myContents.end() );
	if( found ) var = string_as_T<T>( p->second );
	return found;
}


template<class T>
bool ConfigFile::readInto( T& var, const std::string& key, const T& value ) const
{
	// Get the value corresponding to key and store in var
	// Return true if key is found
	// Otherwise set var to given default
	mapci p = myContents.find(key);
	bool found = ( p != myContents.end() );
	if( found )
		var = string_as_T<T>( p->second );
	else
		var = value;
	return found;
}


template<class T>
void ConfigFile::add( std::string key, const T& value )
{
	// Add a key with given value
	std::string v = T_as_string( value );
	trim(key);
	trim(v);
	myContents[key] = v;
	return;
}


std::ostream& operator<<( std::ostream& os, const ConfigFile& cf )
{
	// Save a ConfigFile to os
	for( ConfigFile::mapci p = cf.myContents.begin();
	     p != cf.myContents.end();
		 ++p )
	{
		os << p->first << " " << cf.myDelimiter << " ";
		os << p->second << std::endl;
	}
	return os;
}

std::istream& operator>>( std::istream& is, ConfigFile& cf )
{
	// Load a ConfigFile from is
	// Read in keys and values, keeping internal whitespace
	typedef std::string::size_type pos;
	const std::string& delim  = cf.myDelimiter;  // separator
	const std::string& comm   = cf.myComment;    // comment
	const std::string& sentry = cf.mySentry;     // end of file sentry
	const pos skip = delim.length();        // length of separator
	
	std::string nextline = "";  // might need to read ahead to see where value ends
	
	while( is || nextline.length() > 0 )
	{
		// Read an entire line at a time
		std::string line;
		if( nextline.length() > 0 )
		{
			line = nextline;  // we read ahead; use it now
			nextline = "";
		}
		else
		{
			std::getline( is, line );
		}
		
		// Ignore comments
		line = line.substr( 0, line.find(comm) );
		
		// Check for end of file sentry
		if( sentry != "" && line.find(sentry) != std::string::npos ) return is;
		
		// Parse the line if it contains a delimiter
		pos delimPos = line.find( delim );
		if( delimPos < std::string::npos )
		{
			// Extract the key
			std::string key = line.substr( 0, delimPos );
			line.replace( 0, delimPos+skip, "" );
			
			// See if value continues on the next line
			// Stop at blank line, next line with a key, end of stream,
			// or end of file sentry
			bool terminate = false;
			while( !terminate && is )
			{
				std::getline( is, nextline );
				terminate = true;
				
				std::string nlcopy = nextline;
				ConfigFile::trim(nlcopy);
				if( nlcopy == "" ) continue;
				
				nextline = nextline.substr( 0, nextline.find(comm) );
				if( nextline.find(delim) != std::string::npos )
					continue;
				if( sentry != "" && nextline.find(sentry) != std::string::npos )
					continue;
				
				nlcopy = nextline;
				ConfigFile::trim(nlcopy);
				if( nlcopy != "" ) line += "\n";
				line += nextline;
				terminate = false;
			}
			
			// Store key and value
			ConfigFile::trim(key);
			ConfigFile::trim(line);
			cf.myContents[key] = line;  // overwrites if key is repeated
		}
	}
	
	return is;
}

} }
#endif  // SECPLATFORM_COMMON_CONFIGFILE_H

// Release notes:
// v1.0  21 May 1999
//   + First release
//   + Template read() access only through non-member readConfigFile()
//   + ConfigurationFileBool is only built-in helper class
// 
// v2.0  3 May 2002
//   + Shortened name from ConfigurationFile to ConfigFile
//   + Implemented template member functions
//   + Changed default comment separator from % to #
//   + Enabled reading of multiple-line values
// 
// v2.1  24 May 2004
//   + Made template specializations inline to avoid compiler-dependent linkage
//   + Allowed comments within multiple-line values
//   + Enabled blank line termination for multiple-line values
//   + Added optional sentry to detect end of configuration file
//   + Rewrote messy trimWhitespace() function as elegant trim()
