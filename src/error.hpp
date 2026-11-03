#pragma once

namespace shell {

constexpr int kStatusNotFound = 127;
constexpr int kStatusCannotRun = 126;

[[noreturn]] void throw_errno(const char* what);

}