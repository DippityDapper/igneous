#pragma once

#include <functional>
#include <vector>
#include <algorithm>
#include <utility>
#include <memory>

namespace Engine
{
    /**
     * @class Event
     * @brief A lightweight, type-safe event/signal system.
     *
     * Event implements a publish–subscribe mechanism where multiple callbacks
     * can be connected and later invoked (emitted) with a specific signature.
     * It supports free functions, lambdas, member functions, and weakly-owned
     * object callbacks.
     *
     * Callbacks are identified via Connection objects, which can be used to
     * disconnect individual listeners.
     *
     * @tparam Ret  Return type of the callbacks.
     * @tparam Args Argument types passed to the callbacks.
     *
     * @note The return values of callbacks are ignored when emitting.
     * @note Callbacks are executed in the order they were connected.
     */
    template<typename Ret, typename... Args>
    class Event
    {
      public:
        /**
         * @struct Connection
         * @brief Handle representing a connected callback.
         *
         * A Connection is returned when a callback is registered with the event.
         * It can later be used to disconnect that specific callback.
         */
        struct Connection
        {
            /**
             * @brief Unique identifier of the connection.
             */
            size_t id{};

            /**
             * @brief Equality comparison.
             *
             * @param other Connection to compare against.
             * @return true if both connections refer to the same callback.
             */
            bool operator==(const Connection& other) const
            {
                return id == other.id;
            }
        };

      private:
        /**
         * @struct CallbackEntry
         * @brief Internal representation of a connected callback.
         */
        struct CallbackEntry
        {
            size_t id;                        ///< Unique callback identifier
            std::function<Ret(Args...)> func; ///< Stored callback function
        };

        /**
         * @brief List of registered callbacks.
         */
        std::vector<CallbackEntry> callbacks;

        /**
         * @brief ID assigned to the next connection.
         */
        size_t nextId = 1;

      public:
        /**
         * @brief Connects a std::function callback.
         *
         * @param callback Function to register.
         * @return A Connection handle that can be used to disconnect the callback.
         */
        Connection Connect(const std::function<Ret(Args...)>& callback)
        {
            callbacks.push_back({nextId, callback});
            return {nextId++};
        }

        /**
         * @brief Connects a generic callable (lambda, functor, etc.).
         *
         * @tparam Callable Callable type.
         * @param func Callable to register.
         * @return A Connection handle that can be used to disconnect the callback.
         */
        template<typename Callable>
        Connection Connect(Callable&& func)
        {
            callbacks.push_back({nextId, std::forward<Callable>(func)});
            return {nextId++};
        }

        /**
         * @brief Connects a non-const member function.
         *
         * @tparam T Class type.
         * @param instance Pointer to the object instance.
         * @param method Member function pointer.
         * @return A Connection handle that can be used to disconnect the callback.
         *
         * @note The caller is responsible for ensuring the instance remains valid.
         */
        template<typename T>
        Connection Connect(T* instance, Ret (T::*method)(Args...))
        {
            callbacks.push_back({nextId,
                                 [=](Args... args)
                                 { (instance->*method)(args...); }});
            return {nextId++};
        }

        /**
         * @brief Connects a const member function.
         *
         * @tparam T Class type.
         * @param instance Pointer to the object instance.
         * @param method Const member function pointer.
         * @return A Connection handle that can be used to disconnect the callback.
         *
         * @note The caller is responsible for ensuring the instance remains valid.
         */
        template<typename T>
        Connection Connect(T* instance, Ret (T::*method)(Args...) const)
        {
            callbacks.push_back({nextId,
                                 [=](Args... args)
                                 { (instance->*method)(args...); }});
            return {nextId++};
        }

        /**
         * @brief Connects a member function using a weak pointer.
         *
         * The callback is only invoked if the weak pointer can be locked at
         * emission time, preventing calls on destroyed objects.
         *
         * @tparam T Class type.
         * @param instance Weak pointer to the object.
         * @param method Member function pointer.
         * @return A Connection handle that can be used to disconnect the callback.
         */
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

        /**
         * @brief Connects a const member function using a weak pointer.
         *
         * @tparam T Class type.
         * @param instance Weak pointer to the object.
         * @param method Const member function pointer.
         * @return A Connection handle that can be used to disconnect the callback.
         */
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

        /**
         * @brief Disconnects a previously connected callback.
         *
         * @param connection Connection handle obtained from Connect().
         *
         * @note If the connection does not exist, this is a no-op.
         */
        void Disconnect(Connection connection)
        {
            callbacks.erase(
                    std::remove_if(callbacks.begin(), callbacks.end(), [&](const CallbackEntry& c)
                                   { return c.id == connection.id; }),
                    callbacks.end());
        }

        /**
         * @brief Disconnects all registered callbacks.
         */
        void DisconnectAll()
        {
            callbacks.clear();
        }

        /**
         * @brief Emits the event, invoking all connected callbacks.
         *
         * @param args Arguments forwarded to each callback.
         *
         * @note Callbacks are executed in the order they were connected.
         */
        void Emit(Args... args)
        {
            for (auto& entry: callbacks)
                entry.func(args...);
        }

        /**
         * @brief Convenience call operator for emitting the event.
         *
         * Equivalent to calling Emit().
         *
         * @param args Arguments forwarded to each callback.
         */
        void operator()(Args... args)
        {
            Emit(args...);
        }
    };
}
