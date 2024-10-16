/**
 * hl/silva/collections/threads/basic_pool_async.hpp
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * Made by: Mattis DALLEAU
 */

#pragma once

#include <hl/silva/collections/threads/basic_pool.hpp>
#include <atomic>

namespace hl
{
namespace silva
{
namespace collections
{
namespace threads
{

class basic_pool_async
{
private:
    atomic_bool m_stop;
    atomic_bool m_has_event;
    condition_variable m_condition;
    thread_safe::mutex m_mutex_loop;
    thread_safe::mutex m_mutex_flow;

    thread m_async_thread_pool_update_thread;
    basic_pool m_pool;

public:
    void start(bool lock=true)
    {
        if (lock)
        {
            m_mutex_flow.lock();
        }

        if (!m_stop)
        {
            if (lock)
            {
                m_mutex_flow.unlock();
            }
            return;
        }

        m_stop = false;

        m_async_thread_pool_update_thread = thread([this]() {
            do {
                m_pool.update();
                std::unique_lock<std::mutex> lock_loop(m_mutex_loop);
                m_condition.wait(lock_loop, [this] { return m_stop || m_has_event; });
                m_has_event = false;
            } while (!m_stop);
        });

        if (lock)
        {
            m_mutex_flow.unlock();
        }
    }

    void stop(bool lock=true)
    {
        if (lock)
        {
            m_mutex_flow.lock();
        }

        if (m_stop)
        {
            if (lock)
            {
                m_mutex_flow.unlock();
            }
            return;
        }

        m_stop = true;
        m_condition.notify_one();
        m_async_thread_pool_update_thread.join();

        m_pool.update();
        m_pool.join();

        if (lock)
        {
            m_mutex_flow.unlock();
        }
    }

    basic_pool_async(bool start = true)
        : m_stop(true)
        , m_has_event(false)
        , m_condition()
        , m_mutex_loop()
        , m_mutex_flow()
        , m_async_thread_pool_update_thread()
        , m_pool(m_condition)
    {
        if (start)
        {
            this->start();
        }
    }

    template <typename F, typename... Args>
    void push(F &&f, Args &&...args)
    {
        thread_safe::lock_guard lock(m_mutex_flow);

        if (m_stop)
        {
            return; // Ignore silently (Here just in case of Race condition)
        }

        m_pool.push(
            [this](F &&fthread, Args &&...args_thread)
            {
                fthread(std::forward<Args>(args_thread)...);
                m_has_event = true;
                m_condition.notify_one();
            }, std::forward<F>(f), std::forward<Args>(args)...);
    }

    void join()
    {
        thread_safe::lock_guard lock(m_mutex_flow);

        if (m_stop)
        {
            return;
        }

        stop(false);
        start(false);
    }

    bool empty() const
    {
        return m_pool.empty();
    }

    ~basic_pool_async()
    {
        stop();
    }

    template <bool IncludeDoneNotRemoved = false>
    size_t pending_threads()
    {
        return m_pool.pending_threads<IncludeDoneNotRemoved>();
    }
};

}
}
}
}
