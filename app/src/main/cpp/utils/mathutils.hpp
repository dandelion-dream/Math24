
/*
miscellaneous math utilities used in the program.
*/

#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <cstdint>
#include <string>
#include <bit>
#include <regex>
#include <vector>

typedef enum {
    d_ZERO,
    d_SUBNORMAL,
    d_NORMAL,
    d_INFINITE,
    d_NAN,
    f_ZERO,
    f_SUBNORMAL,
    f_NORMAL,
    f_INFINITE,
    f_NAN
} fp_class;

const std::regex int_regex(R"(^[-+]?\d+$)"); //regex for searching signed integers in strings.;

bool double_is_negative(double d);

uint64_t double_exp(double d);

uint64_t double_real_exp(double d);

uint64_t double_mantissa(double d);

fp_class double_classify(double d);

bool float_is_negative(float f);

uint32_t float_exp(float f);

int32_t float_real_exp(float f);

uint32_t float_mantissa(float f);

fp_class float_classify(float f);

uint64_t comb_number64(uint64_t n, uint64_t k);

__int128_t gcd(__int128_t a, __int128_t b);

#endif