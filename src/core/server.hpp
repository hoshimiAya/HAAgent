#pragma once
#include <boost/asio.hpp>

#include "../routing/router.hpp"

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Server {
public:
	Server(asio::io_context& io, unsigned short port, const Router& router);

private:
	// 异步接受连接
	void do_accept();

private:
	tcp::acceptor acceptor_;
	const Router& router_;
};