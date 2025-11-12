#ifndef __SIMPLE_TEST_H__
#define __SIMPLE_TEST_H__

#include <iostream>
#include <string>
#include <vector>
#include <functional>

// Simple testing framework for when Google Test is not available
class SimpleTest {
public:
    struct TestCase {
        std::string name;
        std::function<void()> test_func;
    };
    
    static std::vector<TestCase>& getTests() {
        static std::vector<TestCase> tests;
        return tests;
    }
    
    static void addTest(const std::string& name, std::function<void()> func) {
        getTests().push_back({name, func});
    }
    
    static int runAllTests() {
        int passed = 0;
        int failed = 0;
        
        std::cout << "Running " << getTests().size() << " tests..." << std::endl;
        std::cout << "========================================" << std::endl;
        
        for (const auto& test : getTests()) {
            std::cout << "[ RUN      ] " << test.name << std::endl;
            try {
                test.test_func();
                std::cout << "[       OK ] " << test.name << std::endl;
                passed++;
            } catch (const std::exception& e) {
                std::cout << "[  FAILED  ] " << test.name << std::endl;
                std::cout << "  Error: " << e.what() << std::endl;
                failed++;
            }
        }
        
        std::cout << "========================================" << std::endl;
        std::cout << "Tests passed: " << passed << "/" << (passed + failed) << std::endl;
        
        if (failed > 0) {
            std::cout << "Tests FAILED: " << failed << " test(s) failed" << std::endl;
            return 1;
        } else {
            std::cout << "All tests PASSED" << std::endl;
            return 0;
        }
    }
};

class TestFailure : public std::exception {
    std::string message;
public:
    TestFailure(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

#define TEST(suite, name) \
    void suite##_##name(); \
    struct suite##_##name##_registrar { \
        suite##_##name##_registrar() { \
            SimpleTest::addTest(#suite "." #name, suite##_##name); \
        } \
    }; \
    static suite##_##name##_registrar suite##_##name##_registrar_instance; \
    void suite##_##name()

template<typename T>
std::string toString(const T& val) {
    return std::to_string(val);
}

inline std::string toString(const std::string& val) {
    return val;
}

inline std::string toString(const char* val) {
    return std::string(val);
}

#define EXPECT_EQ(a, b) \
    do { \
        auto val_a = (a); \
        auto val_b = (b); \
        if (val_a != val_b) { \
            throw TestFailure(std::string("Expected ") + #a + " == " + #b + \
                            " but got " + toString(val_a) + " != " + toString(val_b)); \
        } \
    } while(0)

#define EXPECT_NE(a, b) \
    do { \
        auto val_a = (a); \
        auto val_b = (b); \
        if (val_a == val_b) { \
            throw TestFailure(std::string("Expected ") + #a + " != " + #b + \
                            " but got " + toString(val_a) + " == " + toString(val_b)); \
        } \
    } while(0)

#define EXPECT_TRUE(a) \
    do { \
        if (!(a)) { \
            throw TestFailure(std::string("Expected ") + #a + " to be true"); \
        } \
    } while(0)

#define EXPECT_FALSE(a) \
    do { \
        if (a) { \
            throw TestFailure(std::string("Expected ") + #a + " to be false"); \
        } \
    } while(0)

#define EXPECT_GT(a, b) \
    do { \
        auto val_a = (a); \
        auto val_b = (b); \
        if (!(val_a > val_b)) { \
            throw TestFailure(std::string("Expected ") + #a + " > " + #b); \
        } \
    } while(0)

#define EXPECT_GE(a, b) \
    do { \
        auto val_a = (a); \
        auto val_b = (b); \
        if (!(val_a >= val_b)) { \
            throw TestFailure(std::string("Expected ") + #a + " >= " + #b); \
        } \
    } while(0)

#define ASSERT_EQ(a, b) EXPECT_EQ(a, b)
#define ASSERT_NE(a, b) EXPECT_NE(a, b)
#define ASSERT_TRUE(a) EXPECT_TRUE(a)
#define ASSERT_FALSE(a) EXPECT_FALSE(a)

#define SUCCEED() do {} while(0)

#endif
