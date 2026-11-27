#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include <random>
#include "../globals.hpp"
#include "../fraction/fraction.hpp"
#include "../utils/string_process.hpp"
#include "AEC.hpp"

/*
"AEC" == Arithmetic Expression Construction.
*/

using std::string;
using std::vector;
using std::sscanf;
using std::random_device;
using std::mt19937_64;
using std::uniform_int_distribution;

string AEC_solve(const string &numbers_str, const string &target) {
    fraction fraction_target(target);
    vector<string> number_array = string_split_char(numbers_str, ' ');
    size_t input_size = number_array.size();
    if (input_size > 50) { //It seems impossible to solve an AEC of more than 50 numbers.
        throw TooManyNumbersException();
    }else if(input_size == 1){
        if(compare(fraction(number_array.at(0)), target) == 0){
            return number_array.at(0);
        }else{
            return "No Solution.\n";
        }
    }
    aec_round_finished = 0;
    aec_round_total = input_size;
    size_t bucket_number = 1ULL << input_size;
    aec_buckets_filled = 0;

    //total bucket operations.
    size_t tmp1 = 1;
    for (size_t i=0; i<input_size; i++){
        tmp1 *= 3;
    }
    aec_buckets_total = input_size + (tmp1 + 1 - (1ULL<<(input_size+1))) / 2;
    //total bucket operations.

    vector<node_bucket> all_buckets(bucket_number);
    vector<node> node_pool(input_size);
    for (int64_t i = 0; i < input_size; i++) {
        if (!keep_running) return "";
        node_pool.at(i) = node(number_array.at(i));
    }
    aec_round_finished++;
    for (int64_t i = 0; i < input_size; i++) {
        if (!keep_running) return "";
        if(node_pool.at(i).value.numerator == 0){
            all_buckets.at(1LL << i) = node_bucket(vector<int64_t>{i}, 1ULL << i, 1, 1);
        } else {
            all_buckets.at(1LL << i) = node_bucket(vector<int64_t>{i}, 1ULL << i, 1, 0);
        }
        aec_buckets_filled++;
    } //初始化桶，+n
    for (size_t i = 2; i < input_size; i++) {
        for (size_t n=0; n<bucket_number; n++){
            if (!keep_running) return "";
            const node_bucket& bucket1 = all_buckets.at(n);
            if(bucket1.complexity != 0 && bucket1.complexity < i){
                vector<uint64_t> masks = getPossibleMasks(bucket1.mask, input_size, i);
                for(size_t a : masks){
                    if (!keep_running) return "";
                    if(a > n){
                        node_bucket bucket2 = all_buckets.at(a);
                        size_t mask = bucket1.mask | bucket2.mask;
                        bucketsCombineNormal(node_pool, bucket1, bucket2, all_buckets.at(mask), mask);
                        aec_buckets_filled++;
                    }
                }
            } else {
                continue;
            }
        }
        aec_round_finished++;
    }
    vector<node> result_pool;
    for (size_t n=0; n<bucket_number; n++){
        if (!keep_running) return "";
        const node_bucket& bucket1 = all_buckets.at(n);
        if(bucket1.complexity != 0){
            vector<uint64_t> masks = getPossibleMasks(bucket1.mask, input_size, input_size);
            for(size_t a : masks){
                if(a > n){
                    const node_bucket& bucket2 = all_buckets.at(a);
                    vector<node> tmp = bucketsCombineCheck(node_pool, bucket1, bucket2, fraction_target);
                    result_pool.insert(result_pool.end(),tmp.begin(),tmp.end());
                    aec_buckets_filled++;
                }
            }
        } else {
            continue;
        }
    }
    aec_round_finished++;
    if (!result_pool.empty()){
        string result;
        for (node n : result_pool) {
            if (!keep_running) return "";
            result += n.toString(node_pool);
            result += '\n';
        }
        return result;
    }else{
        return "No Solution.\n";
    }
}

