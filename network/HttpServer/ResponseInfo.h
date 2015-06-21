#ifndef _RESPONSE_INFO_H_
#define _RESPONSE_INFO_H_

#include "hiconfig.h"
#include <string>

namespace hiaux {

class ResponseInfo
{
public:
	std::string content_type;
	std::string server_name;
	ResponseInfo(const std::string &_content_type,
				const std::string &_server_name);
};

}

#endif
