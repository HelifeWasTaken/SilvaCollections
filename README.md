# SilvaCollections

A Collection of C++ classes and functions that I reuse in my projects often.
All classes are in the `hl::silva` namespace.

Some files are codependent but no external dependencies are required.

## StdInt

Provides `u8`, `u16`, `u32`, `u64`, `i8`, `i16`, `i32`, `i64`, `f32`, `f64` types.
Considers `bool` as a `bool8` type.
Alias `byte` to `u8`.

Checks for `u128`, `i128`, `u256`, `i256`, `f128`, `f256` support.

```cpp
#include <hl/SilvaCollections/Bit>

int main()
{
    hl::silva::u8 a = 0;
    hl::silva::u16 b = 0;

    return 0;
}
```

## Bit

Provide an `endian` enum with `c++17` support...
Provides `hl::silva::bit::swap_endian`
Provides `hl::silva::bit::to_*_endian` functions.
Provides `hl::silva::bit::to_*_endian_inplace` functions.
Provides `hl::silva::bit::native_to_network` and `hl::silva::bit::network_to_native` functions.
Provides `hl::silva::bit::native_to_network_inplace` and `hl::silva::bit::network_to_native_inplace` functions.

```cpp
#include <hl/SilvaCollections/Bit>
#include <cassert>

int main()
{
    hl::silva::u16 native = 0x1234;
    hl::silva::u16 rev = hl::silva::bit::swap_endian(native);
    hl::silva::u16 big = hl::silva::bit::to_big_endian(native); 

    assert(rev == 0x3412);

    if (hl::silva::bit::endian::native == hl::silva::bit::endian::big)
    {
        assert(native == big);
    }
    else
    {
        assert(rev == big);
    }
    return 0;
}
```

## Hexdump

