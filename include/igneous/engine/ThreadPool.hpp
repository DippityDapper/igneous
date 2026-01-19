#pragma once

#include <functional>
#include <future>
#include <queue>
#include <thread>
#include <vector>

namespace Engine
{
    class ThreadPool
    {
      public:
        explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
        ~ThreadPool();

        template<class F, class... Args>
        std::future<std::invoke_result_t<F, Args...>> Enqueue(F&& f, Args&&... args);

      private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;

        std::mutex queueMutex;
        std::condition_variable condition;
        std::atomic<bool> stop;
    };
}

namespace Engine
{
    inline ThreadPool::ThreadPool(size_t numThreads)
        : stop(false)
    {
        for (size_t i = 0; i < numThreads; ++i)
        {
            workers.emplace_back([this]
                                 {
                while (true)
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty())
                            return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    task();
                } });
        }
    }

    inline ThreadPool::~ThreadPool()
    {
        stop = true;
        condition.notify_all();
        for (auto& worker: workers)
            worker.join();
    }

    template<class F, class... Args>
    std::future<std::invoke_result_t<F, Args...>> ThreadPool::Enqueue(F&& f, Args&&... args)
    {
        using return_type = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.emplace([task]()
                          { (*task)(); });
        }
        condition.notify_one();
        return res;
    }
}