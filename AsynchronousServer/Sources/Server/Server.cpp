#include "../../Headers/Server/Server.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>

using namespace boost::asio;
using namespace boost::system;

namespace clientServerApp
{
	server::server(io_context& A_ioContext,  const ip::tcp::endpoint& A_endpoint)
		: m_ptrAcceptor(std::move(std::make_shared<ip::tcp::acceptor>(A_ioContext, A_endpoint, false)))
	{
		do_accept();
	}

	void server::handle_connection(const error_code& A_errorCode, ip::tcp::socket A_socket)
	{
		if (!A_errorCode)
		{
			std::cout << "Accepted!\n";
			do_accept();
		}
		else
		{
			std::cout << A_errorCode.message() << std::endl;
		}
	}

	void server::do_accept()
	{
		m_ptrAcceptor->async_accept([this](error_code A_errorCode, ip::tcp::socket A_socket)
			{
				handle_connection(A_errorCode, std::move(A_socket));
			});
	}
}