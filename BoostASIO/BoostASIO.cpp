
#include <iostream>
#include<boost/thread.hpp>
#include<boost/chrono.hpp>
#include<boost/asio.hpp>
#include<functional>
#include"Math/Headers/OwnMath.h"

std::mutex global_stream_lock;
using namespace boost::asio;
using namespace boost::system;
using namespace std::chrono;

template<typename Func, typename... Args>
decltype(auto) PerformOperation(Func&& A_funcObj, Args&&... args)
{
    return std::forward<Func>(A_funcObj)(std::forward<Args>(args)...);
}

void WriteMessageDispatch(int x)
{
    std::cout << "Zadanie Dispatch wywolane z watku: " << std::this_thread::get_id() << std::endl;
    std::cout << x << std::endl;
}

void WriteMessage(const std::string& A_message, io_context& A_caller)
{
    auto strandek = make_strand(A_caller);
    post(strandek, std::bind(WriteMessageDispatch,1));
    post(strandek, [] { WriteMessageDispatch(2); });
}

void WriteMessage2(const std::string& A_message, io_context& A_caller)
{
    auto strandek = make_strand(A_caller);
    post(strandek, [] { WriteMessageDispatch(3); });
    post(strandek, [] { WriteMessageDispatch(4); });
}


void WorkerThread(std::shared_ptr<io_context>
    iosvc, int counter) 
{
    global_stream_lock.lock();
    std::cout << "Thread " << counter << " Start.\n";
    global_stream_lock.unlock();

    while (true)
    {
        try {
            auto result = iosvc->run();

            global_stream_lock.lock();
            std::cout << "Thread " << counter << " End.\n";
            global_stream_lock.unlock();

            if (result == 0)
                break;
        }
        catch (std::exception& ex) {
            global_stream_lock.lock();
            std::cout << "Exception from thread:  " << counter << " Message: " << ex.what() << ".\n";
            global_stream_lock.unlock();
        }
    }
}

void ThrowAnException(std::shared_ptr<io_context>
    iosvc, int counter) {
    global_stream_lock.lock();
    std::cout << "Throw Exception " << counter << "\n";
    global_stream_lock.unlock();
    throw(std::runtime_error("The Exception !!!"));
}


void TimerHandler(const error_code& ec, std::shared_ptr<steady_timer> A_steadyTimer) {
    if (ec) {
        global_stream_lock.lock();
        std::cout << "Error Message: " << ec << ".\n";
        global_stream_lock.unlock();
    }
    else {
        global_stream_lock.lock();
        std::cout << "You see this every three seconds.\n";
        global_stream_lock.unlock();

        A_steadyTimer->expires_after(seconds(3));
        A_steadyTimer->async_wait(
            [A_steadyTimer](const error_code& ec)
            {
                TimerHandler(ec, A_steadyTimer);
            }
        );
    }
}

void Print(int number) 
{
    std::cout << "Number: " << number << std::endl;
    boost::this_thread::sleep(
    boost::posix_time::milliseconds(500));
}


int main()
{
    //io_context io_contx;

    //auto work_guard = std::make_shared<
    //    executor_work_guard<io_context::executor_type>>(make_work_guard(io_contx));

    //boost::thread_group threads;

    //int a = 0;
    //BoostASIO::OwnMath math;

    //auto strandek = make_strand(io_contx);

    //post(strandek, std::bind(WriteMessage, "Dupa", std::ref(io_contx)));
    //post(strandek, std::bind(WriteMessage2, "Dupa", std::ref(io_contx)));

    //for (size_t idx = 0; idx < 5; idx++)
    //{
    //    threads.create_thread([&]() {
    //        io_contx.run();
    //        });
    //}

    //work_guard->reset();
    //threads.join_all();

    //std::cout << "HELO WORD" << std::endl;

    //return 0;

    auto io_svc = std::make_shared<io_context>();
    auto worker = std::make_shared<executor_work_guard<io_context::executor_type>>(make_work_guard(*io_svc));

    global_stream_lock.lock();
    std::cout << "The program will exit once all work has finished.\n";
    global_stream_lock.unlock();

    boost::thread_group threads;
    for (int i = 1; i <= 5; i++)
        threads.create_thread(boost::bind(&WorkerThread, io_svc, i));

    auto strandek = make_strand(*io_svc);

    post(strandek, boost::bind(&Print, 1));
    post(strandek, boost::bind(&Print, 2));
    post(strandek, boost::bind(&Print, 3));
    post(strandek, boost::bind(&Print, 4));
    post(strandek, boost::bind(&Print, 5));
    post(strandek, boost::bind(&Print, 6));
    post(strandek, boost::bind(&Print, 7));
    post(strandek, boost::bind(&Print, 8));
    post(strandek, boost::bind(&Print, 9));
    post(strandek, boost::bind(&Print, 10));
     
    //post(*io_svc, boost::bind(&ThrowAnException, io_svc, 1));
    //post(*io_svc, boost::bind(&ThrowAnException, io_svc, 2));
    //post(*io_svc, boost::bind(&ThrowAnException, io_svc, 3));
    //post(*io_svc, boost::bind(&ThrowAnException, io_svc, 4));
    //post(*io_svc, boost::bind(&ThrowAnException, io_svc, 5));

    auto timer = std::make_shared<steady_timer>(*io_svc);
    timer->expires_after(seconds(3));
    timer->async_wait([timer](const error_code& ec)
       {
            TimerHandler(ec, timer);
       });

    std::cin.get();
    io_svc->stop();

    threads.join_all();
    return 0;
}
