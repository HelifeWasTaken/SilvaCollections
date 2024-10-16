/**
 * hl/silva/collections/thread_safe/queue.hpp
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

#include <hl/silva/collections/thread_safe/deque.hpp>

namespace hl
{
namespace silva
{
namespace collections
{
namespace thread_safe
{

template<class T, class Allocator = default_allocator<T>>
class queue : public deque<T, Allocator>
{
private:
    using deque<T, Allocator>::assign;
    using deque<T, Allocator>::at;
    using deque<T, Allocator>::operator[];
    using deque<T, Allocator>::max_size;
    using deque<T, Allocator>::insert;
    using deque<T, Allocator>::erase;
    using deque<T, Allocator>::pop_back;
    using deque<T, Allocator>::push_front;
    using deque<T, Allocator>::resize;
    using deque<T, Allocator>::poll_back;
    using deque<T, Allocator>::iter;

public:
    using size_type = typename deque<T, Allocator>::size_type;

    inline virtual thread_safe_result<T> front() override final
    {
        return deque<T, Allocator>::front();
    }

    inline virtual thread_safe_result<const T> front() const override final
    {
        return deque<T, Allocator>::front();
    }

    inline virtual thread_safe_result<T> back() override final
    {
        return deque<T, Allocator>::back();
    }

    inline virtual thread_safe_result<const T> back() const override final
    {
        return deque<T, Allocator>::back();
    }

    inline virtual bool empty() const override
    {
        return deque<T, Allocator>::empty();
    }

    inline virtual size_type size() const override
    {
        return deque<T, Allocator>::size();
    }

    inline void push(const T& value)
    {
        deque<T, Allocator>::push_back(value);
    }

    inline void push(T&& value)
    {
        deque<T, Allocator>::push_back(std::move(value));
    }

    template<class... Args>
    inline void emplace(Args&&... args)
    {
        deque<T, Allocator>::emplace_back(std::forward<Args>(args)...);
    }

    inline void pop()
    {
        deque<T, Allocator>::pop_front();
    }

    inline void swap(queue& other)
    {
        deque<T, Allocator>::swap(other);
    }

    queue() = default;
    virtual ~queue() override = default;

    template<bool shrink = false>
    void clear()
    {
        deque<T, Allocator>::clear();
        if constexpr (shrink)
            deque<T, Allocator>::shrink_to_fit();
    }

    [[nodiscard]] bool poll(T& value)
    {
        return deque<T, Allocator>::poll_front(value);
    }
};

}
}
}
}
