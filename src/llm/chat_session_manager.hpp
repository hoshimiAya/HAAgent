#pragma once
#include <memory>
#include <mutex>
#include <map>
#include <vector>

#include "chat_context.hpp"

class ChatSessionManager {
public:
	// 获取或创建会话
	std::shared_ptr<ChatContext> get_or_create_session(const std::string& session_id);

	// 生成会话ID
	std::string generate_session_id();

	// 清理过期会话 
	void cleanup_expired();

	// 会话的数量
	size_t session_count() const;

	// 读历史对话
	std::vector<std::pair<std::string, std::string>> get_history(const std::string& session_id);

	// 写摘要
	void set_summary(const std::string& session_id, const std::string& summary);

	// 读摘要
	std::string get_summary(const std::string& session_id);

	// 追加一轮对话，返回被窗口挤出的信息
	std::vector<std::pair<std::string, std::string>>
		append_turn(const std::string& session_id, 
			const std::string& user_message, 
			const std::string& assistant_message);

private:
	mutable std::mutex mutex_;
	std::map<std::string, std::shared_ptr<ChatContext>> sessions_;
};