#include <string>
#include <iostream>

#include "chat_client.hpp"

int main(int argc, char** argv) {
	std::string host = argc > 1 ? argv[1] : "127.0.0.1";
	std::string port = argc > 2 ? argv[2] : "18080";

	try
	{
		ChatClient client(host, port);
		client.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Fatal: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}