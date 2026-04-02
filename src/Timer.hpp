#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <iostream>
#include <limits>

class TimerStats {
public:
    void addSample(double seconds) {
        ++count;
        total += seconds;
        totalSquared += seconds * seconds;
        if (seconds < min) min = seconds;
        if (seconds > max) max = seconds;
    }

    size_t getCount() const { return count; }
    double mean() const {
        return count ? total / count : 0.0;
    }
    double variance() const {
        if (count < 2) return 0.0;
        double m = mean();
        return (totalSquared / count) - (m * m);
    }
    double stddev() const {
        return std::sqrt(variance());
    }
    void print(std::ostream& out = std::cout) const {
        if (!name.empty()) {
            out <<  name <<  "\n"; 
        }
        out << "Count: " <<  count << "\n";
        out << "Mean: " <<  mean() << "\n";
        out << "StdDev: " <<  stddev() << "\n";
        out << "Min: " <<  min << "\n";
        out << "Max: " <<  max << "\n";
        out << "Total: " <<  total << "\n";
    }
private:
    std::string name;
    size_t count = 0;
    double total = 0.0;
    double totalSquared = 0.0;
    double min = std::numeric_limits<double>::max();
    double max = 0.0;
};

template<typename F>
auto timeFunction(TimerStats& ts, F&& func) {
    using ReturnType = std::invoke_result_t<F>;
    auto start = std::chrono::steady_clock::now();
    if constexpr (std::is_void_v<ReturnType>) {
        std::forward<F>(func)();
        auto end = std::chrono::steady_clock::now();
        ts.addSample(std::chrono::duration<double>(end - start).count());
        return;
    } else {
        auto result = std::forward<F>(func)();
        auto end = std::chrono::steady_clock::now();
        ts.addSample(std::chrono::duration<double>(end - start).count());
        return result;
    }
}       

#endif // TIMER_HPP
