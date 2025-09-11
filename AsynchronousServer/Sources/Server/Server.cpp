#include "../../headers/server/server.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>

using namespace boost::asio;
using namespace boost::system;

namespace clientServerApp
{
	server::server(io_context& A_ioContext, std::string& A_Address, short A_PortNumber)
		: m_ioContext(A_ioContext), m_Address(A_Address), m_portNumber(static_cast<short>(A_PortNumber))
	{
		create_connector();
	}

	void server::create_connector()
	{
		error_code errorCode;
		const auto& address = ip::make_address(m_Address, errorCode);

		if (errorCode)
			return;

		m_ptrAcceptor = std::make_shared<ip::tcp::acceptor>(m_ioContext,
			ip::tcp::endpoint(address, m_portNumber),
			false);
	}

	void server::handle_connection(const error_code& A_errorCode)
	{
		if (!A_errorCode)
		{
			std::cout << "Accepted! from thread: "<< std::this_thread::get_id() << "\n";
			do_accept();
			std::this_thread::sleep_for(chrono::seconds(10));
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
				handle_connection(A_errorCode);
			});
	}
}