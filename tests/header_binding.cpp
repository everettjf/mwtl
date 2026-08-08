#include <mwtl/binding.h>

void HeaderBindingCompiles() {
    mwtl::ChangeGate gate;
    static_cast<void>(gate);
}
