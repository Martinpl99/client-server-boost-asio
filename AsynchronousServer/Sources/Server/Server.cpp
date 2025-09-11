#include "../../headers/server/server.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <stdexcept>

using namespace boost::asio;
using namespace boost::system;

namespace clientServerApp
{
	server::server(std::shared_ptr<io_context> A_ioContextPtr, std::string& A_Address, short A_PortNumber, std::mutex& A_mtx)
		: m_ioContextPtr(A_ioContextPtr),
		m_Address(A_Address),
		m_portNumber(static_cast<short>(A_PortNumber)),
		m_mtx(A_mtx)
	{
		create_acceptor();
	}

	void server::create_acceptor()
	{
		error_code errorCode;
		const auto& address = ip::make_address(m_Address, errorCode);

		if (errorCode)
			return;

		m_ptrAcceptor = std::make_shared<ip::tcp::acceptor>(*m_ioContextPtr,
			ip::tcp::endpoint(address, m_portNumber),
			false);
	}

	void server::handle_connection(const error_code& A_errorCode)
	{
		if (!A_errorCode)
		{
			m_mtx.lock();
			std::cout << "Accepted! from thread: "<< std::this_thread::get_id() << "\n";
			m_mtx.unlock();

			auto strand = make_strand(m_ioContextPtr->get_executor());

			auto strand1 = [&](size_t A_strandNumber)
			{
				m_mtx.lock();
				std::cout << "Strand "<< A_strandNumber << "\n";
				m_mtx.unlock();
			};

			strand.post([&] {strand1(1); }, get_associated_allocator(strand1));
			strand.post([&] {strand1(2); }, get_associated_allocator(strand1));
			strand.post([&] {strand1(3); }, get_associated_allocator(strand1));
			strand.post([&] {strand1(4); }, get_associated_allocator(strand1));
			strand.post([&] {strand1(5); }, get_associated_allocator(strand1));

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