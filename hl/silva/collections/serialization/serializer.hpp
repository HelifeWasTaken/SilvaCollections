/**
 * hl/silva/collections/serialization/serializer.hpp
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

#include <hl/silva/collections/serialization/metadata.hpp>

namespace hl
{
namespace silva
{
namespace collections
{
namespace serialization
{

class serializer
{
private:
    byte_vector m_buffer;
    serialization::metadata::magic_number m_magic;

public:
    serializer() = default;

    serializer(const serializer& other) = default;
    serializer(serializer&& other) = default;
    serializer& operator=(const serializer& other) = default;
    serializer& operator=(serializer&& other) = default;

    serializer(const serialization::metadata::magic_number& magic)
        : m_buffer()
        , m_magic(magic)
    {}

    ~serializer() = default;

private:
    template<class T, serialization::metadata::type_chart CType>
    serializer& _serialize_arithmetic_inplace(const T& value)
    {
        m_buffer.push_back(CType);
        T big_value = bit::native_to_network(value);
        m_buffer.insert(m_buffer.end(), (stdint::byte*)&big_value, (stdint::byte*)&big_value + sizeof(T));
        return *this;
    }

    template<class T, serialization::metadata::type_chart CType>
    serializer& _serialize_array_inplace(const T& value)
    {
        m_buffer.push_back(CType);
        stdint::u64 size = value.size();
        stdint::u64 big_size = bit::native_to_network(size);
        m_buffer.insert(m_buffer.end(), (stdint::byte*)&big_size, (stdint::byte*)&big_size + sizeof(stdint::u64));
        m_buffer.insert(m_buffer.end(), value.begin(), value.end());
        return *this;
    }

public:

#define SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL(CTYPE, FUNC_NAME, TYPE_CHART, OPERATION_TYPE) \
    serializer& operator<<(const CTYPE& value) { return _serialize_##OPERATION_TYPE##_inplace<CTYPE, serialization::metadata::type_chart::TYPE_CHART>(value); } \
    serializer& operator<<(CTYPE&& value)      { return *this << value; } \
    serializer& serialize_##FUNC_NAME(const CTYPE& value) { return *this << value; } \

#define SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL_WITH_OP(CTYPE, FUNC_NAME, OPERATION) \
    serializer& operator<<(const CTYPE& value) { { OPERATION; } return *this; } \
    serializer& operator<<(CTYPE&& value)      { return *this << value; } \
    serializer& serialize_##FUNC_NAME(const CTYPE& value) { return *this << value; } \

#define SILVA_SERIALIZER_MAKE_OPERATOR_UNIMPLEMENTED(CTYPE, FUNC_NAME) \
    SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL_WITH_OP(CTYPE, FUNC_NAME, (void)value; throw error("Not implemented for " #CTYPE))

    SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL(stdint::u8,     u8,     U8,     arithmetic);
    SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL(stdint::u16,    u16,    U16,    arithmetic);
    SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL(stdint::u32,    u32,    U32,    arithmetic);
    SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL(stdint::u64,    u64,    U64,    arithmetic);
    SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL(stdint::i8,     i8,     I8,     arithmetic);
    SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL(stdint::i16,    i16,    I16,    arithmetic);
    SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL(stdint::i32,    i32,    I32,    arithmetic);
    SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL(stdint::i64,    i64,    I64,    arithmetic);

    SILVA_SERIALIZER_MAKE_OPERATOR_UNIMPLEMENTED(stdint::f32, f32);
    SILVA_SERIALIZER_MAKE_OPERATOR_UNIMPLEMENTED(stdint::f64, f64);

    SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL(stdint::bool8, bool8, BOOL8, arithmetic);

    SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL(std::string,    string,     STRING,     array);
    SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL(byte_vector,    byte_array, BYTE_ARRAY, array);

    SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL_WITH_OP(char *, cstring, *this << std::string(value));
    SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL_WITH_OP(serialization::metadata::type_value, type_value, std::visit([&](auto&& arg) { *this << arg; }, value));

#undef SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL
#undef SILVA_SERIALIZER_MAKE_OPERATOR_MANUAL_WITH_OP
#undef SILVA_SERIALIZER_MAKE_OPERATOR_UNIMPLEMENTED

    // wildcard section
    template<typename T>
    serializer& serialize(const T& value)
    {
        return *this << value;
    }

    template<typename T>
    serializer& serialize(T&& value)
    {
        return *this << value;
    }

    const byte_vector &get_raw_buffer() const
    {
        return m_buffer;
    }

    byte_vector serialized_buffer() const
    {
        byte_vector buffer = m_buffer;

        buffer.push_back(serialization::metadata::type_chart::END);
        serialization::metadata::header_byte_array header = serialization::metadata::make_header(m_magic, buffer.size());

        buffer.insert(buffer.begin(), header.begin(), header.end());

        return buffer;
    }
};

}
}
}
}
