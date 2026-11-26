//
// Created by alfred on 2026/11/22.
//
#include <cstdint>
#include <atomic>
#include "globals.hpp"

std::atomic<size_t> aec_round_total(0);
std::atomic<size_t> aec_round_finished(0);
std::atomic<size_t> aec_buckets_total(0);
std::atomic<size_t> aec_buckets_filled(0);
std::atomic<size_t> aec_comb_total(0);
std::atomic<size_t> aec_comb_finished(0);
std::atomic<bool> keep_running(false);