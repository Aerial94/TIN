#ifndef DNS_CHECKER_HTTPPACKET_H
#define DNS_CHECKER_HTTPPACKET_H

#include <vector>
#include <utility>
#include <string>

class HTTPPacket
{
public:
	HTTPPacket();
	void save_line(std::string line);
	void save_json(std::string json);
	std::string get_json();
	int get_content_length();
private:
	std::string json;
	std::vector<std::pair<std::string, std::string>> headers;
	int contentLength;
};


#endif //DNS_CHECKER_HTTPPACKET_H
