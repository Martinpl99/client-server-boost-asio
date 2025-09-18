#include "../../headers/server/session.h"
#include <iostream>

namespace clientServerApp
{
	session::session(ip::tcp::socket A_socket, std::mutex& A_mutex)
		: m_socket(std::move(A_socket)), m_mutex(A_mutex)
	{

	}

	void session::run()
	{
		wait_for_request();
	}

	void session::wait_for_request()
	{
		auto self(shared_from_this());

		async_read_until(m_socket, m_buffer, "\r\n",
			[this, self](error_code ec, std::size_t)
			{
				m_mutex.lock();
				std::cout << "Socket id: " << m_socket.native_handle() << std::endl;
				m_mutex.unlock();

				if (!ec)
				{
					std::string data{
						std::istreambuf_iterator<char>(&m_buffer),
						std::istreambuf_iterator<char>()
					};

					if (data == "c")
						return;

					m_mutex.lock();
					std::cout << data << std::endl;
					m_mutex.unlock();
					wait_for_request();
				}
				else
				{
					m_mutex.lock();
					std::cout << "Error: " << ec << std::endl;
					m_mutex.unlock();
				}
			});
	}
}