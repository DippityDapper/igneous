#pragma once

#include <mutex>
#include <optional>
#include <queue>

namespace Engine
{

    template<typename T>
    class ThreadSafeQueue
    {
      public:
        void Push(const T& value)
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(value);
        }

        std::optional<T> Pop()
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (queue.empty())
                return std::nullopt;

            T value = queue.front();
            queue.pop();
            return value;
        }

        bool Empty() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.empty();
        }

      private:
        mutable std::mutex mutex;
        std::queue<T> queue;
    };
}

template<typename T>
using ThreadSafeQueue [[deprecated("Use Engine::ThreadSafeQueue")]] = Engine::ThreadSafeQueue<T>;
