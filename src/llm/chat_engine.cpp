#include "chat_engine.hpp"

#include <vector>

ChatEngine::ChatEngine()
	: llm_client_("127.0.0.1", "9931")
{
}

std::string ChatEngine::process(const std::string& input)
{
	std::vector<std::pair<std::string, std::string>> messages;
	messages.push_back({ "user", input });

	std::string reply = llm_client_.complete(messages);
	return reply;
}
