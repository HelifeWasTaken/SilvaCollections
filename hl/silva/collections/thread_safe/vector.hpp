/**
 * hl/silva/collections/thread_safe/vector.hpp
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

#include <hl/silva/collections/thread_safe/_base.hpp>
#include <vector>

namespace hl
{
namespace silva
{
namespace collections
{
namespace thread_safe
{

template<class T, class Allocator = default_allocator<T>>
class vector : public meta::NonCopyMoveable
{
public:
    using base_type_t = std::vector<T, Allocator>;
    using size_type = typename base_type_t::size_type;

private:
    base_type_t m_vector;
    mutable mutex m_mutex;

public:
    using iterator = semi_thread_safe_unsafe_iterator_<T, typename base_type_t::iterator>;
    using const_iterator = semi_thread_safe_unsafe_iterator_<T, typename base_type_t::const_iterator>;
    using reverse_iterator = semi_thread_safe_unsafe_iterator_<T, typename base_type_t::reverse_iterator>;
    using const_reverse_iterator = semi_thread_safe_unsafe_iterator_<T, typename base_type_t::const_reverse_iterator>;

    // Member functions
    vector() = default;
    virtual ~vector() override = default;

    void assign(size_type count, const T& value)
    {
        lock_guard lock(m_mutex);
        m_vector.assign(count, value);
    }

    // Element Access

    thread_safe_result<T> at(size_type pos)
    {
        thread_safe_result_lock lock(m_mutex);
        return thread_safe_result<T>(std::move(lock), m_vector.at(pos));
    }

    thread_safe_result<const T> at(size_type pos) const
    {
        thread_safe_result_lock lock(m_mutex);
        return thread_safe_result<const T>(std::move(lock), m_vector.at(pos));
    }

    thread_safe_result<T> operator[](size_type pos)
    {
        thread_safe_result_lock lock(m_mutex);
        return thread_safe_result<T>(std::move(lock), m_vector[pos]);
    }

    thread_safe_result<const T> operator[](size_type pos) const
    {
        thread_safe_result_lock lock(m_mutex);
        return thread_safe_result<const T>(std::move(lock), m_vector[pos]);
    }

    thread_safe_result<T> front()
    {
        thread_safe_result_lock lock(m_mutex);
        return thread_safe_result<T>(std::move(lock), m_vector.front());
    }

    thread_safe_result<const T> front() const
    {
        thread_safe_result_lock lock(m_mutex);
        return thread_safe_result<const T>(std::move(lock), m_vector.front());
    }

    thread_safe_result<T> back()
    {
        thread_safe_result_lock lock(m_mutex);
        return thread_safe_result<T>(std::move(lock), m_vector.back());
    }

    thread_safe_result<const T> back() const
    {
        thread_safe_result_lock lock(m_mutex);
        return thread_safe_result<const T>(std::move(lock), m_vector.back());
    }

    thread_safe_result_ptr<T> data()
    {
        thread_safe_result_lock lock(m_mutex);
        return thread_safe_result<T>(std::move(lock), m_vector.data());
    }

    thread_safe_result_ptr<const T> data() const
    {
        thread_safe_result_lock lock(m_mutex);
        return thread_safe_result<const T>(std::move(lock), m_vector.data());
    }

    // Capacity

    bool empty() const
    {
        lock_guard lock(m_mutex);
        return m_vector.empty();
    }

    size_type size() const
    {
        lock_guard lock(m_mutex);
        return m_vector.size();
    }

    size_type max_size() const
    {
        lock_guard lock(m_mutex);
        return m_vector.max_size();
    }

    void reserve(size_type new_cap)
    {
        lock_guard lock(m_mutex);
        m_vector.reserve(new_cap);
    }

    size_type capacity() const
    {
        lock_guard lock(m_mutex);
        return m_vector.capacity();
    }

    void shrink_to_fit()
    {
        lock_guard lock(m_mutex);
        m_vector.shrink_to_fit();
    }

    // Modifiers

    void clear()
    {
        lock_guard lock(m_mutex);
        m_vector.clear();
    }

    iterator insert(iterator pos, const T& value)
    {
        return iterator(m_vector.insert(pos.non_thread_safe_iterator_unsafe(), value), thread_safe_result_iter_lock(pos.lock(), adopt_lock));
    }

    iterator insert(iterator pos, T&& value)
    {
        return iterator(m_vector.insert(pos.non_thread_safe_iterator_unsafe(), std::move(value)), thread_safe_result_iter_lock(pos.lock(), adopt_lock));
    }

    template<class... Args>
    iterator emplace(iterator pos, Args&&... args)
    {
        return iterator(m_vector.emplace(pos.non_thread_safe_iterator_unsafe(), std::forward<Args>(args)...), thread_safe_result_iter_lock(pos.lock(), adopt_lock));
    }

    iterator erase(iterator pos)
    {
        return iterator(m_vector.erase(pos.non_thread_safe_iterator_unsafe()), thread_safe_result_iter_lock(pos.lock(), adopt_lock));
    }

    iterator erase(iterator first, iterator last)
    {
        // assert(first.lock() == last.lock());
        return iterator(m_vector.erase(first.non_thread_safe_iterator_unsafe(), last.non_thread_safe_iterator_unsafe()), thread_safe_result_iter_lock(first.lock(), adopt_lock));
    }

    thread_safe_iterable<vector> iter()
    {
        return thread_safe_iterable<vector>(m_mutex, m_vector);
    }

    void push_back(const T& value)
    {
        lock_guard lock(m_mutex);
        m_vector.push_back(value);
    }

    void push_back(T&& value)
    {
        lock_guard lock(m_mutex);
        m_vector.push_back(std::move(value));
    }

    template<class... Args>
    void emplace_back(Args&&... args)
    {
        emplace(iter().end(), std::forward<Args>(args)...);
    }

    void pop_back()
    {
        lock_guard lock(m_mutex);
        m_vector.pop_back();
    }

    void resize(size_type count)
    {
        lock_guard lock(m_mutex);
        m_vector.resize(count);
    }

    void resize(size_type count, const T& value)
    {
        lock_guard lock(m_mutex);
        m_vector.resize(count, value);
    }

    void swap(vector& other)
    {
        std::lock(m_mutex, other.m_mutex);
        m_vector.swap(other.m_vector);
        std::unlock(m_mutex, other.m_mutex);
    }

    bool operator==(const vector& other) const
    {
        std::lock(m_mutex, other.m_mutex);
        bool value = m_vector == other.m_vector;
        std::unlock(m_mutex, other.m_mutex);
        return value;
    }

    bool operator!=(const vector& other) const
    {
        return !(*this == other);
    }
};

}
}
}
}
