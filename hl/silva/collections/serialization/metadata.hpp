/**
 * hl/silva/collections/serialization/metadata.hpp
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

#include <hl/silva/collections/serialization/_base.hpp>

namespace hl
{
namespace silva
{
namespace collections
{
namespace serialization
{
namespace metadata 
{
    using type_value = std::variant<stdint::u8, stdint::u16, stdint::u32, stdint::u64, stdint::i8, stdint::i16, stdint::i32, stdint::i64, stdint::f32, stdint::f64, stdint::bool8, std::string, byte_vector, nullptr_t>;

    enum type_chart : stdint::u8
    {
        BYTE        = meta::variant_index<type_value, stdint::u8>(),
        U8          = meta::variant_index<type_value, stdint::u8>(),
        U16         = meta::variant_index<type_value, stdint::u16>(),
        U32         = meta::variant_index<type_value, stdint::u32>(),
        U64         = meta::variant_index<type_value, stdint::u64>(),
        I8          = meta::variant_index<type_value, stdint::i8>(),
        I16         = meta::variant_index<type_value, stdint::i16>(),
        I32         = meta::variant_index<type_value, stdint::i32>(),
        I64         = meta::variant_index<type_value, stdint::i64>(),
        F32         = meta::variant_index<type_value, stdint::f32>(),
        F64         = meta::variant_index<type_value, stdint::f64>(),
        BOOL8       = meta::variant_index<type_value, stdint::bool8>(),
        STRING      = meta::variant_index<type_value, std::string>(),
        BYTE_ARRAY  = meta::variant_index<type_value, byte_vector>(),

        END         = 0xFF
    };

    static inline std::string to_string(const type_chart &type)
    {
        switch (type)
        {
            // case BYTE: return "BYTE";
            case U8:            return "U8";
            case U16:           return "U16";
            case U32:           return "U32";
            case U64:           return "U64";
            case I8:            return "I8";
            case I16:           return "I16";
            case I32:           return "I32";
            case I64:           return "I64";
            case F32:           return "F32";
            case F64:           return "F64";
            case BOOL8:         return "BOOL8";
            case STRING:        return "STRING";
            case BYTE_ARRAY:    return "BYTE_ARRAY";
            case END:           return "END(or nullptr)";
            default:            return "UNKNOWN";
        }
    }

    static inline std::string to_string(const type_value &value)
    {
        // handle vector and string then print the rest
        return std::visit([&value](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            // print using type chart to string
            if constexpr (std::is_same_v<T, std::string>)
            {
                return "string<'" + arg + "'>";
            }
            else if constexpr (std::is_same_v<T, byte_vector>)
            {
                return "byte_vector<size=" + std::to_string(arg.size()) + ">";
            }
            else if constexpr (std::is_same_v<T, nullptr_t>)
            {
                return "nullptr_t<end_marker>";
            }
            else
            {
                return to_string((type_chart)value.index()) + "<" + std::to_string(arg) + ">";
            }
        }, value);
    }

    using header_size_type = stdint::u64;

    #define SILVA_SERIALIZER_DEFAULT_MAGIC_NUMBER_V8_0 0xb1
    #define SILVA_SERIALIZER_DEFAULT_MAGIC_NUMBER_V8_1 0xa5
    #define SILVA_SERIALIZER_DEFAULT_MAGIC_NUMBER_V8_2 0xed
    #define SILVA_SERIALIZER_DEFAULT_MAGIC_NUMBER_V8_3 0xff

    #define SILVA_SERIALIZER_DEFAULT_MAGIC_NUMBER_V32 \
        ((SILVA_SERIALIZER_DEFAULT_MAGIC_NUMBER_V8_0 << 24) | \
        (SILVA_SERIALIZER_DEFAULT_MAGIC_NUMBER_V8_1 << 16) | \
        (SILVA_SERIALIZER_DEFAULT_MAGIC_NUMBER_V8_2 << 8) | \
        SILVA_SERIALIZER_DEFAULT_MAGIC_NUMBER_V8_3)

    union magic_number
    {
        stdint::u32 v32;
        stdint::byte v8[4];

        magic_number(stdint::u32 value) : v32(value) {}
        magic_number(stdint::byte b0, stdint::byte b1, stdint::byte b2, stdint::byte b3) : v8{b0, b1, b2, b3} {}
        magic_number(const magic_number& other) = default;
        magic_number(magic_number&& other) = default;
        magic_number& operator=(const magic_number& other) = default;
        magic_number& operator=(magic_number&& other) = default;
        magic_number() : v32(SILVA_SERIALIZER_DEFAULT_MAGIC_NUMBER_V32) {}
    };

    using size_type = stdint::u64;

    static const size_type       MAGIC_NUMBER_SIZE           = sizeof(magic_number);
    static const size_type       SIZE_TYPE_SIZE              = sizeof(size_type);
    static const size_type       HEADER_SIZE_TYPE_SIZE       = sizeof(header_size_type);
    static const size_type       TYPE_CHART_SIZE             = sizeof(type_chart);
    static const size_type       HEADER_SIZE                 = MAGIC_NUMBER_SIZE + SIZE_TYPE_SIZE;

    using header_byte_array = std::array<stdint::byte, HEADER_SIZE>;

    static header_byte_array make_header(const magic_number& magic, const header_size_type& size)
    {
        header_byte_array header;
        header_size_type net_size = bit::native_to_network(size);
        magic_number net_magic = bit::native_to_network(magic.v32);

        const stdint::byte* magic_ptr = (stdint::byte*)&net_magic.v8[0];
        const stdint::byte* size_ptr = (stdint::byte*)&net_size;

        std::copy(magic_ptr, magic_ptr + MAGIC_NUMBER_SIZE, header.begin());
        std::copy(size_ptr, size_ptr + SIZE_TYPE_SIZE, header.begin() + MAGIC_NUMBER_SIZE);

        return header;
    }

    static void load_header(const byte_vector& buffer, magic_number& magic, header_size_type& size)
    {
        if (buffer.size() < HEADER_SIZE)
        {
            throw error("Buffer is too small to load the header size it cannot possibly contain it");
        }

        std::copy(buffer.begin(), buffer.begin() + MAGIC_NUMBER_SIZE, magic.v8);
        size = *(const header_size_type*)&buffer[MAGIC_NUMBER_SIZE];

        bit::network_to_native_inplace(size);
        bit::network_to_native_inplace(magic.v32);
    }
}
}
}
}
}

namespace std
{
    std::string to_string(const hl::silva::collections::serialization::metadata::type_chart &type)
    {
        return hl::silva::collections::serialization::metadata::to_string(type);
    }

    std::string to_string(const hl::silva::collections::serialization::metadata::type_value &value)
    {
        return hl::silva::collections::serialization::metadata::to_string(value);
    }
}
