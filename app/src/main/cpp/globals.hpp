//
// Created by alfred on 2026/11/22.
//

#ifndef MATH24_GLOBALS_HPP
#define MATH24_GLOBALS_HPP

#include <cstdint>
#include <atomic>

extern std::atomic<size_t> aec_round_total;
extern std::atomic<size_t> aec_round_finished;
extern std::atomic<size_t> aec_buckets_total;
extern std::atomic<size_t> aec_buckets_filled;
extern std::atomic<size_t> aec_comb_total;
extern std::atomic<size_t> aec_comb_finished;
extern std::atomic<bool> keep_running;

#endif //MATH24_GLOBALS_HPP
