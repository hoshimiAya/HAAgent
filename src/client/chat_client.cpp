#include "chat_client.hpp"

#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

ChatClient::ChatClient(const std::string& host, const std::string& port)
	: http_client_(host, port)
{
}

void ChatClient::run()
{
	std::cout << "Chat client ready. /help 查看命令，/q 或 /quit 退出。\n" << std::endl;

	std::string line;
	while (true)
	{
		std::cout << "> " << std::flush;
		if (!std::getline(std::cin, line)) {
			break;  // EOF
		}

		if (line == "/q" || line == "/quit") {
			break;
		}
		if (line == "/help") {
			std::cout << "  /q /quit  退出\n"
				<< "  /help        显示帮助\n"
				<< "  其他内容     作为消息发送给 LLM\n" << std::endl;
			continue;
		}
		if (line.empty()) {
			continue;
		}

		json request_json;
		request_json["msg"] = line;

		std::map<std::string, std::string> headers = {
			{ "Content-Type", "application/json" }
		};
		if (!session_id_.empty()) {
			headers["X-Session-Id"] = session_id_;
		}
		
		try
		{
			HttpResponse resp = http_client_.post_full("/chat", headers, request_json.dump());

			auto session = resp["X-Session-Id"];
			if (!session.empty()) {
				session_id_ = std::string(session);
			}

			if (resp.result() != http::status::ok) {
				std::cerr << "[服务端返回 " << resp.result_int() << "] " << resp.body() << std::endl;
				continue;
			}

			// 服务端用字符串拼接生成 JSON，reply 含引号时可能非法，做兜底
			try {
				auto resp_json = json::parse(resp.body());
				if (resp_json.contains("reply") && resp_json["reply"].is_string()) {
					std::cout << resp_json["reply"].get<std::string>() << std::endl;
				}
				else {
					std::cout << resp.body() << std::endl;
				}
			}
			catch (const std::exception&) {
				std::cout << resp.body() << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << "请求失败: " << e.what() << std::endl;
		}
	}
}
