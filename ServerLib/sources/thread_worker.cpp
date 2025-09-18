#include"../headers/thread_worker.h"
#include <iostream>

namespace clientServerApp
{
	thread_worker::thread_worker(std::shared_ptr<io_context> A_ioContextPtr, std::mutex& A_mtx)
		: m_io_contextPtr(A_ioContextPtr), m_mtx(A_mtx)
	{
	}

	thread_worker::~thread_worker()
	{
	}

	void thread_worker::set_number_of_threads(size_t A_numberOfThreads) noexcept
	{
		m_number_of_threads = std::move(A_numberOfThreads);
	}

	std::shared_ptr<boost::thread_group> thread_worker::run_threads()
	{
		auto threadGroup = std::make_shared<boost::thread_group>();
		
		for (size_t index = 0; index < m_number_of_threads; index++)
		{
			threadGroup->create_thread([=]
				{
					auto threadWorker = std::make_shared<clientServerApp::thread_worker>(m_io_contextPtr, m_mtx);
					threadWorker->run_io_context();
				});
		}

		return threadGroup;
	}

	void thread_worker::run_io_context()
	{
		m_mtx.lock();
		std::cout << "Thread: " << std::this_thread::get_id() << " Start.\n";
		m_mtx.unlock();

		while (true)
		{
			try
			{
				const auto& tasks = m_io_contextPtr->run();

				if (tasks == 0)
					break;
			}
			catch (const std::exception& ex)
			{
				m_mtx.lock();
				std::cout << "Message: " << ex.what() << "\n";
				m_mtx.unlock();
			}
		}

		m_mtx.lock();
		std::cout << "Thread: " << std::this_thread::get_id() << " End.\n";
		m_mtx.unlock();
	}
}