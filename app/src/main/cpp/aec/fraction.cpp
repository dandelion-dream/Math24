#include "fraction.hpp"
#include "mathutils.hpp"
#include <string>
#include <regex>
#include <cstdint>

using std::string;
using std::stoll;
using std::to_string;
using std::regex_match;
using std::swap;

fraction::fraction() {
    this->numerator = 0;
    this->denominator = 1;
}

fraction::fraction(int64_t numerator) {
    this->numerator = numerator;
    this->denominator = 1;
}

fraction::fraction(int64_t numerator, int64_t denominator) {
    if (denominator == 0) {
        throw ZeroDenominatorException();
    }
    this->numerator = numerator;
    this->denominator = denominator;
    this->reduce();
}

fraction::fraction(double value) {
    fp_class fc = double_classify(value);
    bool is_negative = double_is_negative(value);
    int32_t exp = static_cast<int32_t>(double_real_exp(value)) - 52;
    uint64_t mantissa = double_mantissa(value);
    switch (fc) {
        case d_NORMAL: {
            auto tmp_numerator = static_cast<int64_t>(mantissa | 0x10000000000000ULL);
            uint32_t zeros = __builtin_ctzll(tmp_numerator);
            exp += static_cast<int32_t>(zeros);
            tmp_numerator >>= zeros;
            if (is_negative && exp == 63 && tmp_numerator == 1) {
                this->numerator = INT64_MIN;
                this->denominator = 1;
                return;
            } else if (exp >= 0 && exp < (int32_t)(11 + zeros)) {
                tmp_numerator <<= exp;
                this->numerator = tmp_numerator;
                this->denominator = 1;
            } else if (exp < 0) {
                exp = -exp;
                if (exp > 62) {
                    throw ValueOutOfBoundException();
                }
                this->numerator = tmp_numerator;
                this->denominator = 1LL << exp;
                this->reduce();
            } else {
                throw ValueOutOfBoundException();
            }
        }
            if (is_negative) {
                this->numerator = -this->numerator;
            }
            break;
        case d_ZERO:
            this->numerator = 0;
            this->denominator = 1;
            break;

        default:
            throw FpConvertException(fc);
            break;
    }
}

fraction::fraction(float value) {
    fp_class fc = float_classify(value);
    bool is_negative = float_is_negative(value);
    int32_t exp = float_real_exp(value) - 23;
    uint32_t mantissa = float_mantissa(value);
    switch (fc) {
        case f_NORMAL: {
            auto tmp_numerator = static_cast<int64_t>(mantissa | 0x800000UL);
            uint32_t zeros = __builtin_ctzll(tmp_numerator);
            exp += static_cast<int32_t>(zeros);
            tmp_numerator >>= zeros;
            if (is_negative && exp == 63 && tmp_numerator == 1) {
                this->numerator = INT64_MIN;
                this->denominator = 1;
                return;
            } else if (exp >= 0 && exp < (int32_t)(40 + zeros)) {
                tmp_numerator <<= exp;
                this->numerator = tmp_numerator;
                this->denominator = 1;
            } else if (exp < 0) {
                exp = -exp;
                if (exp > 62) {
                    throw ValueOutOfBoundException();
                }
                this->numerator = tmp_numerator;
                this->denominator = 1LL << exp;
                this->reduce();
            } else {
                throw ValueOutOfBoundException();
            }
            if (is_negative) {
                this->numerator = -this->numerator;
            }
        }
            break;
        case f_ZERO:
            this->numerator = 0;
            this->denominator = 1;
            break;

        default:
            throw FpConvertException(fc);
            break;
    }
}

FpConvertException::FpConvertException(fp_class fc) {
    switch (fc) {
        case d_INFINITE:
            this->errorMessage = "Invalid Double Input (infinite)";
            break;
        case d_NAN:
            this->errorMessage = "Invalid Double Input (NaN)";
            break;
        case d_SUBNORMAL:
            this->errorMessage = "Invalid Double Input (too small subnormal number)";
            break;
        case f_INFINITE:
            this->errorMessage = "Invalid Float Input (infinite)";
            break;
        case f_NAN:
            this->errorMessage = "Invalid Float Input (NaN)";
            break;
        case f_SUBNORMAL:
            this->errorMessage = "Invalid Float Input (too small subnormal number)";
            break;
        default:
            this->errorMessage = "";
            break;
    }
}

