#include<boost/asio.hpp>
using namespace boost::asio;
using namespace boost::system;

namespace clientServerApp
{
	class server
	{
	public:
		server(io_context& A_ioContext, const ip::tcp::endpoint& A_endpoint);

	private:
		void do_accept();
		void handle_connection(const error_code& A_errorCode, ip::tcp::socket A_socket);

		std::shared_ptr<ip::tcp::acceptor> m_ptrAcceptor;
	};
}
