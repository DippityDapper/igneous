// Doc: docs/classes/Event.md
#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace Engine
{

    /// Callback signal. Only `void` return types are supported; capture results in the callback if needed.
    template<typename Ret, typename... Args>
    class Event
    {
        static_assert(std::is_void_v<Ret>, "Event only supports void return type.");
      public:

        struct Connection
        {

            size_t id{};

            bool operator==(const Connection& other) const
            {
                return id == other.id;
            }
        };

      private:

        struct CallbackEntry
        {
            size_t id;                        ///< Unique callback identifier
            std::function<Ret(Args...)> func; ///< Stored callback function
        };

        std::vector<CallbackEntry> callbacks;

        size_t nextId = 1;

      public:

        Connection Connect(const std::function<Ret(Args...)>& callback)
        {
            callbacks.push_back({nextId, callback});
            return {nextId++};
        }

        template<typename Callable>
        Connection Connect(Callable&& func)
        {
            callbacks.push_back({nextId, std::forward<Callable>(func)});
            return {nextId++};
        }

        template<typename T>
        Connection Connect(T* instance, Ret (T::*method)(Args...))
        {
            callbacks.push_back({nextId,
                                 [=](Args... args)
                                 { (instance->*method)(args...); }});
            return {nextId++};
        }

        template<typename T>
        Connection Connect(T* instance, Ret (T::*method)(Args...) const)
        {
            callbacks.push_back({nextId,
                                 [=](Args... args)
                                 { (instance->*method)(args...); }});
            return {nextId++};
        }

        template<typename T>
        Connection Connect(std::weak_ptr<T> instance, Ret (T::*method)(Args...))
        {
            callbacks.push_back({nextId,
                                 [=](Args... args)
                                 {
                                     if (auto obj = instance.lock())
                                         (obj.get()->*method)(args...);
                                 }});
            return {nextId++};
        }

        template<typename T>
        Connection Connect(std::weak_ptr<T> instance, Ret (T::*method)(Args...) const)
        {
            callbacks.push_back({nextId,
                                 [=](Args... args)
                                 {
                                     if (auto obj = instance.lock())
                                         (obj.get()->*method)(args...);
                                 }});
            return {nextId++};
        }

        void Disconnect(Connection connection)
        {
            callbacks.erase(
                    std::remove_if(callbacks.begin(), callbacks.end(), [&](const CallbackEntry& c)
                                   { return c.id == connection.id; }),
                    callbacks.end());
        }

        void DisconnectAll()
        {
            callbacks.clear();
        }

        void Emit(Args... args)
        {
            for (auto& entry: callbacks)
                entry.func(args...);
        }

        void operator()(Args... args)
        {
            Emit(args...);
        }
    };
}
