#include "session.hpp"

#include <iostream>

namespace asio = boost::asio;

Session::Session(tcp::socket socket, const Router& router)
	: stream_(std::move(socket))
	, router_(router)
{
}

void Session::run()
{
	asio::dispatch(
		stream_.get_executor(),
		beast::bind_front_handler(&Session::do_read, shared_from_this())
	);
}

void Session::do_read()
{
	request_ = {};
	stream_.expires_after(std::chrono::seconds(30));

	http::async_read(
		stream_, buffer_, request_,
		beast::bind_front_handler(&Session::on_read, shared_from_this())
	);
}

void Session::on_read(beast::error_code ec, std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);

	if (ec == http::error::end_of_stream) {
		return do_close();
	}
	if (ec) {
		std::cerr << "Read error: " << ec.message() << std::endl;
		return;
	}

	handle_request(std::move(request_));
}

void Session::handle_request(http::request<http::string_body>&& request)
{
	const bool keep_alive = request.keep_alive();
	const std::string method(request.method_string());
	const std::string target(request.target());

	std::string path = target;
	std::string query_params;
	size_t query_pos = target.find("?");
	if (query_pos != std::string::npos) {
		path = target.substr(0, query_pos);
		query_params = target.substr(query_pos + 1);
	}

	http::response<http::string_body> response{ http::status::ok, request.version() };
	response.set(http::field::content_type, "application/json");

	auto match = router_.match(method, path);
	if (match) {
		response = match->handler(request);
	}
	else {
		response.result(http::status::not_found);
		response.body() = R"({"error":"not found"})";
	}

	response.keep_alive(keep_alive);
	response.prepare_payload();

	std::cout << "Request: " << method << " " << path
		<< " -> " << response.result_int() << std::endl;

	do_write(std::move(response));
}

void Session::do_write(http::response<http::string_body>&& response)
{
	response_ = std::move(response);
	const bool keep_alive = response_.keep_alive();

	http::async_write(
		stream_, response_,
		beast::bind_front_handler(&Session::on_write, shared_from_this(), keep_alive)
	);
}

void Session::do_close()
{
	beast::error_code ec;
	stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
}

void Session::on_write(bool keep_alive, beast::error_code ec, std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);

	if (ec) {
		std::cerr << "Write error: " << ec.message() << std::endl;
		return;
	}

	if (!keep_alive) {
		return do_close();
	}
	do_read();
}