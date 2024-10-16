/**
 * hl/silva/collections/threads/_base.hpp
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

#ifndef SILVA_THREAD_LIST_DEFAULT_SIZE
#define SILVA_THREAD_LIST_DEFAULT_SIZE 20
#endif

#include <thread>
#include <condition_variable>
#include <atomic>

namespace hl
{
namespace silva
{
namespace collections
{
namespace threads
{

using thread = std::thread;
namespace this_thread = std::this_thread;

using condition_variable = std::condition_variable;
template<typename T>
using atomic = std::atomic<T>;
using atomic_bool = std::atomic_bool;
using atomic_int = std::atomic_int;
using atomic_size_t = std::atomic_size_t;


}
}
}
}
