/**
 * hl/silva/collections/threads/basic_pool.hpp
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

#include <hl/silva/collections/threads/_base.hpp>

#include <thread>
#include <condition_variable>

#include <hl/silva/collections/thread_safe/queue.hpp>
#include <hl/silva/collections/thread_safe/list.hpp>

namespace hl
{
namespace silva
{
namespace collections
{
namespace threads
{

// Thread queue
class basic_pool
{
private:
    thread_safe::list<thread> m_threads;

    // all the current threads ids that are done
    thread_safe::queue<thread::id> m_ids;

    condition_variable& m_notifier;

public:
    /**
     * @brief Construct a new thread pool object
     * @param notifier the notifier to notify when a thread is done so that the main thread can call update
     */
    basic_pool(condition_variable &notifier)
        : m_threads()
        , m_ids()
        , m_notifier(notifier)
    {}

    /**
     * @brief Push a new thread to the pool
     * @tparam F the function type
     * @tparam Args the arguments type
     * @param f the function to push
     * @param args the arguments to push
     */
    template <typename F, typename... Args>
    void push(F &&f, Args &&...args)
    {
        m_threads.emplace_back([this](F &&fthread, Args &&...args_thread) { 
            fthread(std::forward<Args>(args_thread)...);
            m_ids.push(this_thread::get_id());
            m_notifier.notify_one();
        }, std::forward<F>(f), std::forward<Args>(args)...);
    }

    void join()
    {
        for (auto &it : m_threads.iter())
        {
            it.join();
        }
        m_ids.clear();
        m_threads.clear();
    }

    void update()
    {
        // iterate through the ids and remove the threads that are done
        thread::id id;

        while (m_ids.poll(id))
        {
            m_threads.remove_if([id](const thread &pthread) {
                if (pthread.get_id() == id) {
                    const_cast<thread *>(&pthread)->join(); // bruh wtf that should be illegal but it's the fastest i could find
                    return true;
                }
                return false;
            });
        }
    }

    bool empty() const
    {
        return m_threads.empty() && m_ids.empty();
    }

    ~basic_pool()
    {
        join();
    }

    template<bool IncludeDoneNotRemoved = false>
    size_t pending_threads()
    {
        return m_threads.size() + (IncludeDoneNotRemoved ? m_ids.size() : 0);
    }
};

}
}
}
}
