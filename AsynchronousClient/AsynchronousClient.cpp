// AsynchronousClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "thread_worker.h"

using namespace boost::asio;
using namespace boost;
using namespace boost::system;

std::mutex main_mutex;

void WorkerThread(std::shared_ptr<io_context>
    iosvc, int counter)
{
    main_mutex.lock();
    std::cout << "Thread: " << counter << " Start.\n";
    main_mutex.unlock();

    while (true)
    {
        io_context::count_type tasks;
        try
        {
            const auto& tasks = iosvc->run();
        }
        catch (const std::exception& ex)
        {
            main_mutex.lock();
            std::cout << "Message: " << ex.what() << "\n";
            main_mutex.unlock();
        }

        break;
    }

    main_mutex.lock();
    std::cout << "Thread: " << counter << " End.\n";
    main_mutex.unlock();
}

void OnConnect(const error_code& ec) {
    if (ec) {
        main_mutex.lock();
        std::cout << "OnConnect Error: " << ec << ".\n";
        main_mutex.unlock();
    }
    else {
        main_mutex.lock();
        std::cout << "Connected!.\n";
        main_mutex.unlock();
    }
}

int main()
{
    auto io_ctx = std::make_shared<io_context>();
    auto io_ctx_executor = io_ctx->get_executor();
    auto worker = make_work_guard(io_ctx_executor);
    auto strandQueue = make_strand(*io_ctx);

    auto threadWorker = std::make_shared<clientServerApp::thread_worker>(io_ctx, main_mutex);
    threadWorker->set_number_of_threads(4);
    auto runningThreads = threadWorker->run_threads();

    auto socket = std::make_shared< boost::asio::ip::tcp::socket>(*io_ctx);

    try
    {
        boost::asio::ip::tcp::resolver resolver(*io_ctx);

        auto iterator = resolver.resolve("www.packtpub.com", "80");
        const auto& endpoint = *iterator.begin();

        main_mutex.lock();
        std::cout << "Connecting to: " << endpoint.endpoint() << "\n";
        main_mutex.unlock();

        socket->async_connect(endpoint, [](const error_code& ec) { OnConnect(ec); });
    }
    catch (const std::exception&)
    {

    }

    std::cin.get();
    error_code ec;
    socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket->close();

    io_ctx->stop();
    runningThreads->join_all();

    return 0;
}

