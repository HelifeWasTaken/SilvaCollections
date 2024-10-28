#include <hl/silva/collections.hpp>
#include <string>
#include <memory>
#include <iostream>

struct break_line_t
{
    friend std::ostream& operator<<(std::ostream& os, const break_line_t& bl)
    {
        return os << "----------------------------------------\n\n" << std::endl;
    }
};

static inline HL_CONSTEXPR break_line_t break_line;

int main()
{
    // bit::endian

    hl::silva::collections::bit::endian endian_native  = hl::silva::collections::bit::endian::native;
    hl::silva::collections::bit::endian endian_big     = hl::silva::collections::bit::endian::big;
    hl::silva::collections::bit::endian endian_little  = hl::silva::collections::bit::endian::little;
    hl::silva::collections::bit::endian endian_network = hl::silva::collections::bit::endian::network;

    std::cout << break_line;
    std::cout << "Endian: "     << std::endl;
    std::cout << "\tNative: "   << hl::silva::collections::bit::endian_to_string(endian_native) << std::endl;
    std::cout << "\tBig: "      << hl::silva::collections::bit::endian_to_string(endian_big) << std::endl;
    std::cout << "\tLittle: "   << hl::silva::collections::bit::endian_to_string(endian_little) << std::endl;
    std::cout << "\tNetwork: "  << hl::silva::collections::bit::endian_to_string(endian_network) << std::endl;
    std::cout << std::endl;

    // bit::swap_endian

    u8 u8value = 0x12;
    u16 u16value = 0x1234;
    u32 u32value = 0x12345678;
    u64 u64value = 0x1234567890ABCDEF;

    u8 u8value_swap   = hl::silva::collections::bit::swap_endian(u8value);
    u16 u16value_swap = hl::silva::collections::bit::swap_endian(u16value);
    u32 u32value_swap = hl::silva::collections::bit::swap_endian(u32value);
    u64 u64value_swap = hl::silva::collections::bit::swap_endian(u64value);

    u8 u8value_big   = hl::silva::collections::bit::to_big_endian(u8value);
    u16 u16value_big = hl::silva::collections::bit::to_big_endian(u16value);
    u32 u32value_big = hl::silva::collections::bit::to_big_endian(u32value);
    u64 u64value_big = hl::silva::collections::bit::to_big_endian(u64value);

    u8 u8value_little   = hl::silva::collections::bit::to_little_endian(u8value);
    u16 u16value_little = hl::silva::collections::bit::to_little_endian(u16value);
    u32 u32value_little = hl::silva::collections::bit::to_little_endian(u32value);
    u64 u64value_little = hl::silva::collections::bit::to_little_endian(u64value);

    u8 u8value_network   = hl::silva::collections::bit::native_to_network(u8value);
    u16 u16value_network = hl::silva::collections::bit::native_to_network(u16value);
    u32 u32value_network = hl::silva::collections::bit::native_to_network(u32value);
    u64 u64value_network = hl::silva::collections::bit::native_to_network(u64value);

    u8 u8value_native   = hl::silva::collections::bit::network_to_native(u8value_network);
    u16 u16value_native = hl::silva::collections::bit::network_to_native(u16value_network);
    u32 u32value_native = hl::silva::collections::bit::network_to_native(u32value_network);
    u64 u64value_native = hl::silva::collections::bit::network_to_native(u64value_network);

    std::cout << break_line;
    std::cout << "NativeValues: " << std::endl;
    std::cout << "\tU8(asU16): " << std::hex << (u16)u8value << std::endl;
    std::cout << "\tU16: " << std::hex << u16value << std::endl;
    std::cout << "\tU32: " << std::hex << u32value << std::endl;
    std::cout << "\tU64: " << std::hex << u64value << std::endl;
    std::cout << std::endl;
    std::cout << "SwappedValues: " << std::endl;
    std::cout << "\tU8Swap(asU16): " << std::hex << (u16)u8value_swap << std::endl;
    std::cout << "\tU16Swap: " << std::hex << u16value_swap << std::endl;
    std::cout << "\tU32Swap: " << std::hex << u32value_swap << std::endl;
    std::cout << "\tU64Swap: " << std::hex << u64value_swap << std::endl;
    std::cout << std::endl;
    std::cout << "NativeToBigValues: " << std::endl;
    std::cout << "\tU8Big(asU16): " << std::hex << (u16)u8value_big << std::endl;
    std::cout << "\tU16Big: " << std::hex << u16value_big << std::endl;
    std::cout << "\tU32Big: " << std::hex << u32value_big << std::endl;
    std::cout << "\tU64Big: " << std::hex << u64value_big << std::endl;
    std::cout << std::endl;
    std::cout << "NativeToLittleValues: " << std::endl;
    std::cout << "\tU8Little(asU16): " << std::hex << (u16)u8value_little << std::endl;
    std::cout << "\tU16Little: " << std::hex << u16value_little << std::endl;
    std::cout << "\tU32Little: " << std::hex << u32value_little << std::endl;
    std::cout << "\tU64Little: " << std::hex << u64value_little << std::endl;
    std::cout << std::endl;
    std::cout << "NativeNetworkValues: " << std::endl;
    std::cout << "\tU8Network(asU16): " << std::hex << (u16)u8value_network << std::endl;
    std::cout << "\tU16Network: " << std::hex << u16value_network << std::endl;
    std::cout << "\tU32Network: " << std::hex << u32value_network << std::endl;
    std::cout << "\tU64Network: " << std::hex << u64value_network << std::endl;
    std::cout << std::endl;
    std::cout << "NetworkToNative: " << std::endl;
    std::cout << "\tU8Native(asU16): " << std::hex << (u16)u8value_native << std::endl;
    std::cout << "\tU16Native: " << std::hex << u16value_native << std::endl;
    std::cout << "\tU32Native: " << std::hex << u32value_native << std::endl;
    std::cout << "\tU64Native: " << std::hex << u64value_native << std::endl;
    std::cout << std::endl;

    // fmt

    // fmt::hexdump
    const u8 data[] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };
    hl::silva::collections::fmt::hexdump data_hexdump(data, sizeof(data));
    std::cout << break_line;
    std::cout << "Hexdump: " << std::endl;
    std::cout << data_hexdump << std::endl;
    std::cout << std::endl;

    // serialization

    hl::silva::collections::serialization::metadata::magic_number magic(0xDEADBEEF);

    hl::silva::collections::serialization::serializer serializer(magic);
    hl::silva::collections::serialization::byte_vector rest_buffer;
    hl::silva::collections::serialization::byte_vector overfit_buffer = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD };

    std::cout << break_line;
    std::cout << "Serialization: " << std::endl;
    std::cout << "\tMagicNumber: " << std::hex << magic.v32 << std::endl;
    std::cout << "\tOverfitBuffer: " << std::endl;
    std::cout << hl::silva::collections::fmt::hexdump(overfit_buffer.data(), overfit_buffer.size()) << std::endl << std::endl;
    std::cout << std::endl;

    std::cout << "\tSerializing: " << std::endl;
    std::cout << "\t\t[0] U8(asU16): " << std::hex << (u16)u8value << std::endl;
    std::cout << "\t\t[1] U16: " << std::hex << u16value << std::endl;
    std::cout << "\t\t[2] U32: " << std::hex << u32value << std::endl;
    std::cout << "\t\t[3] U64: " << std::hex << u64value << std::endl;
    std::cout << std::endl;

    serializer << u8value << u16value << u32value << u64value;

    auto serialized_buffer = serializer.serialized_buffer();

    std::cout << "\tSerializedBuffer: " << std::endl;
    std::cout << hl::silva::collections::fmt::hexdump(serialized_buffer.data(), serialized_buffer.size()) << std::endl;
    std::cout << std::endl;

    serialized_buffer.insert(serialized_buffer.end(), overfit_buffer.begin(), overfit_buffer.end());

    std::cout << "\tSerializedBufferWithOverfit: " << std::endl;
    std::cout << hl::silva::collections::fmt::hexdump(serialized_buffer.data(), serialized_buffer.size()) << std::endl;
    std::cout << std::endl;

    hl::silva::collections::serialization::deserializer deserializer(serialized_buffer, rest_buffer, magic);

    u8 u8value_deserialized;
    u16 u16value_deserialized;
    u32 u32value_deserialized;
    u64 u64value_deserialized;

    std::cout << "\tDeserializing: " << std::endl;
    deserializer >> u8value_deserialized >> u16value_deserialized >> u32value_deserialized >> u64value_deserialized;
    std::cout << "\t\t[0] U8(asU16): " << std::hex << (u16)u8value_deserialized << std::endl;
    std::cout << "\t\t[1] U16: " << std::hex << u16value_deserialized << std::endl;
    std::cout << "\t\t[2] U32: " << std::hex << u32value_deserialized << std::endl;
    std::cout << "\t\t[3] U64: " << std::hex << u64value_deserialized << std::endl;
    std::cout << std::endl;

    std::cout << "\tRestBuffer: (SHOULD BE EQUAL TO OVERFIT_BUFFER)" << std::endl;
    std::cout << hl::silva::collections::fmt::hexdump(rest_buffer.data(), rest_buffer.size()) << std::endl;
    std::cout << std::endl;

    if (rest_buffer != overfit_buffer)
    {
        std::cout << "\tRestBuffer is not equal to OverfitBuffer!" << std::endl;
    }
    if (u8value != u8value_deserialized || u16value != u16value_deserialized || u32value != u32value_deserialized || u64value != u64value_deserialized)
    {
        std::cout << "\tValues deserialized are not equal!" << std::endl;
    }

    // thread_safe

	hl::silva::collections::thread_safe::list<std::unique_ptr<std::string>> list;
	hl::silva::collections::thread_safe::vector<std::unique_ptr<std::string>> vec;
	hl::silva::collections::thread_safe::deque<std::unique_ptr<std::string>> dequeue;
	hl::silva::collections::thread_safe::queue<std::unique_ptr<std::string>> queue;

	return 0;
}
