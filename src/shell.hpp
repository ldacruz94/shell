#pragma once

#include <string>
#include <vector>

namespace shell {

class Shell {

public:
    int run();
    int last_status() const { return last_status_; }

private:
    int run_command(const std::vector<std::string>& words);

    int last_status_ = 0;

};

}