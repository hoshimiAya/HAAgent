#include "chat_engine.hpp"

#include <iostream>

ChatEngine::ChatEngine() : llm_client_(get_api_key())
{
	if (!is_configured())
	{
		std::cerr << "Error: API Key is not configured." << std::endl;
	}
}

std::string ChatEngine::process(const std::string& input)
{
	return std::string("测试成功");
}

bool ChatEngine::is_configured()
{
	return llm_client_.is_configured();
}

std::string ChatEngine::get_api_key()
{
	return std::string();
}
