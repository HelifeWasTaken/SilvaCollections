/**
 * hl/silva/collections/threads/gpu_sim.hpp
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

#include <hl/silva/collections/threads/basic_pool_async.hpp>

namespace hl
{
namespace silva
{
namespace collections
{
namespace threads
{

/**
 * @brief GPUThreadSim A class that simulates the behavior of a GPU (inspired by CUDA)
 */
class gpu_sim
{
public:
    /**
     * @brief thread_index A structure that represents the index of a thread in a 3D space
     */
    struct thread_index {
        unsigned int x, y, z;
        constexpr thread_index(unsigned int px, unsigned int py, unsigned int pz=1)
            : x(px), y(py), z(pz)
        {};

        constexpr thread_index(const thread_index& index) = default;
        constexpr thread_index& operator=(const thread_index& index) = default;

        constexpr thread_index(void)
            : x(0), y(0), z(0)
        {};

        constexpr bool operator==(const thread_index& index) const
        {
            return x == index.x && y == index.y && z == index.z;
        }

        constexpr bool operator!=(const thread_index& index) const
        {
            return !(*this == index);
        }

        class iterator;
        struct iterable;
    };

    class thread_index::iterator {
    private:
        thread_index _index;
        thread_index _size;

    public:
        constexpr iterator(const thread_index& index, const thread_index& size)
            : _index(index), _size(thread_index(size.x - 1, size.y - 1, size.z - 1))
        {
            if (_index.x > _size.x || _index.y > _size.y || _index.z > _size.z)
            {
                _index = _size;
            }
        }

        constexpr iterator& operator++(void)
        {
            if (_index.x < _size.x)
            {
                _index.x++;
            }
            else if (_index.y < _size.y)
            {
                _index.x = 0;
                _index.y++;
            }
            else if (_index.z < _size.z)
            {
                _index.x = 0;
                _index.y = 0;
                _index.z++;
            }
            return *this;
        }

        constexpr bool operator!=(const iterator& it) const
        {
            return _index != it._index;
        }

        constexpr const thread_index& operator*(void) const
        {
            return _index;
        }
    };

    struct thread_index::iterable {
        const thread_index _index;
        const thread_index _size;

        iterable(const thread_index& size)
            : _index(thread_index()), _size(size)
        {}

        iterable(const thread_index& index, const thread_index& size)
            : _index(index), _size(size)
        {}

        thread_index::iterator begin(void) const
        {
            return thread_index::iterator(_index, _size);
        }

        thread_index::iterator end(void) const
        {
            return thread_index::iterator(_size, _size);
        }
    };

    /**
     * @brief gpu_callback A callback function that is called by a thread
     * @tparam Args The arguments types of the callback function
     */
    template<typename... Args>
    using gpu_callback = std::function<void(const thread_index&, Args...)>;

    /**
     * @brief gpu_index_format A function that formats the index of a thread to a specific format
     */
    using gpu_index_format = std::function<thread_index(const thread_index&)>;

    // Methods to format the index of a thread
    static inline constexpr thread_index XYZ(const thread_index& index) { return index; };
    static inline constexpr thread_index XZY(const thread_index& index) { return thread_index(index.x, index.z, index.y); };
    static inline constexpr thread_index YXZ(const thread_index& index) { return thread_index(index.y, index.x, index.z); };
    static inline constexpr thread_index YZX(const thread_index& index) { return thread_index(index.y, index.z, index.x); };
    static inline constexpr thread_index ZXY(const thread_index& index) { return thread_index(index.z, index.x, index.y); };
    static inline constexpr thread_index ZYX(const thread_index& index) { return thread_index(index.z, index.y, index.x); };

private:
    basic_pool_async _pool;

public:
    /**
     * @brief GPUThreadSim
     * @param thread_count The number of threads that can be used concurrently
     */
    gpu_sim()
        : _pool()
    {}

    /**
     * @brief start_fmt Starts the simulation with a specific format
     * @tparam Args The arguments types of the callback function
     * @param size The size of the 3D space
     * @param callback The callback function
     * @param format The format of the index of a thread
     * @param args The arguments of the callback function
     * @return void
     */
    template<typename... Args>
    void start_fmt(const thread_index& size,
        gpu_callback<Args...>&& callback,
        gpu_index_format&& format,
        Args&&... args)

    {
        for (const thread_index& index : thread_index::iterable(size))
        {
            _pool.start(
                [](const thread_index& lindex, gpu_callback<Args...>&& lcallback, Args&&... largs)
                {
                    lcallback(lindex, std::forward<Args>(largs)...);
                }, format(index), std::forward<gpu_callback<Args...>>(callback), std::forward<Args>(args)...
            );
        }
    }

    /**
     * @brief start Starts the simulation
     * @tparam Args The arguments types of the callback function
     * @param size The size of the 3D space
     * @param callback The callback function
     * @param args The arguments of the callback function
     * @return void
     */
    template<typename... Args>
    void start(const thread_index& size,
        gpu_callback<Args...>&& callback,
        Args&&... args)
    {
        start_fmt(size, std::forward<gpu_callback<Args...>>(callback), XYZ, std::forward<Args>(args)...);
    }

    /**
     * @brief Join all threads stop the simulation and wait until all tasks have finished
     * @return void
     */
    ~gpu_sim()
    {}

    /**
     * @brief join Waits until all current tasks have finished
     * @return void
     */
    void join(void)
    {
        _pool.join();
    }
};

}
}
}
}