fraction::fraction(const string &value, int8_t base) {
    bool int_match = regex_match(value, int_regex);
    bool fraction_match1 = regex_match(value, fraction_regex1);
    bool fraction_match2 = regex_match(value, fraction_regex2);
    if (int_match) {
        this->numerator = stoll(value, nullptr, base);
        this->denominator = 1;
    } else if (fraction_match1) {
        size_t index; //store the index of '/'.
        this->numerator = stoll(value, &index, base);
        this->denominator = stoll(value.substr(index + 1), nullptr, base); //skip the '/'.
        if (this->denominator == 0) {
            throw ZeroDenominatorException();
        }
    } else if (fraction_match2) {
        size_t index;
        this->numerator = stoll(value.substr(1), &index, base);
        this->denominator = stoll(value.substr(index + 1), nullptr, base); //skip the '/'.
    } else {
        throw StringFormatException();
    }
    if (this->denominator == 0) {
        throw ZeroDenominatorException();
    }
    this->reduce();
}

void add(const fraction &a, const fraction &b, fraction &out) {
    __int128_t a_numerator = a.numerator;
    __int128_t b_numerator = b.numerator;
    __int128_t a_denominator = a.denominator;
    __int128_t b_denominator = b.denominator;
    if (a_denominator == 0 || b_denominator == 0) {
        throw ZeroDenominatorException();
    }
    __int128_t tmp_denominator = a_denominator * b_denominator;
    __int128_t tmp_numerator = a_numerator * b_denominator + b_numerator * a_denominator;
    __int128_t tmp_gcd = gcd(tmp_numerator, tmp_denominator);
    tmp_numerator /= tmp_gcd;
    tmp_denominator /= tmp_gcd;
    if (tmp_numerator > INT64_MAX || tmp_numerator < INT64_MIN || tmp_denominator > INT64_MAX ||
        tmp_denominator < INT64_MIN) {
        throw ValueOutOfBoundException();
    } else {
        if (tmp_denominator < 0) {
            tmp_numerator = -tmp_numerator;
            tmp_denominator = -tmp_denominator;
        }
        out.numerator = static_cast<int64_t>(tmp_numerator);
        out.denominator = static_cast<int64_t>(tmp_denominator);
    }
}

void sub(const fraction &a, const fraction &b, fraction &out) {
    __int128_t a_numerator = a.numerator;
    __int128_t b_numerator = b.numerator;
    __int128_t a_denominator = a.denominator;
    __int128_t b_denominator = b.denominator;
    if (a_denominator == 0 || b_denominator == 0) {
        throw ZeroDenominatorException();
    }
    __int128_t tmp_denominator = a_denominator * b_denominator;
    __int128_t tmp_numerator =
            a_numerator * b_denominator - b_numerator * a_denominator; //+换成-，复制粘贴
    __int128_t tmp_gcd = gcd(tmp_numerator, tmp_denominator);
    tmp_numerator /= tmp_gcd;
    tmp_denominator /= tmp_gcd;
    if (tmp_numerator > INT64_MAX || tmp_numerator < INT64_MIN || tmp_denominator > INT64_MAX ||
        tmp_denominator < INT64_MIN) {
        throw ValueOutOfBoundException();
    } else {
        if (tmp_denominator < 0) {
            tmp_numerator = -tmp_numerator;
            tmp_denominator = -tmp_denominator;
        }
        out.numerator = static_cast<int64_t>(tmp_numerator);
        out.denominator = static_cast<int64_t>(tmp_denominator);
    }
}

