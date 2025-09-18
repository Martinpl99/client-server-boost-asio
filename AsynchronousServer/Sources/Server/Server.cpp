#include "../../headers/server/server.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <stdexcept>
#include "../../headers/server/session.h"

using namespace boost::asio;
using namespace boost::system;

namespace clientServerApp
{
	server::server(std::shared_ptr<io_context> A_ioContextPtr, std::string& A_Address, short A_PortNumber, std::mutex& A_mtx)
		: m_ioContextPtr(A_ioContextPtr),
		  m_mtx(A_mtx)
	{
		create_acceptor(A_ioContextPtr, A_Address, A_PortNumber);
	}

	void server::create_acceptor(std::shared_ptr<io_context> A_ioContextPtr, std::string& A_Address, short A_PortNumber)
	{
		error_code errorCode;
		const auto& address = ip::make_address(A_Address, errorCode);

		if (errorCode)
			return;

		m_ptrAcceptor = std::make_shared<ip::tcp::acceptor>(*m_ioContextPtr,
			ip::tcp::endpoint(address, A_PortNumber),
			false);
	}

	void server::handle_connection(const error_code& A_errorCode, ip::tcp::socket A_socket)
	{
		if (!A_errorCode)
		{
			m_mtx.lock();
			std::cout << "Accepted! from thread: "<< std::this_thread::get_id() << "\n";
			m_mtx.unlock();

			std::make_shared<clientServerApp::session>(std::move(A_socket), m_mtx)->run();

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