Provides a `hexdump` formatter for `std::ostream`...
Based on [this](https://github.com/zmb3/hexdump) implementation.

```cpp
#include <hl/SilvaCollections/Hexdump>
#include <iostream>

int main()
{
    hl::silva::u8 data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

    std::cout << hl::silva::Hexdump((void*)data, sizeof(data)) << std::endl;
    return 0;
}
```

## Singleton

Provides a `Singleton` template class that ensures only one instance of a class is created.

### StaticSingleton

```cpp
#include <hl/SilvaCollections/Singleton>

struct MySingleton : public hl::silva::StaticSingleton<MySingleton>
{
    MySingleton() = default;
    ~MySingleton() = default;

    int value = 0;
};

int main()
{
    MySingleton::get().value = 42;
    return 0;
    // MySingleton will be destroyed here.
}
```

### DynamicSingleton

```cpp
#include <hl/SilvaCollections/Singleton>

struct MySingleton : public hl::silva::DynamicSingleton<MySingleton>
{
    MySingleton() = default;
    virtual ~MySingleton() override = default;

    int value = 0;
};

int main()
{
    MySingleton::set(new MySingleton());
    MySingleton::get().value = 42;
    MySingleton::set(new MySingleton()); // Deletes the previous instance.
    std::cout << MySingleton::get().value << std::endl;
    // MySingleton::destroy(); Automatically called when the program ends.
    return 0;
}
```

## Serializer/Deserializer

Provides a `Serializer` and `Deserializer` template class that serializes and deserializes objects.

### Basic Example

```cpp
#include <hl/SilvaCollections/Serializer>
#include <iostream>
#include <cassert>

int main()
{
    hl::silva::u8 a = 42;
    hl::silva::f32 b = 3.14f;
    hl::silva:i8 c = 'c';
    std::string d = "Hello, World!";
    hl::silva::byte_vector e = {0x01, 0x02, 0x03, 0x04, 0x05};

    // used in case the buffer passed to the deserializer has more data than 
    // expected. The rest of the buffer will be stored here.
    // In this case the buffer will and should be empty.
    hl::silva::byte_vector rest_buffer; 

    hl::silva::Serializer serializer;
    serializer << a << b << c << d << e;

    hl::silva::Deserializer deserializer(serializer.serialized_buffer(), rest_buffer);
    assert(rest_buffer.empty());

    deserializer >> a >> b >> c >> d >> e;

    assert(a == 42);
    assert(b == 3.14f);
    assert(c == 'c');
    assert(d == "Hello, World!");
    assert(e == hl::silva::byte_vector({0x01, 0x02, 0x03, 0x04, 0x05}));

    return 
}
```

### Using variant to hold any serializable type

```cpp
#include <hl/SilvaCollections/Serializer>
#include <iostream>
#include <cassert>

hl::silva::Serializer serialize_u8()
{
    hl::silva::u8 a = 42;
    hl::silva::Serializer serializer;
    serializer << a;
    return serializer;
}

int main()
{
    hl::silva::Serializer serializer = serialize_u8();
    // used in case the buffer passed to the deserializer has more data than 
    // expected. The rest of the buffer will be stored here.
    // In this case the buffer will and should be empty.
    hl::silva::byte_vector rest_buffer;

    hl::silva::Deserializer deserializer(serializer.serialized_buffer(), rest_buffer);
    assert(rest_buffer.empty());

    hl::silva::serializer_metadata::TypeValue value;
    deserializer >> value;

    assert(value.index() == serializer_metadata::TypeChart::U8);
    assert(value.value == 42);
}
```

### Magic numbers validation

```cpp
#include <hl/SilvaCollections/Serializer>
#include <iostream>
#include <cassert>

int main()
{
    const hl::silva::serializer_metadata::MagicNumber magic_number(hl::silva::u32(0xdeadbeef));

    hl::silva::Serializer serializer(magic_number);
    hl::silva::u8 a = 42;
    serializer << a;

    hl::silva::byte_vector buffer = serializer.serialized_buffer();

    hl::silva::byte_vector rest_buffer;
    hl::silva::Deserializer deserializer(buffer, rest_buffer, magic_number);
    assert(rest_buffer.empty());

    // If the magic number is not the same as the one passed to the deserializer
    // an exception will be thrown.
}
```

Actually even when not specifying a magic number, the serializer will add a default one. `SILVA_SERIALIZER_DEFAULT_MAGIC_NUMBER_V32` is the default magic number.

### Deserializer iterator

```cpp
#include <hl/SilvaCollections/Serializer>
#include <iostream>
#include <cassert>

int main()
{
    hl::silva::u8 a = 42;
    hl::silva::f32 b = 3.14f;
    hl::silva:i8 c = 'c';
    std::string d = "Hello, World!";
    hl::silva::byte_vector e = {0x01, 0x02, 0x03, 0x04, 0x05};

    hl::silva::Serializer serializer;
    serializer << a << b << c << d << e;

    hl::silva::byte_vector rest_buffer;
    hl::silva::Deserializer deserializer(serializer.serialized_buffer(), rest_buffer);

    // for (const hl::silva::serializer_metadata::TypeValue& value : deserializer)
    for (const auto& value : deserializer)
    {
        std::cout << std::to_string(value) << std::endl;
    }
}
```

### Rest buffer non empty

This may happen when the buffer you receive contains more data than expected for example when you are reading from a socket.

```cpp
#include <hl/SilvaCollections/Serializer>
#include <cassert>

int main()
{
    const hl::silva::u8 a = 42;
    const hl::silva::byte_vector overfit = {0x01, 0x02, 0x03, 0x04, 0x05};

    hl::silva::Serializer serializer;
    serializer << a;

    hl::silva::byte_vector serialized_buffer = serializer.serialized_buffer();
    buffer.insert(buffer.end(), overfit.begin(), overfit.end()); // Simulate overfit

    hl::silva::byte_vector rest_buffer;

    hl::silva::Deserializer deserializer(serialized_buffer, rest_buffer);

    assert(rest_buffer == overfit);

    hl::silva::u8 b;
    deserializer >> b;

    assert(a == b);
}
```

## ThreadList

Provides a `ThreadList` class that manages a list of threads that runs asynchronously.

### Regular mode

```cpp
#include <hl/SilvaCollections/ThreadList>
#include <iostream>

void log_atomic(const std::string& message)
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    std::cout << message << std::endl;
}

int main()
{
    // Can run SILVA_THREAD_LIST_DEFAULT_SIZE tasks at the same time.
    hl::silva::ThreadList thread_list;
    // You can specify
    // hl::silva::ThreadList thread_list(10); // Will run 10 tasks at the same time.


    for (int i = 0; i < 100; i++) {
        thread_list.start([i]() {
            log_atomic("Thread " + std::to_string(i) + " started.");
            int sleep_time = rand() % 10;
            log_atomic("Thread " + std::to_string(i) + " sleeping for " + std::to_string(sleep_time) + " seconds.");
            std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
            log_atomic("Thread " + std::to_string(i) + " finished.");
        });
    }

    thread_list.join();
    return 0;
}
```

### GPU Simulator mode

It does not utilize the GPU but simulates the behavior of a GPU by running tasks in parallel in a 'Square' fashion with ThreadIndexes.

To test this please download the `stb_image.h` file from [here](https://github.com/nothings/stb)

```cpp
#include <hl/SilvaCollections/ThreadList>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static hl::silva::u8** load_square_images_of_same_size(
    hl::silva::u32 image_count, hl::silva::u32 &width,
    hl::silva::u32 &height, hl::silva::u32 &channels,
    hl::silva::u32 square_size)
{
    const hl::silva::u8** image = new hl::silva::u8*[image_count]; // prepare to load n images.

    for (hl::silva::u32 i = 0; i < image_count; i++)
    {
        const std::string file_name = "image" + std::to_string(i) + ".png";
        image[i] = stbi_load(file_name.c_str(), &width, &height, &channels, 0);

        if (image[i] == nullptr)
        {
            std::cerr << "Failed to load image." << std::endl;
            goto image_load_error;
        }

        if (i == 0)
        {
            base_width = width;
            base_height = height;
            base_channels = channels;
        }
        else if (width != base_width || height != base_height || channels != base_channels)
        {
            std::cerr << "Images are not the same size." << std::endl;
            goto image_load_error;
        }

        if (width < square_size || height < square_size)
        {
            std::cerr << "Image is too small." << std::endl;
            goto image_load_error;
        }
        else if (width % square_size != 0 || height % square_size != 0)
        {
            std::cerr << "Image is not divisible by 32." << std::endl;
            goto image_load_error;
        }

        continue;

image_load_error:
        for (hl::silva::u32 j = 0; j < i; j++)
        {
            stbi_image_free(image[j]);
        }
        std::cerr << "Failed to load image." << std::endl;
        delete[] image;
        return nullptr;
    }

    return image;
}

int main()
{
    const unsigned int image_count = 4;
    const unsigned int square_size = 32;
    const unsigned int width, height, channels;
    const hl::silva::f32 rotation = 191.513413; // degrees to rotate the image.

    hl::silva::u8** images = load_square_images_of_same_size(image_count, width, height, channels, square_size);
    if (image == nullptr)
    {
        return 1;
    }

    hl::silva::GPUThreadSim thread_list;
    hl::silva::u8** images_out = new hl::silva::u8*[image_count];
    for (hl::silva::u32 i = 0; i < image_count; i++)
    {
        images_out[i] = new hl::silva::u8[width * height * channels];
    }

    // Divide the image into 32 x 32 squares and run the rotation in parallel.
    // Ensure that the image is divisible by 32.
    // We will not handle the case where the image is not divisible by 32.

    const hl::silva::u32 square_count = (width / square_size) * (height / square_size);

    thread_list.start(
        [](hl::silva::ThreadIndex& thread_index,
        const hl::silva::u8** images,  hl::silva::u8** images_out, 
        const hl::silva::u32 width, const hl::silva::u32 height,
        const hl::silva::u32 channels, const hl::silva::u32 square_size,
        const hl::silva::f32 rotation)
        {
            const hl::silva::u32 x = thread_index.x * square_size;
            const hl::silva::u32 y = thread_index.y * square_size;

            for (hl::silva::u32 j = 0; j < square_size; j++)
            {
                for (hl::silva::u32 k = 0; k < square_size; k++)
                {
                    const hl::silva::u32 index = ((y + j) * width + (x + k)) * channels;
                    const hl::silva::u32 index_out = ((x + j) * height + (y + k)) * channels;

                    // Rotate the image
                    // This is a very simple rotation algorithm.
                    // It is not efficient but it is just an example.
                    // You can replace this with a more efficient algorithm.
                    const hl::silva::f32 x = (hl::silva::f32)j - (hl::silva::f32)square_size / 2.0f;
                    const hl::silva::f32 y = (hl::silva::f32)k - (hl::silva::f32)square_size / 2.0f;
                    const hl::silva::f32 x_rot = x * cos(rotation) - y * sin(rotation);
                    const hl::silva::f32 y_rot = x * sin(rotation) + y * cos(rotation);
                    
                    const hl::silva::u32 x_out = (hl::silva::u32)(x_rot + (hl::silva::f32)square_size / 2.0f);
                    const hl::silva::u32 y_out = (hl::silva::u32)(y_rot + (hl::silva::f32)square_size / 2.0f);
                    
                    if (x_out < square_size && y_out < square_size)
                    {
                        for (hl::silva::u32 l = 0; l < channels; l++)
                        {
                            images_out[thread_index.z][index_out + l] = images[thread_index.z][index + l];
                        }
                    }
                    else
                    {
                        for (hl::silva::u32 l = 0; l < channels; l++)
                        {
                            images_out[thread_index.z][index_out + l] = 0;
                        }
                    }
                }
            }
        }, ThreadIndex(square_count, square_count, image_count),
    images, images_out, width, height, channels, square_size, rotation);

    thread_list.join();

    for (hl::silva::u32 i = 0; i < image_count; i++)
    {
        const std::string file_name = "image" + std::to_string(i) + "_out.png";
        stbi_write_png(file_name.c_str(), width, height, channels, images_out[i], width * channels);
        stbi_image_free(images[i]);
        delete[] images_out[i];
    }

    stbi_image_free(image);
    delete[] image_out;

    return 0;
}
```
