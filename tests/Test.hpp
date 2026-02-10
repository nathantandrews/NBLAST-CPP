#ifndef TEST_HPP
#define TEST_HPP

#include <iostream>
#include <stdexcept>
#include <cstdlib>

inline int tests_run = 0;
inline int tests_failed = 0;

#define TEST_CASE(name) \
    void name(); \
    struct name##_registrar { \
        name##_registrar() { \
            ++tests_run; \
            try { \
                name(); \
                std::cout << "[PASSED] " #name "\n"; \
            } catch (const std::exception& e) { \
                std::cerr << "[FAILED] " #name << ": " << e.what() << "\n"; \
                tests_failed++; \
            } catch (...) { \
                std::cerr << "[FAILED] " #name << ": unknown exception\n"; \
                tests_failed++; \
            } \
        } \
    }; \
    static name##_registrar name##_instance; \
    void name()

#define REQUIRE(cond) \
    do { \
        if (!(cond)) { \
            std::cerr << "[FAILED] " << __FILE__ << ":" << __LINE__ \
                      << " -- (" << #cond << ")\n"; \
            throw std::runtime_error("Test failed: " #cond); \
        } \
    } while (0)

#define REQUIRE_EQ(a, b) REQUIRE((a) == (b))

#endif // TEST_HPP
