# Tiwut HPP Utilities

A collection of **advanced, header-only C++17 utilities** designed for high-performance applications. 

These libraries provide robust implementations for precise timing, cryptographically-adjacent random generation, and safe file I/O. Everything is heavily prefixed with `tiwut_hpp_` to guarantee **zero naming collisions** with your existing codebase or other third-party libraries.

![C++17](https://img.shields.io/badge/Standard-C%2B%2B17-blue.svg)
![Header Only](https://img.shields.io/badge/Type-Header%20Only-success.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

## Modules Included

### 1. ⏱️ Time & Benchmarking (`tiwut_hpp_time.hpp`)
Advanced timing utilities leveraging `<chrono>` and `<future>`.
* **Execution Benchmarking**: Template-based wrappers (`tiwut_hpp_measure_execution`) to measure the exact execution time of ANY function.
* **Stopwatch & Laps**: High-resolution lap-tracking for performance testing.
* **Asynchronous Timeouts**: Non-blocking callbacks and background interval cron-like timers.
* **Precision Timestamps**: Local and UTC ISO-formatted string generation.

### 2. 🎲 Random Generation (`tiwut_hpp_random.hpp`)
Thread-safe, high-entropy random generation using `<random>`.
* **Lock-Free Thread Safety**: Uses `thread_local` MT19937_64 engines for max performance without mutex bottlenecks.
* **UUID v4**: True, standards-compliant UUID generation.
* **Advanced Math**: Gaussian/Normal distribution and weighted random choices for data science/gaming.
* **Byte Generation**: Raw byte buffer generation for crypto/hashing algorithms.

### 3. 📂 File & I/O Operations (`tiwut_hpp_file.hpp`)
Safe and modern file management wrapping `<filesystem>`.
* **Atomic Writes**: `tiwut_hpp_file_write_atomic` writes to temporary files first, preventing data corruption if your program crashes mid-write.
* **Async Reads**: Background file reading returning a `std::future`.
* **Recursive Directory Management**: Create, list, or violently destroy directories recursively.
* **Advanced Metadata**: Extract precise file sizes, permissions, and formatted modification dates.

---

## Installation & Requirements

Because this is a **Header-Only** library, there is no need to compile `.cpp` files, build `.a`/`.so` libraries, or link anything via CMake.

1. Drop the headers (`tiwut_hpp_time.hpp`, `tiwut_hpp_random.hpp`, `tiwut_hpp_file.hpp`) into your project's `src/` or `include/` directory.
2. Include them in your code.
3. **Important:** You must compile with **C++17** or newer.

**Via Terminal (GCC/Clang):**
```bash
g++ -std=c++17 src/main.cpp -o my_app
```

**Via CMake (`CMakeLists.txt`):**
```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
```

---

## Quick Start Example

```cpp
#include "tiwut_hpp_time.hpp"
#include "tiwut_hpp_random.hpp"
#include "tiwut_hpp_file.hpp"
#include <iostream>

// A dummy function to benchmark
void heavy_calculation() {
    tiwut_hpp_delay_ms(150); // Simulate 150ms of hard work
}

int main() {
    std::cout << "Starting at: " << tiwut_hpp_get_local_timestamp() << "\n\n";

    // --- 1. Measure Execution Time ---
    auto duration_us = tiwut_hpp_measure_execution(heavy_calculation);
    std::cout << "[Timer] Calculation took: " << (duration_us / 1000.0) << " ms\n";

    // --- 2. Generate Random Data ---
    std::string transaction_id = tiwut_hpp_random_uuid_v4();
    double sensor_val = tiwut_hpp_random_gaussian(50.0, 5.0); // Mean 50, StdDev 5
    
    std::cout << "[Random] Generated UUID: " << transaction_id << "\n";
    std::cout << "[Random] Sensor Value: " << sensor_val << "\n";

    // --- 3. Safe Atomic File Writing ---
    std::string db_file = "database.dat";
    if (tiwut_hpp_file_write_atomic(db_file, transaction_id)) {
        std::cout << "[File] Atomic write successful. Protected against crash corruption!\n";
    }

    // --- 4. File Metadata Extraction ---
    if (auto meta = tiwut_hpp_file_get_metadata(db_file)) {
        std::cout << "[File] " << db_file << " is " << meta->size_bytes 
                  << " bytes, last modified at " << meta->last_write_time << "\n";
    }

    return 0;
}
```

## 🛡️ License
This project is open-source and distributed under the MIT License. Feel free to use it in personal, educational, or commercial projects.
