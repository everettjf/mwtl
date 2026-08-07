#include <mwtl/control_batch.h>

void mwtl_header_control_batch_smoke() {
    mwtl::ComboBox combo;
    const auto result = mwtl::AddItems(combo, {L"one", L"two"});
    static_cast<void>(result);
}
