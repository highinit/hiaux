
#include <cxxtest/TestSuite.h>

#include "../hpoolserver.h"
#include "../websocket/WebSocketSrv.h"

#include "../../hrpc/hcomm/include/hsock.h"

#include "../../common/string_utils.h"

/* UTF-8 to ISO-8859-1/ISO-8859-15 mapper.
 * Return 0..255 for valid ISO-8859-15 code points, 256 otherwise.
*/
static inline unsigned int to_latin9(const unsigned int code)
{
    /* Code points 0 to U+00FF are the same in both. */
    if (code < 256U)
        return code;
    switch (code) {
    case 0x0152U: return 188U; /* U+0152 = 0xBC: OE ligature */
    case 0x0153U: return 189U; /* U+0153 = 0xBD: oe ligature */
    case 0x0160U: return 166U; /* U+0160 = 0xA6: S with caron */
    case 0x0161U: return 168U; /* U+0161 = 0xA8: s with caron */
    case 0x0178U: return 190U; /* U+0178 = 0xBE: Y with diaresis */
    case 0x017DU: return 180U; /* U+017D = 0xB4: Z with caron */
    case 0x017EU: return 184U; /* U+017E = 0xB8: z with caron */
    case 0x20ACU: return 164U; /* U+20AC = 0xA4: Euro */
    default:      return 256U;
    }
}

