
#include <iostream>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

#include "core/server.hpp"
#include "llm/chat_engine.hpp"
#include "llm/chat_session_manager.hpp"
#include "routing/router.hpp"
#include "log.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

using HttpRequest = http::request<http::string_body>;
using HttpResponse = http::response<http::string_body>;

using json = nlohmann::json;

int main() {
    try {
        logging::init();

		asio::io_context io;
		Router router;
        router.add_route("GET", "/", [](const HttpRequest&) -> HttpResponse {
            HttpResponse response;
            response.set(http::field::content_type, "application/json");
            response.body() = R"({"message":"Welcome!"})";
            response.prepare_payload();
            return response;
        });

        router.add_route("GET", "/health", [](const HttpRequest&) -> HttpResponse {
            HttpResponse response;
            response.set(http::field::content_type, "application/json");
            response.body() = R"({"status":"ok"})";
            response.prepare_payload();
            return response;
        });

        // ChatEngine 用闭包注入
        ChatEngine chat_engine;
        ChatSessionManager chat_mgr;

        router.add_route("POST", "/chat",
            [&chat_engine, &chat_mgr](const HttpRequest& request) -> HttpResponse {
                HttpResponse response;

                std::string session_id(request["x-session-id"]);
                if (session_id.empty()) {
                    session_id = chat_mgr.generate_session_id();
                    LOG_INF("new session={}", logging::short_id(session_id));
                }

                std::string user_msg;
                try
                {
                    auto request_json = json::parse(request.body());
                    if (request_json.contains("msg")) {
                        user_msg = request_json["msg"].get<std::string>();
                    }
                    else {
                        user_msg = request.body(); // 没有 msg 字段就传原始内容
                    }

                }
                catch (const std::exception& e)
                {
                    user_msg = request.body();     // 解析失败就传原始内容
                }

                LOG_DBG("req session={} msg_len={} total_sessions={}",
                    logging::short_id(session_id), user_msg.size(),
                    chat_mgr.session_count());

                auto summary = chat_mgr.get_summary(session_id);
                auto history = chat_mgr.get_history(session_id);

                LOG_DBG("ctx session={} history={} has_summary={}",
                    logging::short_id(session_id), history.size(), !summary.empty());

                std::string reply = chat_engine.process(summary, history, user_msg);
                LOG_INF("llm session={} reply_len={}",
                    logging::short_id(session_id), reply.size());

                auto overflow = chat_mgr.append_turn(session_id, user_msg, reply);
                LOG_DBG("append session={} overflow={}",
                    logging::short_id(session_id), overflow.size());

                if (!overflow.empty()) {
                    auto new_summary = chat_engine.summarize(summary, overflow);
                    if (!new_summary.empty()) {
                        chat_mgr.set_summary(session_id, new_summary);
                        LOG_INF("summarize session={} overflow={} summary_len={}",
                            logging::short_id(session_id), overflow.size(),
                            new_summary.size());
                    }
                    else {
                        LOG_WRN("summarize failed session={} overflow_dropped={}",
                            logging::short_id(session_id), overflow.size());
                    }
                }

                response.set(http::field::content_type, "application/json");
                response.set("X-Session-Id", session_id);

                json response_json;
                response_json["reply"] = reply;
                response.body() = response_json.dump();
                response.prepare_payload();
                return response;
        });

		Server server(io, 18080, router);

		std::vector<std::thread> threads;
		auto thread_count = std::thread::hardware_concurrency();
        std::cout << "[i] Starting " << thread_count << " worker threads..." << std::endl;
		
        for (unsigned i = 0; i < thread_count; ++i) {
            threads.emplace_back([&io]() { io.run(); });
        }

        for (auto& t : threads) {
            t.join();
        }
	}
	catch (std::exception& e) {
        LOG_ERR("exception: {}", e.what());
	}

    return 0;
}