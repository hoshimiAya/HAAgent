#include "server.hpp"

#include <iostream>

#include "session.hpp"

Server::Server(asio::io_context& io, unsigned short port, const Router& router)
	: acceptor_(io, tcp::endpoint(tcp::v4(), port))
	, router_(router)
{
	do_accept();
}

void Server::do_accept()
{
	auto socket = std::make_shared<tcp::socket>(acceptor_.get_executor());
	acceptor_.async_accept(
		*socket,
		[this, socket](boost::system::error_code ec)
		{
			if (!ec)
			{
				std::cout << "New connection accepted." << std::endl;
				std::make_shared<Session>(std::move(*socket), router_)->run();
			}
			else
			{	
				std::cerr << "Accept error: " << ec.message() << std::endl;
			}

			// 继续接受新的连接
			do_accept();
		}
	);
}
