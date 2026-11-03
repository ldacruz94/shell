#include "error.hpp"

#include <cerrno>
#include <system_error>

namespace shell {

void throw_errno(const char* what) {
    throw std::system_error(errno, std::generic_category(), what);
}

}