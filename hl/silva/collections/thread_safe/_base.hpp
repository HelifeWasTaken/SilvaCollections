/**
 * hl/silva/collections/thread_safe/_base.hpp
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

#include <mutex>
#include <memory>
#include <functional>

#include <hl/silva/collections/meta.hpp>

// Extend std
namespace std
{
    template<typename ...LockArgs>
    void unlock(LockArgs&&... args)
    {
        (std::forward<LockArgs>(args).unlock(), ...);
    }
}

namespace hl
{
namespace silva
{
namespace collections
{
namespace thread_safe
{

template<class T>
using default_allocator = std::allocator<T>;

// Requires shared mutex for multiple readers/iterators
using mutex = std::mutex;

using thread_safe_result_lock = std::unique_lock<mutex>;

using lock_guard = std::lock_guard<mutex>;

struct adopt_lock_t
{
    explicit adopt_lock_t() = default;
};

inline constexpr adopt_lock_t adopt_lock;

class thread_safe_result_iter_lock
{
private:
    std::shared_ptr<thread_safe_result_lock> m_lock;

public:
    explicit thread_safe_result_iter_lock(mutex& pmutex)
        : m_lock(std::make_shared<thread_safe_result_lock>(pmutex))
    {
    }

    explicit thread_safe_result_iter_lock(thread_safe_result_iter_lock& other, adopt_lock_t)
        : m_lock(other.m_lock)
    {}


    thread_safe_result_iter_lock& operator=(thread_safe_result_iter_lock& other) = delete;

    thread_safe_result_iter_lock(thread_safe_result_iter_lock&& other) = default;

    thread_safe_result_iter_lock& operator=(thread_safe_result_iter_lock&& other) = default;

    ~thread_safe_result_iter_lock() = default;
};

template<class T>
struct thread_safe_result
{
    thread_safe_result_lock lock;
    std::reference_wrapper<T> value;

    thread_safe_result(thread_safe_result_lock &&plock, T& pvalue)
        : lock(std::move(plock)), value(pvalue)
    {
    }

    ~thread_safe_result() = default;

    thread_safe_result(const thread_safe_result& other) = default;
    thread_safe_result& operator=(const thread_safe_result& other) = default;
    thread_safe_result(thread_safe_result&& other) = default;
    thread_safe_result& operator=(thread_safe_result&& other) = default;

    inline T& operator*()
    {
        return value.get();
    }

    inline const T& operator*() const
    {
        return value.get();
    }
};

template<class T>
using thread_safe_result_ptr = thread_safe_result<T*>;

// Using an iterator is not considered completely thread_safe and may cause a deadlock or be quite
// unsafe or slow if not used properly.
// semi_thread_safe is a just made up term to signify that the iterator is thread_safe only
// in some cases and not all.
// The iterator will automatically lock and adopt the lock of the mutex
// Moving the iterator will result in copying the internal iterator and moving the lock
template<class T, typename IteratorType>
class semi_thread_safe_unsafe_iterator_
{
public:
    using base_iterator_t = IteratorType;

private:
    base_iterator_t m_iterator;
    thread_safe_result_iter_lock m_lock;

public:
    semi_thread_safe_unsafe_iterator_(base_iterator_t iterator, thread_safe_result_iter_lock&& lock)
        : m_iterator(iterator), m_lock(std::move(lock))
    {}

    semi_thread_safe_unsafe_iterator_(base_iterator_t iterator, thread_safe_result_iter_lock& lock)
        : m_iterator(iterator), m_lock(thread_safe_result_iter_lock(lock, adopt_lock))
    {}

    ~semi_thread_safe_unsafe_iterator_() = default;

    semi_thread_safe_unsafe_iterator_(const semi_thread_safe_unsafe_iterator_& other)
        : m_iterator(other.m_iterator), m_lock(thread_safe_result_iter_lock(lock, adopt_lock))
    {}

    semi_thread_safe_unsafe_iterator_(semi_thread_safe_unsafe_iterator_&& other)
        : m_iterator(other.m_iterator), m_lock(std::move(other.m_lock))
    {}

    semi_thread_safe_unsafe_iterator_& operator=(const semi_thread_safe_unsafe_iterator_& other) = delete;

    semi_thread_safe_unsafe_iterator_& operator=(semi_thread_safe_unsafe_iterator_&& other)
    {
        m_iterator = other.m_iterator;
        m_lock = std::move(other.m_lock);
        return *this;
    }

    T& operator*()
    {
        return *m_iterator;
    }

    // disallow direct access to the pointer so that the mutex is always locked
    T* operator->() = delete; 

    semi_thread_safe_unsafe_iterator_& operator++()
    {
        ++m_iterator;
        return *this;
    }

    semi_thread_safe_unsafe_iterator_ operator++(int)
    {
        semi_thread_safe_unsafe_iterator_ temp = *this;
        ++m_iterator;
        return temp;
    }

    semi_thread_safe_unsafe_iterator_& operator--()
    {
        --m_iterator;
        return *this;
    }

    semi_thread_safe_unsafe_iterator_ operator--(int)
    {
        semi_thread_safe_unsafe_iterator_ temp = *this;
        --m_iterator;
        return temp;
    }

    bool operator==(const semi_thread_safe_unsafe_iterator_& other) const
    {
        return m_iterator == other.m_iterator;
    }

    bool operator!=(const semi_thread_safe_unsafe_iterator_& other) const
    {
        return !(*this == other);
    }

    bool operator<(const semi_thread_safe_unsafe_iterator_& other) const
    {
        return m_iterator < other.m_iterator;
    }

    bool operator>(const semi_thread_safe_unsafe_iterator_& other) const
    {
        return m_iterator > other.m_iterator;
    }

    bool operator<=(const semi_thread_safe_unsafe_iterator_& other) const
    {
        return m_iterator <= other.m_iterator;
    }

    bool operator>=(const semi_thread_safe_unsafe_iterator_& other) const
    {
        return m_iterator >= other.m_iterator;
    }

    bool operator==(const base_iterator_t& other) const
    {
        return m_iterator == other;
    }

    bool operator!=(const base_iterator_t& other) const
    {
        return !(*this == other);
    }

    bool operator<(const base_iterator_t& other) const
    {
        return m_iterator < other;
    }

    bool operator>(const base_iterator_t& other) const
    {
        return m_iterator > other;
    }

    bool operator<=(const base_iterator_t& other) const
    {
        return m_iterator <= other;
    }

    bool operator>=(const base_iterator_t& other) const
    {
        return m_iterator >= other;
    }

    semi_thread_safe_unsafe_iterator_& operator=(const base_iterator_t& other)
    {
        m_iterator = other;
        return *this;
    }

    base_iterator_t non_thread_safe_iterator_unsafe() const
    {
        return m_iterator;
    }

    thread_safe_result_iter_lock& lock()
    {
        return m_lock;
    }
};

template<class T>
class thread_safe_iterable
{
private:
    mutable thread_safe_result_iter_lock m_lock;
    typename T::base_type_t& m_base_ref;

public:
    using iterator = typename T::iterator;
    using const_iterator = typename T::const_iterator;
    using reverse_iterator = typename T::reverse_iterator;
    using const_reverse_iterator = typename T::const_reverse_iterator;

    explicit thread_safe_iterable(mutex& pmutex, typename T::base_type_t& ref)
        : m_lock(thread_safe_result_iter_lock(pmutex)), m_base_ref(ref)
    {}

    thread_safe_iterable(thread_safe_iterable& other)
        : m_lock(other.m_lock), m_base_ref(other.m_base_ref)
    {}

    thread_safe_iterable& operator=(thread_safe_iterable& other) = delete;

    thread_safe_iterable(thread_safe_iterable&& other)
        : m_lock(std::move(other.m_lock)), m_base_ref(other.m_base_ref)
    {}

    thread_safe_iterable& operator=(thread_safe_iterable&& other)
    {
        m_lock = std::move(other);
        m_base_ref = other.m_base_ref;
        return *this;
    }

    iterator begin()
    {
        return iterator(m_base_ref.begin(), thread_safe_result_iter_lock(m_lock, adopt_lock));
    }

    iterator end()
    {
        return iterator(m_base_ref.end(), thread_safe_result_iter_lock(m_lock, adopt_lock));
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(m_base_ref.rbegin(), thread_safe_result_iter_lock(m_lock, adopt_lock));
    }

    reverse_iterator rend()
    {
        return reverse_iterator(m_base_ref.rend(), thread_safe_result_iter_lock(m_lock, adopt_lock));
    }

    const_iterator cbegin() const
    {
        return const_iterator(m_base_ref.cbegin(), thread_safe_result_iter_lock(m_lock, adopt_lock));
    }

    const_iterator cend() const
    {
        return const_iterator(m_base_ref.cend(), thread_safe_result_iter_lock(m_lock, adopt_lock));
    }

    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(m_base_ref.rbegin(), thread_safe_result_iter_lock(m_lock, adopt_lock));
    }

    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(m_base_ref.rend(), thread_safe_result_iter_lock(m_lock, adopt_lock));
    }

    const_iterator begin() const
    {
        return cbegin();
    }

    const_iterator end() const
    {
        return cend();
    }

    const_reverse_iterator rbegin() const
    {
        return crbegin();
    }

    const_reverse_iterator rend() const
    {
        return crend();
    }
};

}
}
}
}

