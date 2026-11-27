#include <cstdint>
#include <bit>
#include <string>
#include <regex>
#include <vector>
#include "mathutils.hpp"

using std::string;
using std::bit_cast;
using std::countr_zero;

bool double_is_negative(double d) {
    auto bits = bit_cast<uint64_t>(d);
    return ((bits >> 63) & 0x1) > 0;
}

uint64_t double_exp(double d) {
    auto bits = bit_cast<uint64_t>(d);
    return (bits >> 52) & 0x7FF;
}

uint64_t double_real_exp(double d) {
    auto bits = bit_cast<uint64_t>(d);
    uint64_t exp = (bits >> 52) & 0x7FF;
    return (int64_t) exp - 1023;
}

uint64_t double_mantissa(double d) {
    auto bits = bit_cast<uint64_t>(d);
    return bits & 0xFFFFFFFFFFFFFULL;
}

fp_class double_classify(double d) {
    auto bits = bit_cast<uint64_t>(d);

    uint64_t exp = (bits >> 52) & 0x7FF;
    uint64_t mantissa = bits & 0xFFFFFFFFFFFFFULL;

    if (exp == 0x7FF) return (mantissa == 0) ? d_INFINITE : d_NAN;
    if (exp == 0) return (mantissa == 0) ? d_ZERO : d_SUBNORMAL;
    return d_NORMAL;
}

bool float_is_negative(float f) {
    auto bits = bit_cast<uint32_t>(f);
    return ((bits >> 31) & 0x1) > 0;
}

uint32_t float_exp(float f) {
    auto bits = bit_cast<uint32_t>(f);
    return (bits >> 23) & 0xFF;
}

int32_t float_real_exp(float f) {
    auto bits = bit_cast<uint32_t>(f);
    uint32_t exp = (bits >> 23) & 0xFF;
    return static_cast<int32_t>(exp) - 127;
}

uint32_t float_mantissa(float f) {
    auto bits = bit_cast<uint32_t>(f);
    return bits & 0x7FFFFF;
}

fp_class float_classify(float f) {
    auto bits = bit_cast<uint32_t>(f);

    uint32_t exp = (bits >> 23) & 0xFF;
    uint32_t mantissa = bits & 0x7FFFFF;

    if (exp == 0xFF) return (mantissa == 0) ? f_INFINITE : f_NAN;

    if (exp == 0) return (mantissa == 0) ? f_ZERO : f_SUBNORMAL;

    return f_NORMAL;
}

__int128_t gcd(__int128_t a,
               __int128_t b) { //a much more fast version of gcd(), use for reduction of fractions.
    if (a == 0 && b == 0) return 0;
    if (a == 0) return b;
    if (b == 0) return a;

    //记录符号，如果全为负，返回负；否则（一正一负或全正）返回正。
    bool is_negative = (a < 0 && b < 0);

    //转换为无符号绝对值
    __uint128_t ua = a < 0 ? -(__uint128_t) a : (__uint128_t) a;
    __uint128_t ub = b < 0 ? -(__uint128_t) b : (__uint128_t) b;

    //提取公共的 2 的幂次
    uint32_t az = countr_zero(ua);
    uint32_t bz = countr_zero(ub);
    uint32_t z = az < bz ? az : bz;

    ua >>= az;
    ub >>= bz;

    while (ua != ub) {
        if (ua > ub) {
            ua -= ub;
            ua >>= countr_zero(ua); // ua 在此处绝不等于 0
        } else {
            ub -= ua;
            ub >>= countr_zero(ub); // ub 在此处绝不等于 0
        }
    }

    //恢复公因子并赋予符号
    __uint128_t final_gcd = ua << z;
    if (is_negative) {
        return (__int128_t) (0 - final_gcd);
    }
    return (__int128_t) final_gcd;
}

uint64_t comb_number64(uint64_t n, uint64_t k){
    if(k>n){
        return 0;
    }else if(k==0 || n==k){
        return 1;
    }else{
        return (n*comb_number64(n-1, k-1))/k;
    }
}