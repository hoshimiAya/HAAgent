#include "llm_client.hpp"

#include <string>
#include <vector>
#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

LLMClient::LLMClient(const std::string& host, const std::string& port) 
	: http_client_(host, port)
{
}

std::string LLMClient::complete(const std::vector<std::pair<std::string, std::string>>& message)
{
	return llm_response(message);
}

std::string LLMClient::llm_response(const std::vector<std::pair<std::string, std::string>>& message)
{
	try
	{
		json request_json;
		json messages_array = json::array();
		for (const auto& [role, content] : message) {
			messages_array.push_back({
				{"role", role},
				{"content", content}
			});
		}
		request_json["messages"] = messages_array;

		std::string request_body = request_json.dump();

		std::map<std::string, std::string> headers = {
			{"Content-Type", "application/json"}
		};

		std::string response_body = http_client_.post("/v1/chat/completions", headers, request_body);

		json response_json = json::parse(response_body);

		// 提取 choices[0].message.content
		if (response_json.contains("choices") && response_json["choices"].is_array() && !response_json["choices"].empty()) {
			return response_json["choices"][0]["message"]["content"].get<std::string>();
		}
		else {
			std::cerr << "LLM Response format error: " << response_body << std::endl;
			return "Error: Invalid response format";
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "LLMClient exception: " << e.what() << std::endl;
		return std::string("Error: ") + e.what();
	}
}
