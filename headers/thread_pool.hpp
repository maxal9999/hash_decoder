/**
 * \file
 * \brief Thread pool for async calculations
 * \author Maksimovskiy A.S.
 */


#pragma once

#include <condition_variable>
#include <future>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace pool
{

class ThreadPool
{
public:
    // Constructor
    ThreadPool(size_t);

    // Destructor
    ~ThreadPool();

    // Add func for async calculation
    template<typename Func, typename... Args>
    auto AddFunc(Func&& f, Args&&... args) -> std::future<decltype(f(args...))>;

private:
    // Workers list
    std::vector<std::thread> mWorkers;

    // Task queue
    std::queue<std::function<void()>> mTasks;

    // Synchronization
    std::mutex mMutex;
    std::condition_variable mCondition;
    bool mStop;
};

template<typename Func, typename... Args>
auto ThreadPool::AddFunc(Func&& f, Args&&... args) -> std::future<decltype(f(args...))>
{
    using return_type = decltype(f(args...));
    using package = std::packaged_task<return_type()>;

    auto task = std::make_shared<package>(
        std::bind(std::forward<Func>(f), std::forward<Args>(args)...)
    );

    auto res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(mMutex);

        if (mStop)
            throw std::runtime_error("Add func when stopped ThreadPool");

        mTasks.emplace([task](){ (*task)(); });
    }
    mCondition.notify_one();
    return res;
}

}
