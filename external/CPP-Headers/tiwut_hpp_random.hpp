#ifndef TIWUT_HPP_RANDOM_GENERATOR_HPP
#define TIWUT_HPP_RANDOM_GENERATOR_HPP

/*
 * README: tiwut_hpp_random -std=c++17
 * ------------------------
 * 
 * Features:
 * - Thread-local engine: High performance, thread-safe, no locking overhead.
 * - tiwut_hpp_random_uuid_v4(): True UUID v4 generation.
 * - tiwut_hpp_random_gaussian(): Normal/Gaussian distribution for data science.
 * - tiwut_hpp_random_weighted_choice(): Pick items based on probability weights.
 * - tiwut_hpp_random_bytes(): Generates raw byte buffers for crypto/hashing.
 * - Comprehensive int/float/string randomizers.
 */

#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>

// Thread-Local Engine
// Thread_local guarantees that every thread gets its own PRNG instance,
// preventing race conditions without needing slow std::mutex locks.
inline std::mt19937_64& tiwut_hpp_get_random_engine() {
    thread_local std::random_device rd;
    thread_local std::mt19937_64 engine(rd());
    return engine;
}

// Basic Primitives
inline int64_t tiwut_hpp_random_int(int64_t min, int64_t max) {
    std::uniform_int_distribution<int64_t> dist(min, max);
    return dist(tiwut_hpp_get_random_engine());
}

inline double tiwut_hpp_random_double(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(tiwut_hpp_get_random_engine());
}

inline bool tiwut_hpp_random_bool(double probability_true = 0.5) {
    std::bernoulli_distribution dist(probability_true);
    return dist(tiwut_hpp_get_random_engine());
}

// Strings & Bytes
inline std::string tiwut_hpp_random_string(size_t length, const std::string& charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz") {
    std::string result;
    result.resize(length);
    std::uniform_int_distribution<size_t> dist(0, charset.size() - 1);
    auto& engine = tiwut_hpp_get_random_engine();
    for (size_t i = 0; i < length; ++i) {
        result[i] = charset[dist(engine)];
    }
    return result;
}

inline std::vector<uint8_t> tiwut_hpp_random_bytes(size_t size) {
    std::vector<uint8_t> buffer(size);
    std::uniform_int_distribution<int> dist(0, 255);
    auto& engine = tiwut_hpp_get_random_engine();
    for (size_t i = 0; i < size; ++i) {
        buffer[i] = static_cast<uint8_t>(dist(engine));
    }
    return buffer;
}

// Advanced Generation
inline std::string tiwut_hpp_random_uuid_v4() {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    auto& engine = tiwut_hpp_get_random_engine();
    std::uniform_int_distribution<uint16_t> dist(0, 0xFFFF);

    ss << std::setw(4) << dist(engine) << std::setw(4) << dist(engine) << "-";
    ss << std::setw(4) << dist(engine) << "-";
    ss << std::setw(4) << ((dist(engine) & 0x0FFF) | 0x4000) << "-"; // UUID v4 specific (4xxx)
    ss << std::setw(4) << ((dist(engine) & 0x3FFF) | 0x8000) << "-"; // UUID v4 specific (8,9,A,B)
    ss << std::setw(4) << dist(engine) << std::setw(4) << dist(engine) << std::setw(4) << dist(engine);
    
    return ss.str();
}

inline double tiwut_hpp_random_gaussian(double mean, double std_dev) {
    std::normal_distribution<double> dist(mean, std_dev);
    return dist(tiwut_hpp_get_random_engine());
}

template <typename T>
T tiwut_hpp_random_weighted_choice(const std::vector<T>& items, const std::vector<double>& weights) {
    if (items.empty() || items.size() != weights.size()) throw std::invalid_argument("tiwut_hpp: Invalid arrays for weighted choice");
    std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
    return items[dist(tiwut_hpp_get_random_engine())];
}

template <typename Container>
void tiwut_hpp_random_shuffle(Container& container) {
    std::shuffle(std::begin(container), std::end(container), tiwut_hpp_get_random_engine());
}

#endif // TIWUT_HPP_RANDOM_GENERATOR_HPP
