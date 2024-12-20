/**
 * hl/silva/collections/stdint/ext.hpp
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

#include <hl/silva/collections/stdint/_base.hpp>

#ifdef __SIZEOF_INT128__
#define HL_SILVA_COLLECTIONS_STD_INT_INT128 1
#endif

#ifdef __SIZEOF_INT256__
#define HL_SILVA_COLLECTIONS_STD_INT_INT256 1
#endif

#ifdef __SIZEOF_INT512__
#define HL_SILVA_COLLECTIONS_STD_INT_INT512 1
#endif

#ifdef __SIZEOF_FLOAT128__
#define HL_SILVA_COLLECTIONS_STD_INT_FLOAT128 1
#endif

#ifdef __SIZEOF_FLOAT256__
#define HL_SILVA_COLLECTIONS_STD_INT_FLOAT256 1
#endif

#ifdef __SIZEOF_FLOAT512__
#define HL_SILVA_COLLECTIONS_STD_INT_FLOAT512 1
#endif

namespace hl
{
namespace silva
{
namespace collections
{
namespace stdint
{

using byte  = std::uint8_t;

#if defined(HL_SILVA_COLLECTIONS_STD_INT_INT128)
using u128  = __uint128_t;
using i128  = __int128_t;
#endif

#if defined(HL_SILVA_COLLECTIONS_STD_INT_INT256)
using u256  = __uint256_t;
using i256  = __int256_t;
#endif

#if defined(HL_SILVA_COLLECTIONS_STD_INT_INT512)
using u512  = __uint512_t;
using i512  = __int512_t;
#endif

#if defined(HL_SILVA_COLLECTIONS_STD_INT_FLOAT128)
using f128  = __float128;
#endif

#if defined(HL_SILVA_COLLECTIONS_STD_INT_FLOAT256)
using f256  = __float256;
#endif

#if defined(HL_SILVA_COLLECTIONS_STD_INT_FLOAT512)
using f512  = __float512;
#endif

}
}
}
}
