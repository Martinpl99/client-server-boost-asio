#pragma once
#include<boost/asio.hpp>
#include<memory>

using namespace boost::asio;
using namespace boost::system;

namespace clientServerApp
{
	class session : public std::enable_shared_from_this<session>
	{
	public:
		session(ip::tcp::socket A_socket, std::mutex& A_mutex);
		void run();

	private:
		void wait_for_request();

		ip::tcp::socket m_socket;
		streambuf m_buffer;
		std::mutex& m_mutex;
	};
}
