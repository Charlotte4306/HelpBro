#pragma once
#include <string>
#include <chrono>
#include <sstream>
#include <type_traits>
#include "logger.h"

// ─── RunConfig parser ───────────────────────────────────────
#include "config.h"
RunConfig parseArgs(int argc, char* argv[]);

// ─── Execution timer ────────────────────────────────────────
template <typename Function>
auto measureExecutionTime(const std::string& label, Function func, bool enabled)
    -> std::invoke_result_t<Function>;

#include "helper.tpp"
