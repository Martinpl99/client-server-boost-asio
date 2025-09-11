#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

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

    for (size_t index = 0; index < 2; index++)
        threads.create_thread([=] {WorkerThread(io_ctx, index); });

    boost::asio::ip::tcp::socket sckt(*io_ctx);

    try
    {
        boost::asio::ip::tcp::resolver resolver(*io_ctx);

        auto iterator = resolver.resolve("www.packtpub.com", "80");
        const auto& endpoint = *iterator.begin();
        
        mtx.lock();
        std::cout << "Connecting to: " << endpoint.endpoint() << "\n";
        mtx.unlock();

        sckt.connect(endpoint);
        std::cout << "Connected!\n";
    }
    catch (const std::exception&)
    {

    }

    std::cin.get();
    error_code ec;
    sckt.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    sckt.close();

    io_ctx->stop();
    threads.join_all();

    return 0;
}


