// AsynchronousServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include"Headers/Server/Server.h"

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

int main()
{
    auto io_ctx = std::make_shared<io_context>();
    auto io_ctx_executor = io_ctx->get_executor();
    auto worker = make_work_guard(io_ctx_executor);
    auto strandQueue = make_strand(*io_ctx);

    mtx.lock();
    std::cout << "Press ENTER to EXIT!\n";
    mtx.unlock();

    thread_group threads;

    for (size_t index = 0; index < 1; index++)
        threads.create_thread([=] {WorkerThread(io_ctx, index); });

    //auto socket = std::make_shared<boost::asio::ip::tcp::socket>(*io_ctx);
    //auto acceptor = std::make_shared<ip::tcp::acceptor>(*io_ctx);

    boost::asio::ip::tcp::resolver resolver(*io_ctx);
    auto iterator = resolver.resolve("127.0.0.1", "4444");
    auto& endpoint = *iterator.begin();

    auto server = std::make_shared<clientServerApp::server>(*io_ctx, endpoint);

    mtx.lock();
    std::cout << "Listening on: " << endpoint.endpoint() << "\n";
    mtx.unlock();

    //io_ctx->run();
    //try
    //{
    //    acceptor->open(ip::tcp::v4());
    //    acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(false));
    //    acceptor->bind(endpoint.endpoint());
    //    acceptor->listen(boost::asio::socket_base::max_listen_connections);
    //    acceptor->async_accept(*socket, [=](error_code ec)
    //        {
    //            OnAccept(ec, *socket, acceptor);
    //        });

    //    std::cout << "Listening on: " << endpoint.endpoint() << "\n";
    //    mtx.unlock();
    //}
    //catch (const std::exception&)
    //{

    //}

    //clientServerApp::server(*io_ctx, endpoint.endpoint());
    std::cin.get();

    //error_code ec;
    //socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    //socket->close();

    threads.join_all();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
