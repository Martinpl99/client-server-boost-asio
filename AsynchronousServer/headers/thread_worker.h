#pragma once
#include <mutex>
#include<boost/asio.hpp>
#include <boost/thread.hpp>

using namespace boost::asio;

namespace clientServerApp
{
	class thread_worker
	{
	public:
		thread_worker(std::shared_ptr<io_context> A_ioContextPtr, std::mutex& A_mtx);
		~thread_worker();
		void set_number_of_threads(size_t A_numberOfThreads) noexcept;
		std::shared_ptr<boost::thread_group> run_threads();

	private:
		void run_io_context();

		std::shared_ptr<io_context> m_io_contextPtr;
		std::mutex& m_mtx;
		size_t m_number_of_threads = 1;
	};
}