/**
 * hl/silva/collections/serialization.hpp
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

// u8*4            u64         u8         rodata       u8        rodata
// [magic number]  [size] [[typechartdata]data[0],    [typechart]data[1], ...]
// String: [STRING_TYPE][size][data]
// Array: [BYTE_ARRAY_TYPE][size][data]
// Other: [TYPE][data]
// All sizes and types that validate std::is_arithmetic_v<T> are in network byte order

#if __cplusplus >= 201703L
#include <hl/silva/collections/serialization/deserializer.hpp>
#include <hl/silva/collections/serialization/serializer.hpp>
#else
#error "C++17 or higher is required for this library"
#endif
