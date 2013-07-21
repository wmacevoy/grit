#pragma once

#include <string>
#include <vector>

//
// read comma-separated-value file file looking for columns matching
// comma-seperated value string heading_str (possibly with additional
// columns and in a different order), then read all the rows below that
// into table according to the heading_str heading order.  The end of
// the table is the end of the csv file or the first row with an empty
// value for the the first heading.
//
// note: implementation does not currently support quoted values....
//
bool CSVRead(const std::string &file, const std::string &heading_str, 
	     std::vector < std::vector < std::string > > &table);

//
// As above, but converts all values to doubles... (or 0 if non-numeric)
//
bool CSVRead(const std::string &file, const std::string &heading_str, 
	     std::vector < std::vector < double > > &table);
