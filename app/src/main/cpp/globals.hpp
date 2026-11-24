//
// Created by alfred on 2026/11/22.
//

#ifndef MATH24_GLOBALS_HPP
#define MATH24_GLOBALS_HPP

#include <cstdint>
#include <atomic>

extern std::atomic<size_t> aec_iterate_total;
extern std::atomic<size_t> aec_iterate_progress;
extern std::atomic<size_t> aec_solve_total;
extern std::atomic<size_t> aec_solve_progress;
extern std::atomic<bool> keep_running;

#endif //MATH24_GLOBALS_HPP
