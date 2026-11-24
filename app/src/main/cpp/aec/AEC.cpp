#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include <random>
#include "../globals.hpp"
#include "fraction.hpp"
#include "AEC.hpp"

/*
"AEC" == Arithmetic Expression Construction.
*/

using std::string;
using std::vector;

string AEC_solve(vector<string> *number_array, const string &target) {
    aec_solve_progress = 0;
    size_t input_size = number_array->size();
    aec_solve_total = input_size;
    if (input_size > 63) {
        throw TooManyNumbersException();
    }
    vector<node> node_array(input_size);
    for (size_t i = 0; i < input_size; i++) {
        node_array[i] = node(1ULL << i, (*number_array)[i]);
    }
    fraction fraction_target(target);
    for (size_t i = 2; i <= input_size; i++) {
        if(!keep_running) return "";
        aec_solve_progress = i;
        iterate(&node_array, static_cast<int>(i));
    }
    string result;
    for (size_t i = 0; i < node_array.size(); i++) {
        if(!keep_running) return "";
        node *p = node_array.data() + i;
        if (compare(&(p->value), &fraction_target) == 0 && (size_t) p->complexity == input_size) {
            result += p->toString(&node_array) + "\n";
        }
    }
    if (!result.empty()) {
        return result;
    } else {
        return "No solution\n";
    }
}

aecAnswer AEC_generate(const string &boundary, const string &sz, const string &target) {
    int64_t min = 0;
    int64_t max = 0;
    size_t n = 0;
    if (!boundary.empty()) {
        std::sscanf(boundary.c_str(), "%ld %ld", &min, &max);
    } else {
        min = 1;
        max = 13;
    }
    if (!sz.empty()) {
        std::sscanf(sz.c_str(), "%zu", &n);
    } else {
        n = 4;
    }
    std::random_device rd;
    std::mt19937_64 rnd(rd());
    std::uniform_int_distribution<int64_t> intDist(min, max);
    std::uniform_int_distribution<size_t> treeDist(1, n-1);
    std::uniform_int_distribution<uint8_t> opDist(0, 3);

    node_g root;
    root.depth = n;
    if (!target.empty()) {
        root.value = fraction(target);
    } else {
        root.value = fraction("24");
    }

    root.recurse_gen(rnd, intDist, opDist);

    string s1 = root.getQuestion() + "=" + root.value.toString();
    string s2 = root.getAnswer();

    return {s1, s2};
}

void node_g::recurse_gen(std::mt19937_64 rnd,
                         std::uniform_int_distribution<int64_t> intDist,
                         std::uniform_int_distribution<uint8_t> opDist) {
    if (this->depth == 1) {
        return;
    } else if (this->depth > 1) {
        std::uniform_int_distribution<size_t> treeDist(1, this->depth-1);
        size_t left_depth = treeDist(rnd);
        size_t right_depth = this->depth - left_depth;

        this->op = opDist(rnd);

        fraction rightValue(intDist(rnd));
        fraction leftValue;
        switch (this->op) {
            case ADD:
                sub(&(this->value), &rightValue, &leftValue);
                break;
            case SUB:
                add(&(this->value), &rightValue, &leftValue);
                break;
            case MUL:
                div(&(this->value), &rightValue, &leftValue);
                break;
            case DIV:
                mul(&(this->value), &rightValue, &leftValue);
                break;
            default:
                break;
        }

        this->left = new node_g(leftValue, left_depth);
        this->right = new node_g(rightValue, right_depth);

        this->left->recurse_gen(rnd, intDist, opDist);
        this->right->recurse_gen(rnd, intDist, opDist);
        return;
    }
}

void iterate(vector<node> *input_array, int target_complexity) {
    aec_iterate_progress = 0;
    auto input_size = static_cast<int64_t>(input_array->size());
    aec_iterate_total = (input_size * (input_size - 1)) >> 1;
    node *start = input_array->data();
    for (int64_t i = 0; i < input_size; i++) {
        node *p1 = start + i;
        for (int64_t j = i + 1; j < input_size; j++) {
            if(!keep_running) return;
            node *p2 = start + j;
            if ((p1->mask & p2->mask) == 0 &&
                p1->complexity + p2->complexity == target_complexity) {
                size_t last_size = input_array->size();
                size_t delta_size = 6;
                bool b1 = p1->value.numerator == 0;
                bool b2 = p2->value.numerator == 0;
                if (b1) delta_size--;
                if (b2) delta_size--;
                input_array->resize(last_size + delta_size);
                start = input_array->data();
                p1 = start + i;
                p2 = start + j;
                node *p = start + last_size;
                node_add(p1, p2, p, i, j);
                p++;
                node_mul(p1, p2, p, i, j);
                p++;
                node_sub(p1, p2, p, i, j);
                p++;
                node_sub(p2, p1, p, j, i);
                if (!b2) {
                    p++;
                    node_div(p1, p2, p, i, j);
                }
                if (!b1) {
                    p++;
                    node_div(p2, p1, p, j, i);
                }
            }
            aec_iterate_progress++;
        }
    }
}

node::node()
        : value(), left(-1), right(-1), mask(0), op(0) {}  //凑数，为了能用vector。

node::node(uint64_t init_mask, const string &number, int8_t base)
        : value(number, base), left(-1), right(-1), mask(init_mask), op(0) {}

inline void node_add(node *a, node *b, node *out, int64_t left, int64_t right) {
    out->complexity = a->complexity + b->complexity;
    out->op = ADD;
    out->left = left;
    out->right = right;
    out->mask = a->mask | b->mask;
    add(&(a->value), &(b->value), &(out->value));
}

inline void node_sub(node *a, node *b, node *out, int64_t left, int64_t right) {
    out->complexity = a->complexity + b->complexity;
    out->op = SUB;
    out->left = left;
    out->right = right;
    out->mask = a->mask | b->mask;
    sub(&(a->value), &(b->value), &(out->value));
}

inline void node_mul(node *a, node *b, node *out, int64_t left, int64_t right) {
    out->complexity = a->complexity + b->complexity;
    out->op = MUL;
    out->left = left;
    out->right = right;
    out->mask = a->mask | b->mask;
    mul(&(a->value), &(b->value), &(out->value));
}

inline void node_div(node *a, node *b, node *out, int64_t left, int64_t right) {
    out->complexity = a->complexity + b->complexity;
    out->op = DIV;
    out->left = left;
    out->right = right;
    out->mask = a->mask | b->mask;
    div(&(a->value), &(b->value), &(out->value));
}

string node::toString(vector<node> *input) const {
    if (this->left == -1) {
        return this->value.toString();
    } else {
        string sop = ops[this->op];
        return '('
               + input->at(left).toString(input)
               + sop
               + input->at(right).toString(input)
               + ')';
    }
}

node_g::node_g()
        : value(), depth(1), left(nullptr), right(nullptr), op(0) {}

node_g::node_g(fraction f, size_t n)
        : value(f), depth(n), left(nullptr), right(nullptr), op(0) {}

string node_g::getAnswer() const {
    if (this->depth == 1) {
        return this->value.toString();
    } else {
        string sop = ops[this->op];
        return '('
               + this->left->getAnswer()
               + sop
               + this->right->getAnswer()
               + ')';
    }
}

string node_g::getQuestion() const {
    if (this->depth == 1) {
        return this->value.toString();
    } else {
        return this->left->getQuestion()
               + " "
               + this->right->getQuestion();
    }
}

aecAnswer::aecAnswer(string question, string answer)
        : question(std::move(question)), answer(std::move(answer)) {}