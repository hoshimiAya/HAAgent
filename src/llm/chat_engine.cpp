#include "chat_engine.hpp"

#include <vector>

ChatEngine::ChatEngine()
	: llm_client_("127.0.0.1", "9931")
{
}

std::string ChatEngine::process(const std::string& summary, const std::vector<std::pair<std::string, std::string>>& history, const std::string& input)
{
	std::vector<std::pair<std::string, std::string>> messages;
	if (!summary.empty()) {
		messages.push_back({ "system", "以下是先前对话的摘要，作为背景：\n" + summary });
	}
	messages.insert(messages.end(), history.begin(), history.end());
	messages.push_back({ "user", input });

	return llm_client_.complete(messages);
}

std::string ChatEngine::summarize(const std::string& old_summary, const std::vector<std::pair<std::string, std::string>>& overflow)
{
	std::vector<std::pair<std::string, std::string>> messages;
	messages.push_back({ "system", "你是一个对话摘要器。把给定对话压缩成简洁要点，保留事实、偏好、任务状态。" });

	std::string user_content;
	if (!old_summary.empty()) {
		user_content += "已有摘要：\n" + old_summary + "\n\n";
	}

	user_content += "新增对话：\n";
	for (const auto& [role, content] : overflow) {
		user_content += role + ": " + content + "\n";
	}
	messages.push_back({ "user", user_content });

	return llm_client_.complete(messages);
}
