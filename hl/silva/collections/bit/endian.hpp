/**
 * hl/silva/collections/bit/endian.hpp
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

#include <hl/silva/collections/bit/_base.hpp>

namespace hl
{
namespace silva
{
namespace collections
{
namespace bit
{

/**
 * @brief Endianess enum
 * @details Enum for the endianess
 */
enum class endian
{
#if __cplusplus >= 202002L
    big     = static_cast<int>(std::endian::big),
    little  = static_cast<int>(std::endian::little),
    native  = static_cast<int>(std::endian::native),
#else
    big     = __ORDER_BIG_ENDIAN__,
    little  = __ORDER_LITTLE_ENDIAN__,
    native  = __BYTE_ORDER__,
#endif

#ifdef SILVA_COLLECTIONS_BIT_ENABLE_NETWORK_AS_LITTLE
#warning "Using little endianess on the network is not a standard procedure!"
    network = little
#else
    network = big
#endif
};


/**
 * @brief Swap endian
 * @details Swaps the endian of a value
 * 
 * @tparam T The type of the value
 * @param value The value to swap
 * @return The swapped value
 */
template<typename T, meta::enable_when_arithmetic<T> = true>
static inline constexpr T swap_endian(const T &value)
{
    if constexpr (sizeof(T) == 1)
    {
        return value;
    }
#if defined(__GNUC__) || defined(__clang__)
    else if constexpr (sizeof(T) == 2)
    {
        return __builtin_bswap16(value);
    }
    else if constexpr (sizeof(T) == 4)
    {
        return __builtin_bswap32(value);
    }
    else if constexpr (sizeof(T) == 8)
    {
        return __builtin_bswap64(value);
    }
    else if constexpr (sizeof(T) == 16)
    {
        return __builtin_bswap128(value);
    }
#elif defined(_MSC_VER)
    else if constexpr (sizeof(T) == 2)
    {
        return _byteswap_ushort(value);
    }
    else if constexpr (sizeof(T) == 4)
    {
        return _byteswap_ulong(value);
    }
    else if constexpr (sizeof(T) == 8)
    {
        return _byteswap_uint64(value);
    }
#endif
    else
    {
        /*
        // Generic slow implementation for other sizes (not recommended)
        T result = 0;
        for (std::size_t i = 0; i < sizeof(T); i++)
        {
            result |= ((value >> (i * 8)) & 0xFF) << ((sizeof(T) - i - 1) * 8);
        }
        return result;
        */
        static_assert("Unsupported byteswap size, given: " + std::to_string(sizeof(T)) + " bytes but supports only 1, 2, 4, 8 and (16, GNUC/Clang Only) bytes");
    }
}

/**
 * @brief Transform a native value to big
 * @tparam T The type of the value
 * @param value The value to transform
 * @retval T The value in big endian
 */
template<typename T, meta::enable_when_arithmetic<T> = true>
static inline constexpr T to_big_endian(const T &value)
{
    return endian::native == endian::big ? value : swap_endian(value);
}

/**
 * @brief Transform a native value to big (inplace)
 * @param value The value to transform
 * @tparam T The type of the value
 */
template<typename T, meta::enable_when_arithmetic<T> = true>
static inline constexpr void to_big_endian_inplace(T &value)
{
    value = to_big_endian(value);
}

/**
 * @brief Transform a native value to little
 * @tparam T The type of the value
 * @retval T The value in little endian
 */
template<typename T, meta::enable_when_arithmetic<T> = true>
static inline constexpr T to_little_endian(const T &value)
{
    return endian::native == endian::little ? value : swap_endian(value);
}

/**
 * @brief Transform a native value to little endian
 * @tparam T The type of the value
 * @param value The value to transform
 * @retval T The value in little endian
 */
template<typename T, meta::enable_when_arithmetic<T> = true>
static inline constexpr void to_little_endian_inplace(T &value)
{
    value = to_little_endian(value);
}

/**
 * @brief Swap the value to network endian (big)
 * @tparam T The type of the value
 * @param value The value to swap
 * @retval T The value in network endian
 * @note This function assumes that the given value is in native endian
 *       Applying twice this function will return the original value everytime
 *       Which means that network_to_native(value) == native_to_network(value)
 *       Thoses functions are the same but are named differently for readability
 */
template<typename T, meta::enable_when_arithmetic<T> = true>
static inline constexpr T native_to_network(const T &value)
{
    if constexpr(endian::native != endian::network) {
        return swap_endian(value);
    }
    return value;
}

/**
 * @brief Swap the value to native (big)
 * @tparam T The type of the value
 * @param value The value to swap
 * @retval T The value in network endian
 * @note This function assumes that the given value is in network endian
 *       Applying twice this function will return the original value everytime
 *       Which means that network_to_native(value) == native_to_network(value)
 *       Thoses functions are the same but are named differently for readability
 */
template<typename T, meta::enable_when_arithmetic<T> = true>
static inline constexpr T network_to_native(const T &value)
{
    return native_to_network(value); // actually does the same (is the same as calling byteswap but only checks if the endianess is already big)
}

/**
 * @brief Swap the value to network endian (big) (inplace)
 * @tparam T The type of the value
 * @param value The value to swap
 * @note This function assumes that the given value is in native endian
 *       Applying twice this function will return the original value everytime
 *       Which means that network_to_native(value) == native_to_network(value)
 *       Thoses functions are the same but are named differently for readability
 */
template<typename T, meta::enable_when_arithmetic<T> = true>
static inline constexpr void native_to_network_inplace(T &value)
{
    value = native_to_network(value);
}

/**
 * @brief Swap the value to native (big) (inplace)
 * @tparam T The type of the value
 * @param value The value to swap
 * @note This function assumes that the given value is in network endian
 *       Applying twice this function will return the original value everytime
 *       Which means that network_to_native(value) == native_to_network(value)
 *       Thoses functions are the same but are named differently for readability
 */
template<typename T, meta::enable_when_arithmetic<T> = true>
static inline constexpr void network_to_native_inplace(T &value)
{
    value = network_to_native(value);
}

}
}
}
}