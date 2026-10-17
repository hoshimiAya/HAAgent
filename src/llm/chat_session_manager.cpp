#include "chat_session_manager.hpp"

#include <mutex>
#include <random>
#include <string_view>

std::shared_ptr<ChatContext> ChatSessionManager::get_or_create_session(const std::string& session_id)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto [it, inserted] = sessions_.try_emplace(session_id);
    auto& chat_context = it->second;
    if (inserted) {
        chat_context = std::make_shared<ChatContext>();
        chat_context->session_id = session_id;
    }
    return chat_context;
}

std::string ChatSessionManager::generate_session_id()
{
    thread_local std::mt19937 gen(std::random_device{}());
    thread_local std::uniform_int_distribution<> dis(0, 15);

    static constexpr std::string_view hex = "0123456789abcdef";
    std::string id;
    id.reserve(32);
    for (int i = 0; i < 32; i++) {
        id += hex[dis(gen)];
    }

    return id;
}

void ChatSessionManager::cleanup_expired()
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();

    for (auto session_it = sessions_.begin(); session_it != sessions_.end();) {
        auto& context = session_it->second;
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - context->last_time).count();
        if (elapsed > 300) {
            session_it = sessions_.erase(session_it);
        }
        else {
            session_it++;
        }
    }
}

size_t ChatSessionManager::session_count() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return sessions_.size();
}
