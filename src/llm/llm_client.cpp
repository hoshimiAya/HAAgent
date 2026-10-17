#include "llm_client.hpp"

#include <string>
#include <vector>

LLMClient::LLMClient(const std::string& api_key) : api_key_(api_key)
{
}

bool LLMClient::is_configured() const
{
	return !api_key_.empty();
}

std::string LLMClient::complete(const std::vector<std::pair<std::string, std::string>>& message)
{
	return llm_response(message);
}

std::string LLMClient::llm_response(const std::vector<std::pair<std::string, std::string>>& message)
{
	return std::string();
}
