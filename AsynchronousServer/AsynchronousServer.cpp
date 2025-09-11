// AsynchronousServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "headers/server/server.h"
#include "headers/thread_worker.h"
std::mutex mtx;
using namespace boost::asio;
using namespace boost;
using namespace boost::system;

void WorkerThread(std::shared_ptr<io_context>
    iosvc, int counter)
{
    mtx.lock();
    std::cout << "Thread: " << counter << " Start.\n";
    mtx.unlock();

    while (true)
    {
        io_context::count_type tasks;
        try
        {
            const auto& tasks = iosvc->run();
        }
        catch (const std::exception& ex)
        {
            mtx.lock();
            std::cout << "Message: " << ex.what() << "\n";
            mtx.unlock();
        }

        break;
    }

    mtx.lock();
    std::cout << "Thread: " << counter << " End.\n";
    mtx.unlock();
}

void OnAccept(const error_code& ec,
    boost::asio::ip::tcp::socket& A_socket,
    std::shared_ptr<boost::asio::ip::tcp::acceptor> A_acceptor)
{
    if (ec) {
        mtx.lock();
        std::cout << "OnAccept Error: " << ec.what() << ".\n";
        mtx.unlock();
    }
    else 
    {
        mtx.lock();
        std::cout << "Accepted!" << ".\n";
        mtx.unlock();

        A_acceptor->async_accept([=](const error_code& ec, boost::asio::ip::tcp::socket A_socket) 
            {
                OnAccept(ec, A_socket, A_acceptor);
            });
    }
}

std::string address("127.0.0.1");
constexpr auto portNumber = 4444;

int main()
{
    auto io_ctx = std::make_shared<io_context>();
    auto io_ctx_executor = io_ctx->get_executor();
    auto worker = make_work_guard(io_ctx_executor);

    auto threadWorker = std::make_shared<clientServerApp::thread_worker>(io_ctx, mtx);
    threadWorker->set_number_of_threads(4);
    auto runningThreads = threadWorker->run_threads();

    auto server = std::make_shared<clientServerApp::server>(*io_ctx, address, static_cast<short>(portNumber), mtx);
    server->do_accept();

    std::cin.get();

    runningThreads->join_all();

    return 0;
}