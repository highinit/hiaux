
/*_
 * Copyright 2010-2011 Scyphus Solutions Co. Ltd.  All rights reserved.
 *
 * Authors:
 *      Hirochika Asai
 */

#include "url_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Prototype declarations
 */
static __inline__ int _is_scheme_char(int);

/*
 * Check whether the character is permitted in scheme string
 */
static __inline__ int
_is_scheme_char(int c)
{
    return (!isalpha(c) && '+' != c && '-' != c && '.' != c) ? 0 : 1;
}

bool isUrlChar(char t)
{
	if ('a' <= t && t <= 'z') return true;
	if ('A' <= t && t <= 'Z') return true;
	if ('0' <= t && t <= '9') return true;
	if (t==':' || t=='.' || t=='/' || t=='-' || t=='_' \
			|| t=='&' || t=='?' || t=='%' || t=='=' || t=='#')
		return true;
	
	//std::cout << "WRONG CHAR: |" << t << "|\n"; 
	
	return false;
}

bool checkUrlCorrect(const std::string &_url)
{
	/*
	boost::regex url_regex ("(http|https)://(\\w+\\.)*(\\w*)\\/([\\w\\d]+/{0,1})+"); 
	boost::smatch results;
	if (boost::regex_match(_url, results, url_regex)) {
		std::cout << "______URL_CORRECT:" << _url << std::endl;
		return true;
	} else {
		std::cout << "______URL_NOT_CORRECT:" << _url << std::endl;;
		return false;
	}
	*/
	for (int i = 0; i<_url.size(); i++)
		if (!isUrlChar(_url[i]))
			return false;
	return true;
	
}

ParsedUrl::ParsedUrl(const std::string &_url, bool _inverted) {
	
	doParsing(_url);
	if (_inverted)
		host = invertDomain(host);
}

/*
 * See RFC 1738, 3986
 */
ParsedUrl::ParsedUrl(const std::string &_url) {
	
	doParsing(_url);
}

