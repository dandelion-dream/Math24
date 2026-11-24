#ifndef FRACTION_H
#define FRACTION_H

#include <cstdint>
#include <string>
#include <regex>
#include <exception>
#include "mathutils.hpp"

using std::string;
using std::regex;
using std::exception;

const regex fraction_regex1(R"(^[+-]?[A-Za-z0-9]+/[+-]?[A-Za-z0-9]+$)");
const regex fraction_regex2(R"(\(^[+-]?[A-Za-z0-9]+/[+-]?[A-Za-z0-9]+$\))");

class fraction {
public:
    fraction();

    fraction(int64_t numerator);

    fraction(int64_t numerator, int64_t denominator);

    fraction(double value);

    fraction(float value);

    fraction(const string &value, int8_t base = 10);

    void reduce();

    void flip();

    [[nodiscard]] double toDouble() const;

    [[nodiscard]] string toString() const;

    int64_t numerator;
    int64_t denominator;
};

class StringFormatException : public std::exception {
public:
    string errorMessage = "Incorrect Fraction String Format";

    [[nodiscard]] const char *what() const noexcept override {
        return errorMessage.c_str();
    }
};

class ValueOutOfBoundException : public std::exception {
public:
    string errorMessage = "Numerator/Denominator Out Of The Bound Of int64_t";

    [[nodiscard]] const char *what() const noexcept override {
        return errorMessage.c_str();
    }
};

class ZeroDenominatorException : public std::exception {
public:
    string errorMessage = "Denominator is zero";

    [[nodiscard]] const char *what() const noexcept override {
        return errorMessage.c_str();
    }
};

class DividedByZeroException : public std::exception {
public:
    string errorMessage = "Divided by zero";

    [[nodiscard]] const char *what() const noexcept override {
        return errorMessage.c_str();
    }
};

class FpConvertException : public std::exception {
public:
    FpConvertException(fp_class dc);

    string errorMessage;

    [[nodiscard]] const char *what() const noexcept override {
        return errorMessage.c_str();
    }
};

void add(fraction *a, fraction *b, fraction *out);  //快速运算函数，指针读取，指针写入，全程0拷贝。

void sub(fraction *a, fraction *b, fraction *out);

void mul(fraction *a, fraction *b, fraction *out);

void div(fraction *a, fraction *b, fraction *out);  //同时牺牲一点性能支持运算符重载。

int8_t compare(fraction *a, fraction *b);

#endif