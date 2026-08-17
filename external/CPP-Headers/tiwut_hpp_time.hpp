#ifndef TIWUT_HPP_TIME_MANAGER_HPP
#define TIWUT_HPP_TIME_MANAGER_HPP

/*
 * README: tiwut_hpp_time -std=c++17
 * ----------------------
 * 
 * Features:
 * - tiwut_hpp_delay_ms/us/ns(): Precision blocking delays.
 * - tiwut_hpp_async_timeout(): Non-blocking delay with callback.
 * - tiwut_hpp_IntervalTimer: Background thread for repetitive cron-like tasks.
 * - tiwut_hpp_Stopwatch: Pause/Resume, Lap times, high-resolution tracking.
 * - tiwut_hpp_measure_execution(): Template wrapper to benchmark ANY function.
 * - tiwut_hpp_get_utc_timestamp() / tiwut_hpp_get_local_timestamp(): Precision strings.
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <future>
#include <atomic>
#include <functional>
#include <type_traits>
#include <utility>

// Precision Delays
inline void tiwut_hpp_delay_ms(uint64_t ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }
inline void tiwut_hpp_delay_us(uint64_t us) { std::this_thread::sleep_for(std::chrono::microseconds(us)); }
inline void tiwut_hpp_delay_ns(uint64_t ns) { std::this_thread::sleep_for(std::chrono::nanoseconds(ns)); }

// Advanced Timestamps
inline std::string tiwut_hpp_get_local_timestamp(const std::string& format = "%Y-%m-%d %H:%M:%S") {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), format.c_str());
    return ss.str();
}

inline std::string tiwut_hpp_get_utc_timestamp(const std::string& format = "%Y-%m-%dT%H:%M:%SZ") {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t_now), format.c_str());
    return ss.str();
}

// Asynchronous Execution & Timers
inline void tiwut_hpp_async_timeout(uint64_t ms, std::function<void()> callback) {
    std::thread([ms, callback]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        callback();
    }).detach();
}

class tiwut_hpp_IntervalTimer {
private:
    std::atomic<bool> active{false};
    std::thread worker;
public:
    ~tiwut_hpp_IntervalTimer() { stop(); }
    void start(uint64_t interval_ms, std::function<void()> task) {
        if (active) return;
        active = true;
        worker = std::thread([this, interval_ms, task]() {
            while (active) {
                std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
                if (active) task();
            }
        });
    }
    void stop() {
        active = false;
        if (worker.joinable()) worker.join();
    }
};

// Advanced Benchmarking
class tiwut_hpp_Stopwatch {
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    TimePoint start_time;
    TimePoint pause_time;
    bool running = false;
    std::vector<uint64_t> laps;
public:
    void start() { start_time = Clock::now(); running = true; laps.clear(); }
    void pause() { if(running) { pause_time = Clock::now(); running = false; } }
    void resume() { if(!running) { start_time += Clock::now() - pause_time; running = true; } }
    
    void lap() {
        if (running) {
            auto now = Clock::now();
            laps.push_back(std::chrono::duration_cast<std::chrono::microseconds>(now - start_time).count());
        }
    }
    
    uint64_t get_elapsed_us() const {
        TimePoint end_time = running ? Clock::now() : pause_time;
        return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    }
    
    std::vector<uint64_t> get_laps() const { return laps; }
};

// Template Metaprogramming: Measure execution time of ANY function
template <typename F, typename... Args>
auto tiwut_hpp_measure_execution(F&& f, Args&&... args) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Using decltype is more universally supported across compilers than std::invoke_result
    using ReturnType = decltype(std::forward<F>(f)(std::forward<Args>(args)...));
    
    if constexpr (std::is_same<ReturnType, void>::value) {
        std::forward<F>(f)(std::forward<Args>(args)...);
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    } else {
        auto result = std::forward<F>(f)(std::forward<Args>(args)...);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        return std::make_pair(result, duration);
    }
}

#endif // TIWUT_HPP_TIME_MANAGER_HPP
