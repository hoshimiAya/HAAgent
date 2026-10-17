#include "http_client.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = asio::ip::tcp;

HttpClient::HttpClient(const std::string& host, const std::string& port)
	: host_(host)
	, port_(port)
{
}

std::string HttpClient::post(const std::string& target, const std::map<std::string, std::string>& headers, const std::string& body)
{
	try
	{
		asio::io_context io;
		tcp::resolver resolver(io);
		beast::tcp_stream stream(io);

		auto const results = resolver.resolve(host_, port_);
		stream.connect(results);

		http::request<http::string_body> request(http::verb::post, target, 11);
		request.set(http::field::host, host_);
		request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
		for (const auto& [key, value] : headers) {
			request.set(key, value);
		}
		request.body() = body;
		request.prepare_payload();

		http::write(stream, request);

		beast::flat_buffer buffer;
		http::response<http::string_body> response;
		http::read(stream, buffer, response);

		beast::error_code ec;
		stream.socket().shutdown(tcp::socket::shutdown_both, ec);
		if (ec && ec != beast::errc::not_connected) {
			// donothing
		}

		if (response.result() != http::status::ok) {
			throw std::runtime_error("HTTP Error: " + std::to_string(response.result_int()));
		}
		return response.body();
	}
	catch (const std::exception& e)
	{
		throw std::runtime_error(std::string("HttpClient::post failed: ") + e.what());
	}
}
