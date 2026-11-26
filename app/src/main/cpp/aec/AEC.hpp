#ifndef AEC_H
#define AEC_H

#include <cstdint>
#include <string>
#include <vector>
#include <random>
#include "fraction.hpp"

#define ADD 0
#define SUB 1
#define MUL 2
#define DIV 3

const std::string ops[] = {"+", "-", "×", "÷"};

class node {
public:
    node();

    node(const std::string &number, int8_t base = 10);

    fraction value;
    int64_t left;  //index instead of pointer.
    int64_t right;
    uint8_t op{};

    [[nodiscard]] std::string toString(const std::vector<node> &input) const;
};

class node_bucket{
public:
    node_bucket();
    node_bucket(std::vector<int64_t> elements, uint64_t mask, uint8_t complexity, size_t zero_count);

    std::vector<int64_t> elements;
    size_t zero_count;
    uint64_t mask;
    uint8_t complexity;
};

class node_g {
public:
    node_g();

    node_g(fraction f, size_t n);

    fraction value;
    node_g *left;
    node_g *right;
    size_t depth;
    uint8_t op;

    void recurse_gen(std::mt19937_64 rnd,
                     std::uniform_int_distribution<int64_t> intDist,
                     std::uniform_int_distribution<uint8_t> opDist);

    [[nodiscard]] std::string getQuestion() const;

    [[nodiscard]] std::string getAnswer() const;
};

class aecAnswer {
public:
    aecAnswer(std::string question, std::string answer);

    std::string question;
    std::string answer;
};

class TooManyNumbersException : public std::exception {
public:
    std::string errorMessage = "Too Many Input Number (>63)";

    [[nodiscard]] const char *what() const noexcept override {
        return errorMessage.c_str();
    }
};

std::string AEC_solve(const std::vector<std::string> &number_array, const std::string &target);

aecAnswer AEC_generate(const std::string &boundary, const std::string &sz, const std::string &target);

void bucketsCombineNormal(std::vector<node> &node_pool, const node_bucket &a, const node_bucket &b, node_bucket &out, uint64_t mask);

std::vector<node> bucketsCombineCheck(std::vector<node> &node_pool, const node_bucket &a, const node_bucket &b, const fraction &target);

std::vector<uint64_t> getPossibleMasks(uint64_t msk, size_t n, size_t target_complexity);

void fillMask(uint64_t input, size_t depth, const std::vector<uint64_t> &candidates, std::vector<uint64_t> &container);

size_t node_add(std::vector<node> &node_pool, int64_t a, int64_t b, int64_t out);

size_t node_sub(std::vector<node> &node_pool, int64_t a, int64_t b, int64_t out);

size_t node_mul(std::vector<node> &node_pool, int64_t a, int64_t b, int64_t out);

size_t node_div(std::vector<node> &node_pool, int64_t a, int64_t b, int64_t out);

#endif