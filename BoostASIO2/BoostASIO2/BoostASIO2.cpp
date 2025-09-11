
#include <iostream>
#include<boost/thread.hpp>
#include<boost/chrono.hpp>
#include<boost/asio.hpp>
#include<functional>
//#include"Math/Headers/OwnMath.h"

std::mutex mtx;
using namespace boost::asio;
using namespace boost::system;
using namespace std::chrono;

template<typename Func, typename... Args>
decltype(auto) PerformOperation(Func&& A_funcObj, Args&&... args)
{
    return std::forward<Func>(A_funcObj)(std::forward<Args>(args)...);
}

void WriteMessage(const std::string& A_message)
{
    std::lock_guard<std::mutex> guardLock(mtx);
    std::cout << "Zadanie wywolane z watku: " << std::this_thread::get_id() << std::endl;
    std::cout << A_message << std::endl;
}

void PerformTasks(io_context& A_ioContx)
{
    //post(A_ioContx, std::bind(WriteMessage, "Dupa"));
    //post(A_ioContx, std::bind(WriteMessage, "Dupa"));
    //post(A_ioContx, std::bind(WriteMessage, "Dupa"));
    //post(A_ioContx, std::bind(WriteMessage, "Dupa"));
    //post(A_ioContx, std::bind(WriteMessage, "Dupa"));
    //post(A_ioContx, std::bind(WriteMessage, "Dupa"));
    //post(A_ioContx, std::bind(WriteMessage, "Dupa"));
    //post(A_ioContx, std::bind(WriteMessage, "Dupa"));
    //post(A_ioContx, std::bind(WriteMessage, "Dupa"));
    //post(A_ioContx, std::bind(WriteMessage, "Dupa"));
    //post(A_ioContx, std::bind(WriteMessage, "Dupa"));
}

int main()
{
    io_context io_contx;

    auto work_guard = std::make_shared<
        executor_work_guard<io_context::executor_type>>(make_work_guard(io_contx));

    boost::thread_group threads;

    int a = 0;
    PerformTasks(io_contx);

    for (size_t idx = 0; idx < 5; idx++)
    {
        threads.create_thread([&]() {
            io_contx.run();
            });
    }
    work_guard->reset();
    threads.join_all();
    std::cout << "HELO WORD" << std::endl;

    return 0;
}