void mul(const fraction &a, const fraction &b, fraction &out) {
    __int128_t a_numerator = a.numerator;
    __int128_t b_numerator = b.numerator;
    __int128_t a_denominator = a.denominator;
    __int128_t b_denominator = b.denominator;
    if (a_denominator == 0 || b_denominator == 0) {
        throw ZeroDenominatorException();
    }
    __int128_t tmp_numerator = a_numerator * b_numerator;
    __int128_t tmp_denominator = a_denominator * b_denominator;
    __int128_t tmp_gcd = gcd(tmp_numerator, tmp_denominator);
    tmp_numerator /= tmp_gcd;
    tmp_denominator /= tmp_gcd;
    if (tmp_numerator > INT64_MAX || tmp_numerator < INT64_MIN || tmp_denominator > INT64_MAX ||
        tmp_denominator < INT64_MIN) {
        throw ValueOutOfBoundException();
    } else {
        if (tmp_denominator < 0) {
            tmp_numerator = -tmp_numerator;
            tmp_denominator = -tmp_denominator;
        }
        out.numerator = static_cast<int64_t>(tmp_numerator);
        out.denominator = static_cast<int64_t>(tmp_denominator);
    }
}

void div(const fraction &a, const fraction &b, fraction &out) {
    __int128_t a_numerator = a.numerator;
    __int128_t b_numerator = b.numerator;
    __int128_t a_denominator = a.denominator;
    __int128_t b_denominator = b.denominator;
    if (a_denominator == 0 || b_denominator == 0) {
        throw ZeroDenominatorException();
    }
    if (b_numerator == 0) {
        throw DividedByZeroException();
    }
    __int128_t tmp_numerator = a_numerator * b_denominator;
    __int128_t tmp_denominator = a_denominator * b_numerator;
    __int128_t tmp_gcd = gcd(tmp_numerator, tmp_denominator);
    tmp_numerator /= tmp_gcd;
    tmp_denominator /= tmp_gcd;
    if (tmp_numerator > INT64_MAX || tmp_numerator < INT64_MIN || tmp_denominator > INT64_MAX ||
        tmp_denominator < INT64_MIN) {
        throw ValueOutOfBoundException();
    } else {
        if (tmp_denominator < 0) {
            tmp_numerator = -tmp_numerator;
            tmp_denominator = -tmp_denominator;
        }
        out.numerator = static_cast<int64_t>(tmp_numerator);
        out.denominator = static_cast<int64_t>(tmp_denominator);
    }
}

int8_t compare(const fraction &a, const fraction &b) {
    __int128_t a_numerator = a.numerator;
    __int128_t b_numerator = b.numerator;
    __int128_t a_denominator = a.denominator;
    __int128_t b_denominator = b.denominator;
    if (a_denominator < 0) {
        a_numerator = -a_numerator;
        a_denominator = -a_denominator;
    }
    if (b_denominator < 0) {
        b_numerator = -b_numerator;
        b_denominator = -b_denominator;
    }
    __int128_t tmp_a = a_numerator * b_denominator;
    __int128_t tmp_b = b_numerator * a_denominator;
    if (tmp_a == tmp_b) {
        return 0;
    } else if (tmp_a > tmp_b) {
        return 1;
    } else {
        return -1;
    }
}

void fraction::flip() {
    if (this->numerator == 0) {
        throw ZeroDenominatorException();
    }
    swap(this->numerator, this->denominator);
    if (this->denominator < 0) {
        this->numerator = -this->numerator;
        this->denominator = -this->denominator;
    }
}

void fraction::reduce() {
    auto tmp_gcd = static_cast<int64_t>(gcd(this->numerator, this->denominator));
    this->numerator /= tmp_gcd;
    this->denominator /= tmp_gcd;
}

double fraction::toDouble() const {
    return (double) this->numerator / (double) this->denominator;
}

string fraction::toString() const {
    if (this->denominator == 1) {
        return to_string(this->numerator);
    } else {
        return '('
               + to_string(this->numerator)
               + '/'
               + to_string(this->denominator)
               + ')';
    }
}