void ParsedUrl::doParsing(const std::string &_url) {
	
	if (!checkUrlCorrect(_url)) {
			ok = false;
			//std::cout << "______URL_NOT_CORRECT:" << _url << std::endl;
			return;
		}
		
	const char *tmpstr;
	//const char *curstr;
	int len;
	int i;
	int userpass_flag;
	int bracket_flag;

	const char* curstr  = _url.data(); // FIXME to const pointer to _url.data()
	//strcpy(curstr, _url.c_str());

	/*
	 * <scheme>:<scheme-specific-part>
	 * <scheme> := [a-z\+\-\.]+
	 *             upper case = lower case for resiliency
	 */
	/* Read scheme */
	tmpstr = strchr(curstr, ':');
	if ( NULL == tmpstr ) {
		/* Not found the character */
		//http_parsed_url_free(purl);
		ok = false;
		return;
	}
	/* Get the scheme length */
	len = tmpstr - curstr;
	/* Check restrictions */
	for ( i = 0; i < len; i++ ) {
		if ( !_is_scheme_char(curstr[i]) ) {
			/* Invalid format */
			ok = false;
			return;
		}
	}
	/* Copy the scheme to the storage */
	char scheme_bf [len + 1];
	
	(void)strncpy(scheme_bf, curstr, len);
	scheme_bf[len] = '\0';
	/* Make the character to lower if it is upper case. */
	for ( i = 0; i < len; i++ ) {
		scheme_bf[i] = tolower(scheme_bf[i]);
	}
	scheme = std::string(scheme_bf);
	
	/* Skip ':' */
	tmpstr++;
	curstr = tmpstr;

	/*
	 * //<user>:<password>@<host>:<port>/<url-path>
	 * Any ":", "@" and "/" must be encoded.
	 */
	/* Eat "//" */
	for ( i = 0; i < 2; i++ ) {
		if ( '/' != *curstr ) {
			ok = false;
			return;
		}
		curstr++;
	}

	/* Check if the user (and password) are specified. */
	userpass_flag = 0;
	tmpstr = curstr;
	while ( '\0' != *tmpstr ) {
		if ( '@' == *tmpstr ) {
			/* Username and password are specified */
			userpass_flag = 1;
			break;
		} else if ( '/' == *tmpstr ) {
			/* End of <host>:<port> specification */
			userpass_flag = 0;
			break;
		}
		tmpstr++;
	}

	/* User and password specification */
	
	tmpstr = curstr;
	if ( userpass_flag ) {
		/* Read username */
		while ( '\0' != *tmpstr && ':' != *tmpstr && '@' != *tmpstr ) {
			tmpstr++;
		}
		len = tmpstr - curstr;
		char username_bf [ len + 1 ];
		
		strncpy(username_bf, curstr, len);
		username_bf[len] = '\0';
		username = std::string(username_bf);
		
		/* Proceed current pointer */
		curstr = tmpstr;
		if ( ':' == *curstr ) {
			/* Skip ':' */
			curstr++;
			/* Read password */
			tmpstr = curstr;
			while ( '\0' != *tmpstr && '@' != *tmpstr ) {
				tmpstr++;
			}
			len = tmpstr - curstr;
			char password_bf [ len + 1 ];
			strncpy(password_bf, curstr, len);
			password_bf[len] = '\0';
			password = std::string(password_bf);
			curstr = tmpstr;
		}
		/* Skip '@' */
		if ( '@' != *curstr ) {
			ok = false;
			return;
		}
		curstr++;
	}

	if ( '[' == *curstr ) {
		bracket_flag = 1;
	} else {
		bracket_flag = 0;
	}
	/* Proceed on by delimiters with reading host */
	tmpstr = curstr;
	while ( '\0' != *tmpstr ) {
		if ( bracket_flag && ']' == *tmpstr ) {
			/* End of IPv6 address. */
			tmpstr++;
			break;
		} else if ( !bracket_flag && (':' == *tmpstr || '/' == *tmpstr) ) {
			/* Port number is specified. */
			break;
		}
		tmpstr++;
	}
	len = tmpstr - curstr;
	char host_bf [ len + 1 ];
	if (  len <= 0 ) {
		ok = false;
		return;
	}
	strncpy(host_bf, curstr, len);
	host_bf[len] = '\0';
	host = std::string(host_bf);
	curstr = tmpstr;

	/* Is port number specified? */
	if ( ':' == *curstr ) {
		curstr++;
		/* Read port number */
		tmpstr = curstr;
		while ( '\0' != *tmpstr && '/' != *tmpstr ) {
			tmpstr++;
		}
		len = tmpstr - curstr;
		char port_bf [ len + 1 ];
		strncpy(port_bf, curstr, len);
		port_bf[len] = '\0';
		port = std::string(port_bf);
		curstr = tmpstr;
	}

	/* End of the string */
	if ( '\0' == *curstr ) {
		ok = true;
		return;
	}

	/* Skip '/' */
	if ( '/' != *curstr ) {
		ok = false;
		return;
	}
	curstr++;

	/* Parse path */
	tmpstr = curstr;
	while ( '\0' != *tmpstr && '#' != *tmpstr  && '?' != *tmpstr ) {
		tmpstr++;
	}
	len = tmpstr - curstr;
	char path_bf [ len + 1 ];
	
	strncpy(path_bf, curstr, len);
	path_bf[len] = '\0';
	path = std::string(path_bf);
	curstr = tmpstr;

	/* Is query specified? */
	if ( '?' == *curstr ) {
		/* Skip '?' */
		curstr++;
		/* Read query */
		tmpstr = curstr;
		while ( '\0' != *tmpstr && '#' != *tmpstr ) {
			tmpstr++;
		}
		len = tmpstr - curstr;
		char query_bf [ len + 1 ];
		strncpy(query_bf, curstr, len);
		query_bf[len] = '\0';
		query = std::string(query_bf);
		curstr = tmpstr;
	}

	/* Is fragment specified? */
	if ( '#' == *curstr ) {
		/* Skip '#' */
		curstr++;
		/* Read fragment */
		tmpstr = curstr;
		while ( '\0' != *tmpstr ) {
			tmpstr++;
		}
		len = tmpstr - curstr;
		char fragment_bf [ len + 1 ];
		strncpy(fragment_bf, curstr, len);
		fragment_bf[len] = '\0';
		fragment = std::string(fragment_bf);
		curstr = tmpstr;
	}

	ok = 1;
}

std::string ParsedUrl::withoutParams() {
	
	return scheme + "://" + host + "/" + path;
}

void ParsedUrl::normalizeTo(std::string &_url)
{
	_url = scheme + "://" + host + "/" + path;
	if (query != "")
		_url += "?" + query;
}

std::string ParsedUrl::normalize()
{
	std::string url;
	normalizeTo(url);
	return url;
}

std::string ParsedUrl::getInverted() {
	
	std::string ret = scheme;
	ret.append("://");
	ret.append(invertDomain(host));
	
	if (port.size() != 0) {
		ret.append(":");
		ret.append(port);
	}
	
	if (path.size() != 0) {
		ret.append("/");
		ret.append(path);
	}
	
	if (query != "")
		ret += "?" + query;
	
	return ret;
}
