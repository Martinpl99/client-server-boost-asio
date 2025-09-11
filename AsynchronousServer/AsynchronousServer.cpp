// AsynchronousServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "headers/server/server.h"
#include "headers/thread_worker.h"
using namespace boost::asio;
using namespace boost;
using namespace boost::system;


std::string address("127.0.0.1");
constexpr auto portNumber = 4444;

int main()
{
    std::mutex main_mutex;

    auto io_ctx = std::make_shared<io_context>();
    auto io_ctx_executor = io_ctx->get_executor();
    auto worker = make_work_guard(io_ctx_executor);

    auto threadWorker = std::make_shared<clientServerApp::thread_worker>(io_ctx, main_mutex);
    threadWorker->set_number_of_threads(4);
    auto runningThreads = threadWorker->run_threads();

    clientServerApp::server server(*io_ctx, address, static_cast<short>(portNumber), main_mutex);
    server.do_accept();

    std::cin.get();
    io_ctx->stop();

    runningThreads->join_all();

    return 0;
}