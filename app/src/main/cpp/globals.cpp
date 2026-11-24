//
// Created by alfred on 2026/11/22.
//
#include <cstdint>
#include <atomic>
#include "globals.hpp"

std::atomic<size_t> aec_iterate_total(0);
std::atomic<size_t> aec_iterate_progress(0);
std::atomic<size_t> aec_solve_total(0);
std::atomic<size_t> aec_solve_progress(0);
std::atomic<bool> keep_running(false);