# Silva Collections

A Collection of C++ classes and functions that I reuse in my projects often.
All classes are in the `hl::silva` namespace.

Some files are codependent but no external dependencies are required.

## Requirements

Requires at least a `c++17` compiler

## StdInt

Provides `u8`, `u16`, `unsigned int`, `u64`, `i8`, `i16`, `i32`, `i64`, `f32`, `f64` types.
Considers `bool` as a `bool8` type.
Alias `byte` to `u8`.

Checks for `u128`, `i128`, `u256`, `i256`, `f128`, `f256` support.

```cpp
#include <hl/silva/collections/stdint.hpp>

int main()
{
    // hl::silva::collections::stdint::u8, hl::silva::collections::stdint::u16, etc...
    // etc...
    // The typedefs are enabled by default but you can disable them
    u8 a = 0;
    u16 b = 0;

    return 0;
}
```

## Bit

Provide an `endian` enum with `c++17<=` support...
Provides `hl::silva::collections::bit::swap_endian`
Provides `hl::silva::collections::bit::to_*_endian` functions.
Provides `hl::silva::collections::bit::to_*_endian_inplace` functions.
Provides `hl::silva::collections::bit::native_to_network` and `hl::silva::bit::collections::network_to_native` functions.
Provides `hl::silva::bit::native_to_network_inplace` and `hl::silva::bit::network_to_native_inplace` functions.

```cpp
#include <hl/silva/collections/bit/endian.hpp>
#include <cassert>

int main()
{
    u16 native = 0x1234;
    u16 rev = hl::silva::collections::bit::swap_endian(native);
    u16 big = hl::silva::collections::bit::to_big_endian(native); 

    assert(rev == 0x3412);

    if (hl::silva::collections::bit::endian::native == hl::silva::collections::bit::endian::big)
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

## Fmt

### Hexdump

Provides a `hexdump` formatter for `std::ostream`...
Based on [this](https://github.com/zmb3/hexdump) implementation.

```cpp
#include <hl/silva/collections/fmt/hexdump.hpp>
#include <iostream>

int main()
{
    u8 data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

    std::cout << hl::silva::collections::fmt::hexdump((void*)data, sizeof(data)) << std::endl;
    return 0;
}
```

## Serializer/Deserializer

Provides a `serializer` and `deserializer` class that serializes and deserializes native objects.

### Basic Example

```cpp
#include <hl/silva/collections/serialization/serializer.hpp>
#include <hl/silva/collections/serialization/deserializer.hpp>
#include <iostream>
#include <cassert>

