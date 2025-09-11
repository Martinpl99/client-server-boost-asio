#include<boost/asio.hpp>
#include "iServer.h"

using namespace boost::asio;
using namespace boost::system;

namespace clientServerApp
{
	class server : public iServer
	{
	public:
		server(io_context& A_ioContext, std::string& A_Address, short A_PortNumber, std::mutex& A_mtx);
		virtual void do_accept() override;

	private:
		virtual void create_connector();
		virtual void handle_connection(const error_code& A_errorCode);

		std::shared_ptr<ip::tcp::acceptor> m_ptrAcceptor;
		io_context& m_ioContext;
		const std::string& m_Address;
		const short m_portNumber;
		std::mutex& m_mtx;
	};
}