/* Convert an UTF-8 string to ISO-8859-15.
 * All invalid sequences are ignored.
 * Note: output == input is allowed,
 * but   input < output < input + length
 * is not.
 * Output has to have room for (length+1) chars, including the trailing NUL byte.
*/
size_t utf8_to_latin9(char *const output, const char *const input, const size_t length)
{
    unsigned char             *out = (unsigned char *)output;
    const unsigned char       *in  = (const unsigned char *)input;
    const unsigned char *const end = (const unsigned char *)input + length;
    unsigned int               c;

    while (in < end)
        if (*in < 128)
            *(out++) = *(in++); /* Valid codepoint */
        else
        if (*in < 192)
            in++;               /* 10000000 .. 10111111 are invalid */
        else
        if (*in < 224) {        /* 110xxxxx 10xxxxxx */
            if (in + 1 >= end)
                break;
            if ((in[1] & 192U) == 128U) {
                c = to_latin9( (((unsigned int)(in[0] & 0x1FU)) << 6U)
                             |  ((unsigned int)(in[1] & 0x3FU)) );
                if (c < 256)
                    *(out++) = c;
            }
            in += 2;

        } else
        if (*in < 240) {        /* 1110xxxx 10xxxxxx 10xxxxxx */
            if (in + 2 >= end)
                break;
            if ((in[1] & 192U) == 128U &&
                (in[2] & 192U) == 128U) {
                c = to_latin9( (((unsigned int)(in[0] & 0x0FU)) << 12U)
                             | (((unsigned int)(in[1] & 0x3FU)) << 6U)
                             |  ((unsigned int)(in[2] & 0x3FU)) );
                if (c < 256)
                    *(out++) = c;
            }
            in += 3;

        } else
        if (*in < 248) {        /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
            if (in + 3 >= end)
                break;
            if ((in[1] & 192U) == 128U &&
                (in[2] & 192U) == 128U &&
                (in[3] & 192U) == 128U) {
                c = to_latin9( (((unsigned int)(in[0] & 0x07U)) << 18U)
                             | (((unsigned int)(in[1] & 0x3FU)) << 12U)
                             | (((unsigned int)(in[2] & 0x3FU)) << 6U)
                             |  ((unsigned int)(in[3] & 0x3FU)) );
                if (c < 256)
                    *(out++) = c;
            }
            in += 4;

        } else
        if (*in < 252) {        /* 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
            if (in + 4 >= end)
                break;
            if ((in[1] & 192U) == 128U &&
                (in[2] & 192U) == 128U &&
                (in[3] & 192U) == 128U &&
                (in[4] & 192U) == 128U) {
                c = to_latin9( (((unsigned int)(in[0] & 0x03U)) << 24U)
                             | (((unsigned int)(in[1] & 0x3FU)) << 18U)
                             | (((unsigned int)(in[2] & 0x3FU)) << 12U)
                             | (((unsigned int)(in[3] & 0x3FU)) << 6U)
                             |  ((unsigned int)(in[4] & 0x3FU)) );
                if (c < 256)
                    *(out++) = c;
            }
            in += 5;

        } else
        if (*in < 254) {        /* 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
            if (in + 5 >= end)
                break;
            if ((in[1] & 192U) == 128U &&
                (in[2] & 192U) == 128U &&
                (in[3] & 192U) == 128U &&
                (in[4] & 192U) == 128U &&
                (in[5] & 192U) == 128U) {
                c = to_latin9( (((unsigned int)(in[0] & 0x01U)) << 30U)
                             | (((unsigned int)(in[1] & 0x3FU)) << 24U)
                             | (((unsigned int)(in[2] & 0x3FU)) << 18U)
                             | (((unsigned int)(in[3] & 0x3FU)) << 12U)
                             | (((unsigned int)(in[4] & 0x3FU)) << 6U)
                             |  ((unsigned int)(in[5] & 0x3FU)) );
                if (c < 256)
                    *(out++) = c;
            }
            in += 6;

        } else
            in++;               /* 11111110 and 11111111 are invalid */

    /* Terminate the output string. */
    *out = '\0';

    return (size_t)(out - (unsigned char *)output);
}


class hPoolServerTests : public CxxTest::TestSuite
{    

public:
   
    class CheckInitConnection
    {
//       int connected;
    public:     
/*
        void serve(hSockClientInfo cli_info)
        {
            connected++;
        }

        CheckInitConnection()
        {
            connected = 0;
            boost::shared_ptr<hThreadPool> pool(new hThreadPool(4));
            hPoolServer pool_server(pool, boost::bind(&CheckInitConnection::serve, this, _1)); 
            int port = 6666;
            pool_server.start(port);
            pool->run();

            int client = hSock::client("127.0.0.1", port);
            int client2 = hSock::client("127.0.0.1", port);
            sleep(1);
            TS_ASSERT(connected == 2);
        }   

 */
	};
    
    void testCheckInitConnection()      //////////////////////////
    {
        CheckInitConnection();
    }
    
    class CheckSendSimple
    {
    public:
        /*
        void serve(hSockClientInfo cli_info)
        {
            hSendChannel server (cli_info.m_sock);
            std::string str = server.crecv();
            TS_ASSERT(str == "4815162342");           
        }
        
        CheckSendSimple()
        {
            boost::shared_ptr<hThreadPool> pool(new hThreadPool(2));
            hPoolServer pool_server(pool, boost::bind(&CheckSendSimple::serve, this, _1)); 
            int port = 6667;
            pool_server.start(port);
            pool->run();
            sleep(1);
            hSendChannel client ( hSock::client("127.0.0.1", port) );
            client.csend("4815162342");
            sleep(1);
        }*/
    };
    
    void testSendSimple()               //////////////////////////
    {
        CheckSendSimple();
    }
	
	void onFinished()
	{
	
	}
	
	bool getPairGET(const std::string &s, std::pair<std::string, std::string> &kv)
	{
		size_t eq_sym_pos = s.find('=');
		if (eq_sym_pos == std::string::npos)
			return false;
		std::string key = s.substr(0, eq_sym_pos);
		std::string value = s.substr(eq_sym_pos+1, s.size()-eq_sym_pos);
		removeLeadingAndEndingSpaces(key);
		removeLeadingAndEndingSpaces(value);
		kv.first = key;
		kv.second = value;
		return true;
	}
	
	void parseUrl(const std::string &data,
				std::tr1::unordered_map<std::string, std::string> &values_GET)
	{
		std::cout << "\n___PARSE URL___\n";
		if (data.size()<4) return;
		//std::string data = _data.substr(2, _data.size()-3);
		std::vector<char> delims;
		delims.push_back('?');
		delims.push_back('&');
		
		std::vector<std::string> keyvalues;
		split(data, delims, keyvalues);
		for (int i = 0; i<keyvalues.size(); i++) {
			std::pair<std::string, std::string> kv;
			if (getPairGET(keyvalues[i], kv))
				std::cout << "KEY: " << kv.first << " VALUE: " << kv.second << std::endl;
		}
	}
	
	void poolServerHandler(hPoolServer::ConnectionPtr conn)
	{
		std::string bf = "";
		
	//	while (bf.find('\n')==std::string::npos)
		
		conn->recv(bf);
		std::cout << bf << std::endl;
		if (bf!="") {
			std::tr1::unordered_map<std::string, std::string> values_GET;
			
			
			std::vector<std::string> lines;
			split(bf, '\n', lines);
			std::cout << "LINES: " << lines.size() << std::endl;
			if (lines.size()>=1) {
				std::vector<std::string> words;
				split(lines[0], ' ', words);
				std::cout << "WORDS: " << words.size() << std::endl;
				if (words.size()>2)
					parseUrl(words[1], values_GET);
			}
			
			std::string content = "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">"
								"<html><body>SERVER HEIL!</body></html>";

			char content_len_c[50];
			sprintf(content_len_c, "%d", content.size());
			std::string content_len(content_len_c); 
			std::string response = "HTTP/1.1 200 OK\r\n"
						"Content-Type: text/html; charset=utf-8\r\n"
						"Date: Sat, 28 Dec 2013 18:33:30 GMT\r\n"
						"Server: highinit suggest server\r\n"
						"Connection: keep-alive\r\n"
						"Transfer-Encoding: none\r\n"
						"Content-Length: "+content_len+"\n\n"+content+"\r\n";


			char buf[response.size()];
			utf8_to_latin9(buf, response.c_str(), response.size());
			//std::cout << "\n\nRESPONSE: \n" << response;
			conn->send(buf);
		}
		//conn->close();
	}
	
	void testPoolServer()
	{
		try {
		hThreadPool *pool = new hThreadPool(10);
		TaskLauncherPtr launcher (new TaskLauncher(
						pool, 10, boost::bind(&hPoolServerTests::onFinished, this)));
		hPoolServerPtr server(new hPoolServer(launcher,
						boost::bind(&hPoolServerTests::poolServerHandler, this, _1)));
		const int port = 12345;
		server->start(port);
		pool->run();
		launcher->setNoMoreTasks();
		
		//sleep(1);
		/*
			for (int i = 0; i<10; i++) {
				int cli = hSock::client("127.0.0.1", port);
				char bf[255];
				sprintf(bf, "CLIENT SIEG!\n");
				::send(cli, bf, 255, 0);
				::recv(cli, bf, 255, 0);
				std::cout << bf << std::endl;
			}*/
		pool->join();
		}
		catch (std::string *s) {
			std::cout << s->c_str();
		}
		
	}
	
	void XtestWebSocketServer()
	{
		const int port = 12345;
		hThreadPool *pool = new hThreadPool(10);
		TaskLauncherPtr launcher (new TaskLauncher(
						pool, 10, boost::bind(&hPoolServerTests::onFinished, this)));
		
		WebSocketSrvPtr websocket_srv(new WebSocketSrv(launcher));
		websocket_srv->start(port);
		pool->run();
		//launcher->setNoMoreTasks();
		pool->join();
	}
	
	
};
