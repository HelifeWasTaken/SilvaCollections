/**
 * hl/silva/collections/meta.hpp
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

#include <variant>
#include <type_traits>

namespace hl
{
namespace silva
{
namespace collections
{
namespace meta
{
    /**
     * @brief Makes a class non assignable by copy when inherited
     */
    class NonAssignableCopy {
    public:
        NonAssignableCopy& operator=(const NonAssignableCopy&) = delete;
        NonAssignableCopy() = default;
        virtual ~NonAssignableCopy() = default;
    };

    /**
     * @brief Makes a class non assignable by move when inherited
     */
    class NonAssignableMove {
    public:
        NonAssignableMove& operator=(NonAssignableMove&&) = delete;
        NonAssignableMove() = default;
        virtual ~NonAssignableMove() = default;
    };

    /**
     * @brief Makes a class non constructible by copy when inherited
     */
    class NonConstructibleCopy {
    public:
        NonConstructibleCopy(const NonConstructibleCopy&) = delete;
        NonConstructibleCopy() = default;
        virtual ~NonConstructibleCopy() = default;
    };

    /**
     * @brief Makes a class non constructible by move when inherited
     */
    class NonConstructibleMove {
    public:
        NonConstructibleMove(NonConstructibleMove&&) = delete;
        NonConstructibleMove() = default;
        virtual ~NonConstructibleMove() = default;
    };

    /**
     * @brief Makes a class non copyable when inherited
     */
    class NonCopyable : public NonAssignableCopy, public NonAssignableMove {
    public:
        NonCopyable() = default;
        virtual ~NonCopyable() override = default;
    };

    /**
     * @brief Makes a class non moveable when inherited
     */
    class NonMoveable : public NonConstructibleMove, public NonConstructibleCopy {
    public:
        NonMoveable() = default;
        virtual ~NonMoveable() override = default;
    };

    /**
     * @brief Makes a class non copyable and non moveable when inherited
     */
    class NonCopyMoveable : public NonCopyable, public NonMoveable {
    public:
        NonCopyMoveable() = default;
        virtual ~NonCopyMoveable() override = default;
    };

    /**
     * @brief Makes a class non constructible when inherited
     */
    class NonConstructible {
    public:
        NonConstructible() = delete;
        virtual ~NonConstructible() = default;
    };

    /**
     * @brief Makes a class non destructible when inherited
     */
    class NonDestructible {
    public:
        NonDestructible() = default;
        virtual ~NonDestructible() = delete;
    };

    /**
     * @brief Enable when arithmetic
     */
    template<typename T>
    using is_arithmetic = std::enable_if_t<std::is_arithmetic<T>::value, bool>;

    /**
     * @brief Get the index of a type in a variant
     * @tparam VariantType The variant type
     * @tparam T The type to find
     * @tparam index The index of the variant
     * @return The index of the type in the variant
     */
    template<typename VariantType, typename T, unsigned long int index = 0>
    constexpr unsigned long int variant_index() {
        static_assert(std::variant_size_v<VariantType> > index, "Type not found in variant");
        if constexpr (index == std::variant_size_v<VariantType>) {
            return index;
        } else if constexpr (std::is_same_v<std::variant_alternative_t<index, VariantType>, T>) {
            return index;
        } else {
            return variant_index<VariantType, T, index + 1>();
        }
    } 

}
}
}
}
