
#include <iostream>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "core/server.hpp"
#include "llm/chat_engine.hpp"
#include "llm/chat_session_manager.hpp"
#include "routing/router.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

using HttpRequest = http::request<http::string_body>;
using HttpResponse = http::response<http::string_body>;

int main() {
    try {
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
                }
                auto ctx = chat_mgr.get_or_create_session(session_id);

                std::string reply = chat_engine.process(request.body());

                response.set(http::field::content_type, "application/json");
                response.set("X-Session-Id", session_id);
                response.body() = R"({"reply":")" + reply + R"("})";
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
		std::cerr << "Exception: " << e.what() << "\n";
	}

    return 0;
}