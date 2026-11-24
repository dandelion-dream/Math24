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

using std::string;
using std::vector;

const string ops[] = {"+", "-", "×", "÷"};

class node {
public:
    node();

    node(uint64_t init_mask, const string &number, int8_t base = 10);

    fraction value;
    int64_t left;  //index instead of pointer.
    int64_t right;
    uint64_t mask;
    int complexity = 1;
    uint8_t op{};

    string toString(vector<node> *input) const;
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

    [[nodiscard]] string getQuestion() const;

    [[nodiscard]] string getAnswer() const;
};

class aecAnswer {
public:
    aecAnswer(string question, string answer);

    string question;
    string answer;
};

class TooManyNumbersException : public std::exception {
public:
    string errorMessage = "Too Many Input Number (>63)";

    [[nodiscard]] const char *what() const noexcept override {
        return errorMessage.c_str();
    }
};

string AEC_solve(vector<string> *number_array, const string &target);

aecAnswer AEC_generate(const string &boundary, const string &sz, const string &target);

void iterate(vector<node> *input_array, int target_complexity);

void node_add(node *a, node *b, node *out, int64_t left, int64_t right);

void node_sub(node *a, node *b, node *out, int64_t left, int64_t right);

void node_mul(node *a, node *b, node *out, int64_t left, int64_t right);

void node_div(node *a, node *b, node *out, int64_t left, int64_t right);

#endif