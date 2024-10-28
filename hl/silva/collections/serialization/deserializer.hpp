/**
 * hl/silva/collections/serialization/deserializer.hpp
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

class deserializer
{
private:
    byte_vector m_buffer;

    stdint::size_t m_index = 0;

public:
    deserializer() = default;
    ~deserializer() = default;

    deserializer(const byte_vector& buffer,
                byte_vector& rest_buffer,
                const serialization::metadata::magic_number &expected_magic=serialization::metadata::magic_number())
        : m_buffer(buffer)
    {
        serialization::metadata::header_size_type size;
        serialization::metadata::magic_number magic;

        serialization::metadata::load_header(buffer, magic, size);

        if (magic.v32 != expected_magic.v32)
        {
            throw error("Magic number does not match the expected magic number");
        }

        m_index = serialization::metadata::HEADER_SIZE;

        if (m_index + size > m_buffer.size())
        {
            throw error("Buffer is too small to contain the data");
        }

        if (m_buffer[m_index + size - sizeof(serialization::metadata::type_chart)] != serialization::metadata::type_chart::END)
        {
            throw error("Missing END marker");
        }

        // Extract the rest of the buffer that will be not used by the deserializer
        if (m_index + size < m_buffer.size())
        {
            rest_buffer = byte_vector(m_buffer.begin() + m_index + size, m_buffer.end());
            // drop the end of the buffer that we stored in rest_buffer
            m_buffer.erase(m_buffer.begin() + m_index + size, m_buffer.end());
        }
        else
        {
            rest_buffer = byte_vector();
        }
    }

    deserializer(const deserializer& other) = default;
    deserializer(deserializer&& other) = default;
    deserializer& operator=(const deserializer& other) = default;
    deserializer& operator=(deserializer&& other) = default;

private:
    template<typename T, serialization::metadata::type_chart CType>
    deserializer& _deserialize_inplace_arithmetic(T& value)
    {
        static const size_t MINIMUM_SIZE = sizeof(serialization::metadata::type_chart) + sizeof(T);

        if (m_index + MINIMUM_SIZE > m_buffer.size())
        {
            throw error("Buffer is too small to contain the value");
        }
        else if (m_buffer[m_index] != CType)
        {
            throw error("Type does not match the expected type");
        }

        value = *reinterpret_cast<T*>(&m_buffer[m_index + sizeof(serialization::metadata::type_chart)]);
        m_index += sizeof(serialization::metadata::type_chart) + sizeof(T);

        bit::network_to_native_inplace(value);

        return *this;
    }

    template<typename T, serialization::metadata::type_chart CType>
    deserializer& _deserialize_inplace_array(T& value)
    {
        static const size_t MINIMUM_SIZE = sizeof(serialization::metadata::type_chart) + sizeof(serialization::metadata::type_chart);

        if (m_index + MINIMUM_SIZE > m_buffer.size())
        {
            throw error("Buffer is too small to contain the value");
        }
        else if (m_buffer[m_index] != CType)
        {
            throw error("Type does not match the expected type");
        }

        serialization::metadata::size_type size = *reinterpret_cast<serialization::metadata::size_type*>(&m_buffer[m_index + sizeof(serialization::metadata::type_chart)]);
        bit::network_to_native_inplace(size);

        if (m_index + size + MINIMUM_SIZE > m_buffer.size())
        {
            throw error("Buffer is too small to contain the value");
        }

        m_index += MINIMUM_SIZE;


        value = T(m_buffer.begin() + m_index, m_buffer.begin() + m_index + size);
        m_index += size;

        return *this;
    }

public:
#define SILVA_DESERIALIZER_OPERATOR_NAMED(CTYPE, METADATA_TYPE, MEMBER_FUNC_NAME, METHOD_TYPE) \
    CTYPE get_##MEMBER_FUNC_NAME() { CTYPE value; _deserialize_inplace_##METHOD_TYPE<CTYPE, serialization::metadata::type_chart::METADATA_TYPE>(value); return value; } \
    deserializer& operator>>(CTYPE& value) { return _deserialize_inplace_##METHOD_TYPE<CTYPE, serialization::metadata::type_chart::METADATA_TYPE>(value); } \
    deserializer& get_##MEMBER_FUNC_NAME##_inplace(CTYPE& value) { return *this >> value; }

#define SILVA_DESERIALIZER_OPERATOR_CTYPE_UNIMPLEMENTED(CTYPE, METADATA_TYPE, MEMBER_FUNC_NAME, METHOD_TYPE) \
    CTYPE get_##MEMBER_FUNC_NAME()   { throw error("Not implemented for " #CTYPE); } \
    deserializer& operator>>(CTYPE&) { throw error("Not implemented for " #CTYPE); } \
    deserializer& get_##MEMBER_FUNC_NAME##_inplace(CTYPE&) { throw error("Not implemented for " #CTYPE); }

    SILVA_DESERIALIZER_OPERATOR_NAMED(stdint::u8,     U8,     u8,     arithmetic);
    SILVA_DESERIALIZER_OPERATOR_NAMED(stdint::u16,    U16,    u16,    arithmetic);
    SILVA_DESERIALIZER_OPERATOR_NAMED(stdint::u32,    U32,    u32,    arithmetic);
    SILVA_DESERIALIZER_OPERATOR_NAMED(stdint::u64,    U64,    u64,    arithmetic);
    SILVA_DESERIALIZER_OPERATOR_NAMED(stdint::i8,     I8,     i8,     arithmetic);
    SILVA_DESERIALIZER_OPERATOR_NAMED(stdint::i16,    I16,    i16,    arithmetic);
    SILVA_DESERIALIZER_OPERATOR_NAMED(stdint::i32,    I32,    i32,    arithmetic);
    SILVA_DESERIALIZER_OPERATOR_NAMED(stdint::i64,    I64,    i64,    arithmetic);
    
    SILVA_DESERIALIZER_OPERATOR_CTYPE_UNIMPLEMENTED(stdint::f32, F32, f32, arithmetic);
    SILVA_DESERIALIZER_OPERATOR_CTYPE_UNIMPLEMENTED(stdint::f64, F64, f64, arithmetic);

    SILVA_DESERIALIZER_OPERATOR_NAMED(stdint::bool8,    BOOL8,      bool8,  arithmetic);

    SILVA_DESERIALIZER_OPERATOR_NAMED(std::string, STRING,      string,     array);
    SILVA_DESERIALIZER_OPERATOR_NAMED(byte_vector, BYTE_ARRAY,  byte_array, array);

#undef SILVA_DESERIALIZER_OPERATOR_NAMED
#undef SILVA_DESERIALIZER_OPERATOR_CTYPE_ARITHMETIC
#undef SILVA_DESERIALIZER_OPERATOR_CTYPE_UNIMPLEMENTED

    deserializer& operator>>(serialization::metadata::type_value& value)
    {
        if (m_index + sizeof(serialization::metadata::type_chart) > m_buffer.size())
        {
            throw error("Buffer is too small to contain the value");
        }
        switch ((serialization::metadata::type_chart)m_buffer[m_index])
        {
            #define SILVA_DESERIALIZER_SWITCH_CASE_TYPE_VALUE(TYPE, CTYPE) \
            case serialization::metadata::type_chart::TYPE: { \
                value = get_##CTYPE(); \
                break; \
            }
            
            SILVA_DESERIALIZER_SWITCH_CASE_TYPE_VALUE(U8, u8);
            SILVA_DESERIALIZER_SWITCH_CASE_TYPE_VALUE(U16, u16);
            SILVA_DESERIALIZER_SWITCH_CASE_TYPE_VALUE(U32, u32);
            SILVA_DESERIALIZER_SWITCH_CASE_TYPE_VALUE(U64, u64);
            SILVA_DESERIALIZER_SWITCH_CASE_TYPE_VALUE(I8, i8);
            SILVA_DESERIALIZER_SWITCH_CASE_TYPE_VALUE(I16, i16);
            SILVA_DESERIALIZER_SWITCH_CASE_TYPE_VALUE(I32, i32);
            SILVA_DESERIALIZER_SWITCH_CASE_TYPE_VALUE(I64, i64);
            SILVA_DESERIALIZER_SWITCH_CASE_TYPE_VALUE(F32, f32);
            SILVA_DESERIALIZER_SWITCH_CASE_TYPE_VALUE(F64, f64);
            SILVA_DESERIALIZER_SWITCH_CASE_TYPE_VALUE(BOOL8, bool8);
            SILVA_DESERIALIZER_SWITCH_CASE_TYPE_VALUE(STRING, string);
            SILVA_DESERIALIZER_SWITCH_CASE_TYPE_VALUE(BYTE_ARRAY, byte_array);

            #undef SILVA_DESERIALIZER_SWITCH_CASE_TYPE_VALUE

            case serialization::metadata::type_chart::END:
                value = nullptr;
                m_index += sizeof(serialization::metadata::type_chart);
                break;

            default:
                throw error("Unknown type??");
                break;
        }
        return *this;
    }

    serialization::metadata::type_value get_type_value()
    {
        serialization::metadata::type_value value;
        *this >> value;
        return value;
    }

    const byte_vector& get_buffer() const
    {
        return m_buffer;
    }

    bool is_at_end() const
    {
        return m_index >= m_buffer.size();
    }

    stdint::size_t get_index() const
    {
        return m_index;
    }

    void seek(const stdint::size_t &index)
    {
        if (index >= m_buffer.size())
        {
            throw error("Index out of bounds");
        }
        m_index = index;
    }

    // to make it compatible with std::for_each

    class iterable
    {
    private:
        deserializer &_deserializer;
        serialization::metadata::type_value _last_value;
        stdint::size_t _index;
        const stdint::size_t _original_index;

    public:
        iterable(deserializer &deserializer, const stdint::size_t& index = 0)
            : _deserializer(deserializer)
            , _last_value(nullptr)
            , _index(index)
            , _original_index(deserializer.get_index())
        {
            if (_index == 0) {
                _index = serialization::metadata::HEADER_SIZE;
            }
        }

        bool operator!=(const iterable& other) const
        {
            return _index != other._index;
        }

        bool operator==(const iterable& other) const
        {
            return !(*this != other);
        }

        iterable &operator++()
        {
            _deserializer.seek(_index);

            try {
                _last_value = _deserializer.get_type_value();
                _index = _deserializer.get_index();
                _deserializer.seek(_original_index);
            } catch (const error &e) {
                _deserializer.seek(_original_index);
                throw e;
            }

            return *this;
        }

        iterable& operator+=(const stdint::size_t& value)
        {
            for (stdint::size_t i = 0; i < value; i++)
            {
                ++(*this);
            }
            return *this;
        }

        iterable operator+(const stdint::size_t& value)
        {
            return iterable(*this) += value;
        }

        iterable &operator--() = delete;
        iterable operator--(int) = delete;
        iterable operator-(const stdint::size_t& value) = delete;
        iterable operator-=(const stdint::size_t& value) = delete;

        serialization::metadata::type_value operator*()
        {
            return _last_value;
        }

        const serialization::metadata::type_value &operator*() const
        {
            return _last_value;
        }
    };

    iterable begin()
    {
        return iterable(*this);
    }

    iterable end()
    {
        return iterable(*this, m_buffer.size());
    }
};


}
}
}
}
