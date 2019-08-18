/**
 * \file
 * \brief Thread pool for async calculations
 * \author Maksimovskiy A.S.
 */


#include <thread_pool.hpp>


namespace pool
{

ThreadPool::ThreadPool(size_t threads)
    : mStop(false)
{
    for (size_t idx = 0; idx < threads; ++idx)
        mWorkers.emplace_back(
            [this]
            {
                for (;;)
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->mMutex);
                        this->mCondition.wait(lock,
                                              [this]{ return this->mStop || !this->mTasks.empty(); });

                        if (this->mStop && this->mTasks.empty())
                            return;

                        task = std::move(this->mTasks.front());
                        this->mTasks.pop();
                    }

                    task();
                }
            }
        );
}

// Destructor joins all threads
ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mStop = true;
    }

    mCondition.notify_all();

    for (auto& worker: mWorkers)
        worker.join();
}

}
