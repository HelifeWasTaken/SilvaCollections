/**
 * hl/silva/collections/stdint/typedef.hpp
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

#if HL_SILVA_COLLECTIONS_STDINT_ALLOW_TYPEDEF == 1

#include <hl/silva/collections/stdint/default.hpp>
#include <hl/silva/collections/stdint/ext.hpp>

typedef hl::silva::collections::stdint::u8 u8;
typedef hl::silva::collections::stdint::u16 u16;
typedef hl::silva::collections::stdint::u32 u32;
typedef hl::silva::collections::stdint::u64 u64;
typedef hl::silva::collections::stdint::i8 i8;
typedef hl::silva::collections::stdint::i16 i16;
typedef hl::silva::collections::stdint::i32 i32;
typedef hl::silva::collections::stdint::i64 i64;
typedef hl::silva::collections::stdint::f32 f32;
typedef hl::silva::collections::stdint::f64 f64;
typedef hl::silva::collections::stdint::bool8 bool8;

typedef hl::silva::collections::stdint::byte byte;

#if defined(HL_SILVA_COLLECTIONS_STD_INT_INT128)
typedef hl::silva::collections::stdint::u128 u128;
typedef hl::silva::collections::stdint::i128 i128;
#endif

#if defined(HL_SILVA_COLLECTIONS_STD_INT_INT256)
typedef hl::silva::collections::stdint::u256 u256;
typedef hl::silva::collections::stdint::i256 i256;
#endif

#if defined(HL_SILVA_COLLECTIONS_STD_INT_INT512)
typedef hl::silva::collections::stdint::u512 u512;
typedef hl::silva::collections::stdint::i512 i512;
#endif

#if defined(HL_SILVA_COLLECTIONS_STD_INT_FLOAT128)
typedef hl::silva::collections::stdint::f128 f128;
#endif

#if defined(HL_SILVA_COLLECTIONS_STD_INT_FLOAT256)
typedef hl::silva::collections::stdint::f256 f256;
#endif

#if defined(HL_SILVA_COLLECTIONS_STD_INT_FLOAT512)
typedef hl::silva::collections::stdint::f512 f512;
#endif

#endif
