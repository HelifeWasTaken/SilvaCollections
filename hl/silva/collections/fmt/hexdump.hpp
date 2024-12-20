/*
Copyright (c) 2014, Zac Bergquist
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/**
 * hl/silva/collections/fmt/hexdump.hpp
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

// This file is based on the implementation of Zac Bergquist
// https://github.com/zmb3/hexdump

#pragma once

#include <hl/silva/collections/fmt/_base.hpp>

namespace hl
{
namespace silva
{
namespace collections
{
namespace fmt
{

/**
 * @brief HexdumpConfig
 * @details A configuration for the hexdump
 */
template <size_t RowSize, bool ShowAscii = true>
struct hexdump_config
{
    /**
     * @brief Construct a new HexdumpConfig object
     * @param  data: The data to dump
     * @param  length: The length of the data
     */
    hexdump_config(const void *data, const size_t &length) :
        m_data(static_cast<const unsigned char *>(data)), m_length(length) { }

    const unsigned char *m_data;
    const size_t m_length;

    /**
     * @brief Print the hexdump
     * @param  out: The output stream
     * @retval The output stream
     */
    std::ostream &print(std::ostream &out, bool print_empty=true) const
    {
        if (m_length == 0)
        {
            if (print_empty)
            {
                out << "0x000000: " << "Empty" << std::endl;
            }
            return out;
        }

        out.fill('0');
        for (size_t i = 0; i < m_length; i += RowSize)
        {
            out << "0x" << std::setw(6) << std::hex << i << ": ";
            for (size_t j = 0; j < RowSize; ++j)
            {
                if (i + j < m_length)
                {
                    out << std::hex << std::setw(2) << static_cast<unsigned short>(m_data[i + j]) << " ";
                }
                else
                {
                    out << "   ";
                }
            }

            HL_IF_CONSTEXPR (ShowAscii)
            {
                out << " ";
                for (size_t j = 0; j < RowSize; ++j)
                {
                    if (i + j < m_length)
                    {
                        out << (std::isprint(m_data[i + j]) ? static_cast<char>(m_data[i + j]) : '.');
                    }
                }
            }
            out << std::endl;
        }
        return out;
    }
};

using hexdump          = hexdump_config<0x10>;
using hexdump_extended = hexdump_config<0x20>;

}
}
}
}

namespace std
{
    /**
     * @brief Operator << overload
     * @param  out: The output stream
     * @param  dump: The dump to print
     * @tparam RowSize: The size of the row
     * @tparam ShowAscii: Show the ascii
     * @retval The output stream
     */
    template<size_t RowSize, bool ShowAscii>
    inline std::ostream& operator<<(std::ostream& out, const hl::silva::collections::fmt::hexdump_config<RowSize, ShowAscii>& dump)
    {
        return dump.print(out);
    }

    /**
     * @brief to_string
     * @param  dump: The dump to convert
     * @tparam RowSize: The size of the row
     * @tparam ShowAscii: Show the ascii
     * @retval The string
     */
    template<size_t RowSize, bool ShowAscii>
    inline std::string to_string(const hl::silva::collections::fmt::hexdump_config<RowSize, ShowAscii>& dump)
    {
        std::ostringstream out;
        dump.print(out);
        return out.str();
    }
}
