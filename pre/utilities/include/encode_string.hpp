#ifndef UTILITIES_ENCODE_STRING_HPP
#define UTILITIES_ENCODE_STRING_HPP

#include <string>
#include <iostream>

namespace utilities {

	// this function removes whitespace, along with any 
	// /* ... */ comment pairs from the input stream.
	//
	void skip_ws_and_comments(std::istream &is);

	// this function returns true if any of the following is true
	//
	//  s is null
	//  s begins with a digit, hyphen (-), or period (.)
  //  s begins with a question mark (?)
	//  s contains a double quote, either parenthesis ( ), or a comma
	//  s contains an ascii value 32 (blank) or less
	//  s contains an ascii value greater than 127
	//  s contains the begin comment sequence /*
	//
	bool oddball_string(const std::string &s);

	//
	//   if (oddbal_string(s)) is not true, encode_string, reports
	//   the string as its literal contents (no encoding).
	//
	//   if the string is oddball, it is encoded in double quotes,
	//   with the following encoding conventions
	//
	//   " becomes \"
	//   "\" becomes "\\"
        //   <newline> becomes \n
	//   <carriage return> becomes \r
	//   <tab>  becomes \t
	//
	//   other characters less than 32 or greater than 127 are
	//   encoded as \xhh, where each h is a lowercase hexadecimal
	//   digit.
	//
	void encode_string(std::ostream &os, const std::string &s);
  void encode_string(std::string &es, const std::string &s);
  std::string encode_string(const std::string &s);

	//
	//  like encode_string, but always encodes the string
	//
	void encode_string_always(std::ostream &os, const std::string &s);
  std::string encode_string_always(const std::string &s);

	//
	// after skipping whitespace and comments, this function extracts
	// a string.  If the first character is a double quote, it is
	// assumed that the string was encoded using the conventions of
	// encode_string_always().  Otherwise, the string is extracted as
	// a whitespace delimited string.
	//
  void decode_string(std::string &s, std::istream &is);
  std::string decode_string(std::istream &is);
  std::string decode_string(const std::string &s);

}

#endif