int main()
{
    u8 a = 42;
    f32 b = 3.14f;
    i8 c = 'c';
    std::string d = "Hello, World!";

    hl::silva::collections::serialization::byte_vector e = {0x01, 0x02, 0x03, 0x04, 0x05};

    // used in case the buffer passed to the deserializer has more data than 
    // expected. The rest of the buffer will be stored here.
    // In this case the buffer will and should be empty.
    hl::silva::collections::serialization::byte_vector rest_buffer; 

    hl::silva::collections::serialization::serializer serializer;
    serializer << a << b << c << d << e;

    hl::silva::collections::serialization::deserializer deserializer(serializer.serialized_buffer(), rest_buffer);
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
#include <hl/silva/collections/serialization/serializer.hpp>
#include <hl/silva/collections/serialization/deserializer.hpp>
#include <iostream>
#include <cassert>

hl::silva::collections::serialization::serializer serialize_u8()
{
    u8 a = 42;
    hl::silva::serialization::serializer serializer;
    serializer << a;
    return serializer;
}

int main()
{
    auto serializer = serialize_u8();
    // used in case the buffer passed to the deserializer has more data than 
    // expected. The rest of the buffer will be stored here.
    // In this case the buffer will and should be empty.
    hl::silva::collections::serialization::byte_vector rest_buffer;

    hl::silva::collections::serialization::deserializer deserializer(serializer.serialized_buffer(), rest_buffer);
    assert(rest_buffer.empty());

    hl::silva::collections::serialization::metadata::TypeValue value;
    deserializer >> value;

    assert(value.index() == hl::silva::collections::serialization::metadata::TypeChart::U8);
    assert(value.value == 42);
}
```

### Magic numbers validation

```cpp
#include <hl/silva/collections/serialization/serializer.hpp>
#include <hl/silva/collections/serialization/deserializer.hpp>
#include <iostream>
#include <cassert>

int main()
{
    const hl::silva::collections::serialization::metadata::magic_number magic_number(hl::silva::u32(0xdeadbeef));

    hl::silva::collections::serialization::serializer serializer(magic_number);
    u8 a = 42;
    serializer << a;

    hl::silva::collections::serialization::byte_vector buffer = serializer.serialized_buffer();

    hl::silva::collections::serialization::byte_vector rest_buffer;
    hl::silva::collections::serialization::deserializer deserializer(buffer, rest_buffer, magic_number);
    assert(rest_buffer.empty());

    // If the magic number is not the same as the one passed to the deserializer
    // an exception will be thrown.
}
```

Actually even when not specifying a magic number, the serializer will add a default one. `SILVA_SERIALIZER_DEFAULT_MAGIC_NUMBER_V32` is the default magic number.

### Deserializer iterator

```cpp
#include <hl/silva/collections/serialization/serializer.hpp>
#include <hl/silva/collections/serialization/deserializer.hpp>
#include <iostream>
#include <cassert>

int main()
{
    u8 a = 42;
    f32 b = 3.14f;
    i8 c = 'c';
    std::string d = "Hello, World!";
    hl::silva::collections::serialization::byte_vector e = {0x01, 0x02, 0x03, 0x04, 0x05};

    hl::silva::collections::serialization::serializer serializer;
    serializer << a << b << c << d << e;

    hl::silva::collections::serialization::byte_vector rest_buffer;
    hl::silva::collections::serialization::deserializer deserializer(serializer.serialized_buffer(), rest_buffer);

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
#include <hl/silva/collections/serialization/serializer.hpp>
#include <hl/silva/collections/serialization/deserializer.hpp>
#include <cassert>

int main()
{
    const u8 a = 42;
    const hl::silva::byte_vector overfit = {0x01, 0x02, 0x03, 0x04, 0x05};

    hl::silva::collections::serialization::serializer serializer;
    serializer << a;

    hl::silva::collections::serialization::byte_vector serialized_buffer = serializer.serialized_buffer();
    buffer.insert(buffer.end(), overfit.begin(), overfit.end()); // Simulate overfit

    hl::silva::collections::serialization::byte_vector rest_buffer;

    hl::silva::collections::serialization::deserializer deserializer(serialized_buffer, rest_buffer);

    assert(rest_buffer == overfit);

    u8 b;
    deserializer >> b;

    assert(a == b);
}
```

## Threads

Provides a `ThreadList` class that manages a list of threads that runs asynchronously.

### Basic pool async

```cpp
#include <hl/silva/collections/threads/basic_pool_async.hpp>
#include <iostream>
#include <chrono>
#include <string>
#include <fmt>

// Could also use std::osyncstream for atomic logging if c++20 is available.

void log_atomic(const std::string& message)
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    std::cout << message << std::endl;
}

int main()
{
    hl::silva::collections::threads::basic_pool_async pool;

    for (int i = 0; i < 100; i++) {
        pool.start([i]() {
            log_atomic(std::fmt("Thread {} started", i));
            int sleep_time = rand() % 10;
            log_atomic(std::fmt("Thread {} sleeping for {} seconds", i, sleep_time));
            std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
            log_atomic(std::fmt("Thread {} finished sleeping", i));
        });
    }

    // thread_list.join(); // Is automatic at end of scope.
    return 0;
}
```

### GPU Simulator mode

It does not utilize the GPU but simulates the behavior of a GPU by running tasks in parallel in a 'Square' fashion with ThreadIndexes.

To test this please download the `stb_image.h` file from [here](https://github.com/nothings/stb)

This example loads 4 images and rotates them by 191.513413 degrees in parallel and saves the rotated images.
The images must be the same size and divisible by 32 for simplicity of the example.

It can for example assume that the images are 32x32, 64x64, 128x128, 256x256, etc...

```cpp
#include <hl/silva/collections/threads/gpu_sim.hpp>
#include <string>
#include <fmt>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static std::string get_image_name(unsigned int index, const char* prefix = "image", const char* extension = ".png")
{
    return fmt::format("{}{}{}", prefix, index, extension);
}

static unsigned char** load_square_images_of_same_size(
    unsigned int image_count, unsigned int &width,
    unsigned int &height, unsigned int &channels,
    unsigned int square_size)
{
    const unsigned char** image = new unsigned char*[image_count]; // prepare to load n images.

    if (image == nullptr)
    {
        std::cerr << "Failed to allocate memory for images." << std::endl;
        exit(1);
    }

    for (unsigned int i = 0; i < image_count; i++)
    {
        const std::string file_name = get_image_name(i);
        image[i] = (unsigned char*)stbi_load(file_name.c_str(), &width, &height, &channels, 0);

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
        for (unsigned int j = 0; j < i; j++)
        {
            stbi_image_free(image[j]);
        }
        std::cerr << "Failed to load image: " << file_name << std::endl;
        delete[] image;
        exit(1);
    }

    return image;
}

void prepare_images_out(size_t image_count)
{
    unsigned char **images_out = new unsigned char*[image_count];

    if (images_out == nullptr)
    {
        goto free_images;
    }
    for (unsigned int i = 0; i < image_count; i++)
    {
        images_out[i] = new u8[width * height * channels];
        if (images_out[i] == nullptr)
        {
            goto free_images_out;
        }
    }

    return images_out;

free_images_out:
    for (unsigned int i = 0; i < image_count; i++)
    {
        delete[] images_out[i];
    }
    delete[] images_out;
    return nullptr;
}

static const unsigned int image_count = 4;
static const unsigned int square_size = 32;
static const float rotation = 191.513413; // degrees to rotate the image.

int main()
{
    using GPUSim = hl::silva::collections::threads::gpu_sim;

    unsigned int width, height, channels;

    unsigned char **images = load_square_images_of_same_size(image_count, width, height, channels, square_size);
    unsigned char **images_out = prepare_images_out(image_count);

    if (images_out == nullptr)
    {
        std::cerr << "Failed to allocate memory for images_out." << std::endl;
        for (unsigned int i = 0; i < image_count; i++)
        {
            stbi_image_free(images[i]);
        }
        delete[] images;
        return 1;
    }

    GPUSim gpu_sim;

    // Divide the image into 32 x 32 squares and run the rotation in parallel.
    // Ensure that the image is divisible by 32.
    // We will not handle the case where the image is not divisible by 32.

    const unsigned int square_count = (width / square_size) * (height / square_size);

    gpu_sim.start(
        [images, images_out, width, height, channels, square_size, rotation]
        (const GPUSim::ThreadIndex& thread_index)
        {
            const unsigned int x = thread_index.x * square_size;
            const unsigned int y = thread_index.y * square_size;

            for (unsigned int j = 0; j < square_size; j++)
            {
                for (unsigned int k = 0; k < square_size; k++)
                {
                    const unsigned int index = ((y + j) * width + (x + k)) * channels;
                    const unsigned int index_out = ((x + j) * height + (y + k)) * channels;

                    // Rotate the image
                    // This is a very simple rotation algorithm.
                    // It is not efficient but it is just an example.
                    // You can replace this with a more efficient algorithm.
                    const float x = (float)j - (float)square_size / 2.0f;
                    const float y = (float)k - (float)square_size / 2.0f;
                    const float x_rot = x * cos(rotation) - y * sin(rotation);
                    const float y_rot = x * sin(rotation) + y * cos(rotation);
                    
                    const unsigned int x_out = (unsigned int)(x_rot + (float)square_size / 2.0f);
                    const unsigned int y_out = (unsigned int)(y_rot + (float)square_size / 2.0f);
                    
                    if (x_out < square_size && y_out < square_size)
                    {
                        for (unsigned int l = 0; l < channels; l++)
                        {
                            images_out[thread_index.z][index_out + l] = images[thread_index.z][index + l];
                        }
                    }
                    else
                    {
                        for (unsigned int l = 0; l < channels; l++)
                        {
                            images_out[thread_index.z][index_out + l] = 0x0;
                        }
                    }
                }
            }
        }, ThreadIndex(square_count, square_count, image_count));

    gpu_sim.join();

    gpu_sim.start(
        [images, images_out, width, height, channels, rotation]
        (const GPUSim::ThreadIndex& thread_index)
        {
            const std::string file_name = std::fmt("image_{}_rotated.png", std::to_string(thread_index.z));
            stbi_write_png(file_name.c_str(), width, height, channels, images_out[thread_index.z], width * channels);
            stbi_image_free(images_out[thread_index.z]);
            delete[] images[thread_index.z];
        }, ThreadIndex(1, 1, image_count));

    gpu_sim.join();

    delete[] images;
    delete[] images_out;

    return 0;
}
```
