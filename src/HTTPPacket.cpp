#include "HTTPPacket.hpp"
#include "Logger.hpp"


HTTPPacket::HTTPPacket(){
	valid_request = false;
}


std::string HTTPPacket::get_json()
{
	return this->json;
}
size_t HTTPPacket::get_content_length(){
	return this->contentLength;
}

void HTTPPacket::save_line(std::string line)
{
	line = line.erase(line.length() - 1);
	line = line.erase(line.length() - 1);
	if(line.find("HTTP/") != std::string::npos){
		if(line.find("POST") != std::string::npos){
			this->valid_request = true;
			Logger::getInstance().logInfo("HTTPPacket", "Request is valid");
			return;
		}
	}
	std::size_t separator = line.find(":");
	std::string key = line.substr(0, separator);
	std::string value = line.substr(separator+1, std::string::npos);
	headers.push_back(std::make_pair(key, value));
	if(key == "Content-Length"){
		this->contentLength = std::atoi(value.c_str());
		Logger::getInstance().logInfo("HTTPPacket", "Content-Length is " + this->contentLength);
	}
	
}
void HTTPPacket::save_json(std::string json_response)
{
	this->json=json_response;
}

bool HTTPPacket::is_valid_request(){
	return this->valid_request;
}