/**
 * L1 Test Suite - Main Entry Point
 * Google Test main runner for Westeros GL BRCM L1 tests
 */

#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char **argv) {
    std::cout << "========================================" << std::endl;
    std::cout << "Westeros GL BRCM L1 Test Suite" << std::endl;
    std::cout << "Target Coverage: 75%+" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    
    int result = RUN_ALL_TESTS();
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Test execution completed" << std::endl;
    std::cout << "Result: " << (result == 0 ? "SUCCESS" : "FAILURE") << std::endl;
    std::cout << "========================================" << std::endl;
    
    return result;
}
