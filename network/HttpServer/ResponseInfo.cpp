#include "ResponseInfo.h"

namespace hiaux {

ResponseInfo::ResponseInfo(const std::string &_content_type,
					const std::string &_server_name):
		content_type(_content_type),
		server_name(_server_name)
{
}

}
