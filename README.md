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
#include <hl/silva/collections/StdInt>

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
#include <hl/silva/collections/Bit>
#include <cassert>

int main()
{
    using hl_bit = hl::silva::collections::bit;

    u16 native = 0x1234;
    u16 rev = hl_bit::swap_endian(native);
    u16 big = hl_bit::to_big_endian(native); 

    assert(rev == 0x3412);

    if (hl_bit::endian::native == hl_bit::endian::big)
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
#include <hl/silva/collections/fmt/Hexdump>
#include <iostream>

int main()
{
    using hl_fmt = hl::silva::collections::fmt;

    u8 data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

    std::cout << hl_fmt::Hexdump((void*)data, sizeof(data)) << std::endl;
    return 0;
}
```

## Singletons

Provides a `Singleton` template class that ensures only one instance of a class is created.

### Static

```cpp
#include <hl/silva/collections/Singletons/Static>

struct MySingleton : public hl::silva::collections::singletons::Static<MySingleton>
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

### Dynamic

```cpp
#include <hl/silva/collections/Singletons/Dynamic>

struct MySingleton : public hl::silva::collections::singletons::Dynamic<MySingleton>
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
#include <hl/silva/collections/Serialization>
#include <iostream>
#include <cassert>

int main()
{
    using hl_serialization = hl::silva::collections::serialization;

    u8 a = 42;
    f32 b = 3.14f;
    i8 c = 'c';
    std::string d = "Hello, World!";

    hl_serialization::byte_vector e = {0x01, 0x02, 0x03, 0x04, 0x05};

    // used in case the buffer passed to the deserializer has more data than 
    // expected. The rest of the buffer will be stored here.
    // In this case the buffer will and should be empty.
    hl_serialization::byte_vector rest_buffer; 

    hl_serialization::Serializer serializer;
    serializer << a << b << c << d << e;

    hl_serialization::Deserializer deserializer(serializer.serialized_buffer(), rest_buffer);
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
#include <hl/silva/collections/Serialization>
#include <iostream>
#include <cassert>

hl::silva::collections::serialization::Serializer serialize_u8()
{
    u8 a = 42;
    hl::silva::serialization::Serializer serializer;
    serializer << a;
    return serializer;
}

int main()
{
    using hl_serialization = hl::silva::collections::serialization;

    auto serializer = serialize_u8();
    // used in case the buffer passed to the deserializer has more data than 
    // expected. The rest of the buffer will be stored here.
    // In this case the buffer will and should be empty.
    hl_serialization::byte_vector rest_buffer;

    hl_serialization::Deserializer deserializer(serializer.serialized_buffer(), rest_buffer);
    assert(rest_buffer.empty());

    hl_serialization::metadata::TypeValue value;
    deserializer >> value;

    assert(value.index() == hl_serialization::metadata::TypeChart::U8);
    assert(value.value == 42);
}
```

### Magic numbers validation

```cpp
#include <hl/silva/collections/Serialization>
#include <iostream>
#include <cassert>

int main()
{
    using hl_serialization = hl::silva::collections::serialization;

    const hl_serialization::metadata::MagicNumber magic_number(hl::silva::unsigned int(0xdeadbeef));

    hl_serialization::Serializer serializer(magic_number);
    u8 a = 42;
    serializer << a;

    hl_serialization::byte_vector buffer = serializer.serialized_buffer();

    hl_serialization::byte_vector rest_buffer;
    hl_serialization::Deserializer deserializer(buffer, rest_buffer, magic_number);
    assert(rest_buffer.empty());

    // If the magic number is not the same as the one passed to the deserializer
    // an exception will be thrown.
}
```

Actually even when not specifying a magic number, the serializer will add a default one. `SILVA_SERIALIZER_DEFAULT_MAGIC_NUMBER_V32` is the default magic number.

### Deserializer iterator

```cpp
#include <hl/silva/collections/Serialization>
#include <iostream>
#include <cassert>

int main()
{
    using hl_serialization = hl::silva::collections::serialization;

    u8 a = 42;
    f32 b = 3.14f;
    i8 c = 'c';
    std::string d = "Hello, World!";
    hl_serialization::byte_vector e = {0x01, 0x02, 0x03, 0x04, 0x05};

    hl_serialization::Serializer serializer;
    serializer << a << b << c << d << e;

    hl_serialization::byte_vector rest_buffer;
    hl_serialization::Deserializer deserializer(serializer.serialized_buffer(), rest_buffer);

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
#include <hl/silva/collections/Serialization>
#include <cassert>

int main()
{
    using hl_serialization = hl::silva::collections::serialization;

    const u8 a = 42;
    const hl::silva::byte_vector overfit = {0x01, 0x02, 0x03, 0x04, 0x05};

    hl_serialization::Serializer serializer;
    serializer << a;

    hl_serialization::byte_vector serialized_buffer = serializer.serialized_buffer();
    buffer.insert(buffer.end(), overfit.begin(), overfit.end()); // Simulate overfit

    hl_serialization::byte_vector rest_buffer;

    hl_serialization::Deserializer deserializer(serialized_buffer, rest_buffer);

    assert(rest_buffer == overfit);

    u8 b;
    deserializer >> b;

    assert(a == b);
}
```

## ThreadList

Provides a `ThreadList` class that manages a list of threads that runs asynchronously.

### Regular mode

```cpp
#include <hl/silva/collections/Threads>
#include <iostream>
#include <chrono>
#include <string>

void log_atomic(const std::string& message)
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    std::cout << message << std::endl;
}

int main()
{
    // Can run SILVA_THREAD_LIST_DEFAULT_SIZE tasks at the same time.
    hl::silva::collections::threads::BasicService service;
    // You can specify
    // hl::silva::ThreadList thread_list(10); // Will run 10 tasks at the same time.


    for (int i = 0; i < 100; i++) {
        service.start([i]() {
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
#include <hl/silva/collections/Threads>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static unsigned char** load_square_images_of_same_size(
    unsigned int image_count, unsigned int &width,
    unsigned int &height, unsigned int &channels,
    unsigned int square_size)
{
    const unsigned char** image = new unsigned char*[image_count]; // prepare to load n images.

    for (unsigned int i = 0; i < image_count; i++)
    {
        const std::string file_name = "image" + std::to_string(i) + ".png";
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
    const float rotation = 191.513413; // degrees to rotate the image.

    unsigned char **images = load_square_images_of_same_size(image_count, width, height, channels, square_size);
    if (image == nullptr)
    {
        return 1;
    }

    using hl_threads = hl::silva::collections::threads;

    hl_threads::GPUSim service;

    unsigned char **images_out = new unsigned char*[image_count];
    for (unsigned int i = 0; i < image_count; i++)
    {
        images_out[i] = new u8[width * height * channels];
    }

    // Divide the image into 32 x 32 squares and run the rotation in parallel.
    // Ensure that the image is divisible by 32.
    // We will not handle the case where the image is not divisible by 32.

    const unsigned int square_count = (width / square_size) * (height / square_size);

    thread_list.start(
        [](hl_threads::GPUSim::ThreadIndex& thread_index,
        const unsigned char **images,  unsigned char **images_out, 
        const unsigned int width, const unsigned int height,
        const unsigned int channels, const unsigned int square_size,
        const float rotation)
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
        }, ThreadIndex(square_count, square_count, image_count),
    images, images_out, width, height, channels, square_size, rotation);

    thread_list.join();

    for (unsigned int i = 0; i < image_count; i++)
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
