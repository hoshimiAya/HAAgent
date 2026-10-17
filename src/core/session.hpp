#pragma once
#include <memory>
#include <array>
#include <string>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>

#include "../routing/router.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
	Session(tcp::socket socket, const Router& router);

	// 启动会话
	void run();

private:
	// 异步读取数据
	void do_read();

	//
	void on_read(beast::error_code ec, std::size_t bytes_transferred);

	// 处理请求
	void handle_request(http::request<http::string_body>&& request);

	// 异步写入数据
	void do_write(http::response<http::string_body>&& response);

	//
	void on_write(bool keep_alive, beast::error_code ec, std::size_t bytes_transferred);

	//
	void do_close();

private:
	beast::tcp_stream stream_;
	beast::flat_buffer buffer_;
	http::request<http::string_body> request_;
	http::response<http::string_body> response_;

	const Router& router_;
};