aecAnswer AEC_generate(const string &boundary, const string &sz, const string &target) {
    int64_t min = 0;
    int64_t max = 0;
    size_t n = 0;
    if (!boundary.empty()) {
        sscanf(boundary.c_str(), "%ld %ld", &min, &max);
    } else {
        min = 1;
        max = 13;
    }
    if (!sz.empty()) {
        sscanf(sz.c_str(), "%zu", &n);
    } else {
        n = 4;
    }
    random_device rd;
    mt19937_64 rnd(rd());
    uniform_int_distribution<int64_t> intDist(min, max);
    uniform_int_distribution<size_t> treeDist(1, n - 1);
    uniform_int_distribution<uint8_t> opDist(0, 3);

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

void node_g::recurse_gen(mt19937_64 rnd,
                         uniform_int_distribution<int64_t> intDist,
                         uniform_int_distribution<uint8_t> opDist) {
    if (this->depth == 1 || !keep_running) {
        return;
    } else if (this->depth > 1) {
        uniform_int_distribution<size_t> treeDist(1, this->depth - 1);
        size_t left_depth = treeDist(rnd);
        size_t right_depth = this->depth - left_depth;

        this->op = opDist(rnd);

        fraction rightValue(intDist(rnd));
        fraction leftValue;
        switch (this->op) {
            case ADD:
                sub(this->value, rightValue, leftValue);
                break;
            case SUB:
                add(this->value, rightValue, leftValue);
                break;
            case MUL:
                div(this->value, rightValue, leftValue);
                break;
            case DIV:
                mul(this->value, rightValue, leftValue);
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

void bucketsCombineNormal(vector<node> &node_pool, const node_bucket &a, const node_bucket &b, node_bucket &out, uint64_t mask) {
    size_t size_a = a.elements.size();
    size_t size_b = b.elements.size();
    size_t current_size = node_pool.size();
    size_t new_size = 6*size_a*size_b - size_a*b.zero_count - size_b*a.zero_count;

    aec_comb_finished = 0;
    aec_comb_total = new_size;

    size_t zero_count = 0;
    vector<int64_t> &tmp = out.elements;
    tmp.reserve(tmp.size()+new_size);
    node_pool.resize(current_size + new_size);

    auto index = static_cast<int64_t>(current_size);
    for (int64_t index_a : a.elements){
        for(int64_t index_b : b.elements){
            if (!keep_running) return;
            tmp.push_back(index);
            zero_count += node_add(node_pool,index_a,index_b,index++);
            tmp.push_back(index);
            zero_count += node_mul(node_pool,index_a,index_b,index++);
            tmp.push_back(index);
            zero_count += node_sub(node_pool,index_a,index_b,index++);
            tmp.push_back(index);
            zero_count += node_sub(node_pool,index_b,index_a,index++);
            aec_comb_finished += 4;
            if (node_pool.at(index_b).value.numerator != 0){
                tmp.push_back(index);
                zero_count += node_div(node_pool,index_a,index_b,index++);
                aec_comb_finished++;
            }
            if (node_pool.at(index_a).value.numerator != 0){
                tmp.push_back(index);
                zero_count += node_div(node_pool,index_b,index_a,index++);
                aec_comb_finished++;
            }
        }
    }
    out.mask = mask;
    out.complexity = a.complexity + b.complexity;
    out.zero_count += zero_count;
}

vector<node> bucketsCombineCheck(vector<node> &node_pool, const node_bucket &a, const node_bucket &b, const fraction &target){
    size_t size_a = a.elements.size();
    size_t size_b = b.elements.size();
    aec_comb_finished = 0;
    aec_comb_total = 6*size_a*size_b - size_a*b.zero_count - size_b*a.zero_count;
    vector<node> result;
    auto n = static_cast<int64_t>(node_pool.size());
    node_pool.resize(n+1);
    node &tmp = node_pool.at(n);
    for (int64_t index_a : a.elements){
        for(int64_t index_b : b.elements){
            if (!keep_running) return result;
            node_add(node_pool,index_a,index_b,n);
            if(compare(tmp.value,target) == 0) result.push_back(tmp);
            node_mul(node_pool,index_a,index_b,n);
            if(compare(tmp.value,target) == 0) result.push_back(tmp);
            node_sub(node_pool,index_a,index_b,n);
            if(compare(tmp.value,target) == 0) result.push_back(tmp);
            node_sub(node_pool,index_b,index_a,n);
            if(compare(tmp.value,target) == 0) result.push_back(tmp);
            aec_comb_finished += 4;
            if (node_pool.at(index_b).value.numerator != 0){
                node_div(node_pool,index_a,index_b,n);
                if(compare(tmp.value,target) == 0) result.push_back(tmp);
                aec_comb_finished++;
            }
            if (node_pool.at(index_a).value.numerator != 0){
                node_div(node_pool,index_b,index_a,n);
                if(compare(tmp.value,target) == 0) result.push_back(tmp);
                aec_comb_finished++;
            }
        }
    }
    node_pool.pop_back();
    return result;
}

vector<uint64_t> getPossibleMasks(uint64_t msk, size_t n, size_t target_complexity) {
    vector<uint64_t> result;
    if(!keep_running) return result;
    vector<size_t> p;
    p.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        uint64_t j = 1ULL << i;
        if ((msk & j) == 0) p.push_back(j);
    }
    size_t popcountz = p.size();
    size_t delta_complexity = target_complexity - (n - popcountz);
    size_t sz = comb_number64(popcountz, delta_complexity);
    if (delta_complexity <= popcountz){
        result.reserve(sz);
        fillMask(0, delta_complexity, p, result);
    }
    return result;
}

void fillMask(uint64_t input, size_t depth, const vector<uint64_t> &candidates,
              vector<uint64_t> &container) {
    if (depth == 0) {
        container.push_back(input);
        return;
    }else if(candidates.size() < depth || !keep_running){
        return;
    } else {
        size_t sz = candidates.size();
        vector<uint64_t> next_candidates;
        for (size_t i = 0; i < sz; i++) {
            uint64_t next_input = input | candidates.at(i);
            next_candidates = candidates;
            next_candidates.erase(next_candidates.begin(), next_candidates.begin()+static_cast<long>(i)+1);
            fillMask(next_input, depth - 1, next_candidates, container);
        }
    }
}

node::node()
        : value(), left(-1), right(-1), op(0) {}  //凑数，为了能用vector。

node::node(const string &number, int8_t base)
        : value(number, base), left(-1), right(-1), op(0) {}

node_bucket::node_bucket()
    :elements(), mask(0), complexity(0), zero_count(0) {}

node_bucket::node_bucket(std::vector<int64_t> elements, uint64_t mask, uint8_t complexity, size_t zero_count)
    :elements(std::move(elements)), mask(mask), complexity(complexity), zero_count(zero_count) {}

size_t node_add(vector<node> &node_pool, int64_t a, int64_t b, int64_t out) {
    node &n = node_pool.at(out);
    add(node_pool.at(a).value,node_pool.at(b).value,n.value);
    n.left = a;
    n.right = b;
    n.op = ADD;
    return n.value.numerator == 0 ? 1:0;
}

size_t node_sub(vector<node> &node_pool, int64_t a, int64_t b, int64_t out) {
    node &n = node_pool.at(out);
    sub(node_pool.at(a).value,node_pool.at(b).value,n.value);
    n.left = a;
    n.right = b;
    n.op = SUB;
    return n.value.numerator == 0 ? 1:0;
}

size_t node_mul(vector<node> &node_pool, int64_t a, int64_t b, int64_t out) {
    node &n = node_pool.at(out);
    mul(node_pool.at(a).value,node_pool.at(b).value,n.value);
    n.left = a;
    n.right = b;
    n.op = MUL;
    return n.value.numerator == 0 ? 1:0;
}

size_t node_div(vector<node> &node_pool, int64_t a, int64_t b, int64_t out) {
    node &n = node_pool.at(out);
    div(node_pool.at(a).value,node_pool.at(b).value,n.value);
    n.left = a;
    n.right = b;
    n.op = DIV;
    return n.value.numerator == 0 ? 1:0;
}

string node::toString(const vector<node> &input) const {
    if (this->left == -1) {
        return this->value.toString();
    } else {
        string sop = ops[this->op];
        return '('
               + input.at(left).toString(input)
               + sop
               + input.at(right).toString(input)
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