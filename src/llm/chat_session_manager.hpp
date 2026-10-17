#pragma once
#include <memory>
#include <mutex>
#include <map>

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

private:
	mutable std::mutex mutex_;
	std::map<std::string, std::shared_ptr<ChatContext>> sessions_;
};