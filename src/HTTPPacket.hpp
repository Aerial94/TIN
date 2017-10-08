#ifndef DNS_CHECKER_HTTPPACKET_H
#define DNS_CHECKER_HTTPPACKET_H

#include <string>
#include <utility>
#include <vector>

class HTTPPacket
{
  public:
    HTTPPacket();
    void save_line(std::string line);
    void save_json(std::string json);
    std::string get_json();
    unsigned int get_content_length();
    bool is_valid_request();

  private:
    std::string json;
    std::vector<std::pair<std::string, std::string>> headers;
    unsigned int contentLength;
    bool valid_request;
};

#endif // DNS_CHECKER_HTTPPACKET_H
