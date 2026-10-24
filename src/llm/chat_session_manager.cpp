#include "chat_session_manager.hpp"

#include <mutex>
#include <random>
#include <string_view>
#include <memory>

#include "../log.hpp"

std::shared_ptr<ChatContext> ChatSessionManager::get_or_create_session(const std::string& session_id)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto [it, inserted] = sessions_.try_emplace(session_id);
    auto& chat_context = it->second;
    if (inserted) {
        chat_context = std::make_shared<ChatContext>();
        chat_context->session_id = session_id;
        LOG_DBG("session.create id={} total={}",
            logging::short_id(session_id), sessions_.size());
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

    std::size_t erased = 0;
    for (auto session_it = sessions_.begin(); session_it != sessions_.end();) {
        auto& context = session_it->second;
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - context->last_time).count();
        if (elapsed > 300) {
            session_it = sessions_.erase(session_it);
            erased++;
        }
        else {
            session_it++;
        }
    }
    if (erased > 0) {
        LOG_INF("session.cleanup erased={} remain={}", erased, sessions_.size());
    }
}

size_t ChatSessionManager::session_count() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return sessions_.size();
}

std::vector<std::pair<std::string, std::string>> ChatSessionManager::get_history(const std::string& session_id)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto context_it = sessions_.find(session_id);
    if (context_it == sessions_.end()) {
        return {};
    }

    auto& context = context_it->second;
    context->last_time = std::chrono::steady_clock::now();
    return context->history;
}

void ChatSessionManager::set_summary(const std::string& session_id, const std::string& summary)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto& context = sessions_[session_id];
    if (!context) {
        context = std::make_shared<ChatContext>();
        context->session_id = session_id;
    }
    context->summary = summary;
    context->last_time = std::chrono::steady_clock::now();
}

std::string ChatSessionManager::get_summary(const std::string& session_id)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto context_it = sessions_.find(session_id);
    if (context_it == sessions_.end()) {
        return {};
    }

    auto& context = context_it->second;
    context->last_time = std::chrono::steady_clock::now();
    return context->summary;
}

std::vector<std::pair<std::string, std::string>> ChatSessionManager::append_turn(const std::string& session_id, const std::string& user_message, const std::string& assistant_message)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto& context = sessions_[session_id];
    if (!context) {
        context = std::make_shared<ChatContext>();
        context->session_id = session_id;
    }

    context->history.emplace_back("user", user_message);
    context->history.emplace_back("assistant", assistant_message);

    std::vector<std::pair<std::string, std::string>> overflow;
    constexpr std::size_t maxHistoryMessages = 20;
    if (context->history.size() > maxHistoryMessages) {
        auto n = static_cast<std::ptrdiff_t>(context->history.size() - maxHistoryMessages);
        auto end = std::next(context->history.begin(), n);
        overflow.reserve(n);
        overflow.assign(std::make_move_iterator(context->history.begin()), std::make_move_iterator(end));
        context->history.erase(context->history.begin(), end);
    }

    context->last_time = std::chrono::steady_clock::now();
    return overflow;
}
