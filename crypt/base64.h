#include <string>
#include <vector>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);
std::vector<char> base64_decode_vec(std::string const& s);

char *unbase64(unsigned char *input, int length, int *_outlength);