#include "HTTPPacket.hpp"

HTTPPacket::HTTPPacket(){}


std::string HTTPPacket::get_json()
{
	return this->json;
}
int HTTPPacket::get_content_length(){
	return this->contentLength;
}

void HTTPPacket::save_line(std::string line)
{
	if(line.find("HTTP/") != std::string::npos){
		if(line.find("POST") != std::string::npos)
			return;
	}
	std::size_t separator = line.find(":");
	std::string key = line.substr(0, separator);
	std::string value = line.substr(separator+1, std::string::npos);
	
	if(key.compare("Content-Length") != 0)
		this->contentLength = std::stoi(value);
	
}
void HTTPPacket::save_json(std::string json_response)
{
	this->json=json_response;
}