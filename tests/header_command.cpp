#include <mwtl/command.h>

void HeaderCommandCompiles() {
    mwtl::CommandSet commands;
    static_cast<void>(commands);
}
