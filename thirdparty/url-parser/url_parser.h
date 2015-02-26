/* 
 * File:   url_parser.h
 * Author: phrk
 *
 * Created on January 23, 2014, 1:41 PM
 */


/*_
 * Copyright 2010 Scyphus Solutions Co. Ltd.  All rights reserved.
 *
 * Authors:
 *      Hirochika Asai
 */

#ifndef _URL_PARSER_H
#define _URL_PARSER_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>

#include "hiaux/strings/string_utils.h"

#include <iostream>

class ParsedUrl {
	
	void doParsing(const std::string &_url);
	
public:
	bool ok;
	std::string scheme;               /* reqired */
	std::string host;                 /* reqired */
	std::string port;                 /* optional */
	std::string path;                 /* optional */
	std::string query;                /* optional */
	std::string fragment;             /* optional */
	std::string username;             /* optional */
	std::string password;             /* optional */
	
	ParsedUrl(const std::string &_url);
	ParsedUrl(const std::string &_url, bool _inverted);
	
	void normalizeTo(std::string &_url);
	std::string withoutParams();
	std::string normalize();
	
	std::string getInverted();
};

typedef boost::shared_ptr<ParsedUrl> ParsedUrlPtr;

#endif	/* URL_PARSER_H */

