#include <mwtl/control_batch.h>

#include <string_view>

namespace {

struct FakeItems {
    int AddItem(std::wstring_view) noexcept {
        return calls++ == fail_at ? -7 : calls - 1;
    }

    int calls = 0;
    int fail_at = -1;
};

}  // namespace

int main() {
    FakeItems success;
    const mwtl::BatchResult added =
        mwtl::AddItems(success, {L"one", L"two", L"three"});
    if (!added || added.completed != 3 || added.failed_at != -1) return 1;

    FakeItems failure;
    failure.fail_at = 1;
    const mwtl::BatchResult stopped =
        mwtl::AddItems(failure, {L"one", L"two", L"three"});
    if (stopped || stopped.completed != 1 || stopped.failed_at != 1 ||
        stopped.native_result != -7 || failure.calls != 2) {
        return 2;
    }
    return 0;
}
