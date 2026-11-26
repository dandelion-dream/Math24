#ifndef FRACTION_H
#define FRACTION_H

#include <cstdint>
#include <string>
#include <regex>
#include <exception>
#include "mathutils.hpp"

const std::regex fraction_regex1(R"(^[+-]?[A-Za-z0-9]+/[+-]?[A-Za-z0-9]+$)");
const std::regex fraction_regex2(R"(\(^[+-]?[A-Za-z0-9]+/[+-]?[A-Za-z0-9]+$\))");

class fraction {
public:
    fraction();

    fraction(int64_t numerator);

    fraction(int64_t numerator, int64_t denominator);

    fraction(double value);

    fraction(float value);

    fraction(const std::string &value, int8_t base = 10);

    void reduce();

    void flip();

    [[nodiscard]] double toDouble() const;

    [[nodiscard]] std::string toString() const;

    int64_t numerator;
    int64_t denominator;
};

class StringFormatException : public std::exception {
public:
    std::string errorMessage = "Incorrect Fraction String Format";

    [[nodiscard]] const char *what() const noexcept override {
        return errorMessage.c_str();
    }
};

class ValueOutOfBoundException : public std::exception {
public:
    std::string errorMessage = "Numerator/Denominator Out Of The Bound Of int64_t";

    [[nodiscard]] const char *what() const noexcept override {
        return errorMessage.c_str();
    }
};

class ZeroDenominatorException : public std::exception {
public:
    std::string errorMessage = "Denominator is zero";

    [[nodiscard]] const char *what() const noexcept override {
        return errorMessage.c_str();
    }
};

class DividedByZeroException : public std::exception {
public:
    std::string errorMessage = "Divided by zero";

    [[nodiscard]] const char *what() const noexcept override {
        return errorMessage.c_str();
    }
};

class FpConvertException : public std::exception {
public:
    FpConvertException(fp_class dc);

    std::string errorMessage;

    [[nodiscard]] const char *what() const noexcept override {
        return errorMessage.c_str();
    }
};

void add(const fraction &a, const fraction &b, fraction &out);  //快速运算函数，指针读取，指针写入，全程0拷贝。

void sub(const fraction &a, const fraction &b, fraction &out);

void mul(const fraction &a, const fraction &b, fraction &out);

void div(const fraction &a, const fraction &b, fraction &out);  //同时牺牲一点性能支持运算符重载。

int8_t compare(const fraction &a, const fraction &b);

#endif