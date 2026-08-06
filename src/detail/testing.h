#pragma once

#ifdef MWTL_TESTING

namespace mwtl::detail {

struct LifecycleSnapshot {
    int module_initialized;
    int loop_registered;
    int loop_removed;
    int module_terminated;
};

void ResetLifecycleSnapshotForTesting() noexcept;
LifecycleSnapshot GetLifecycleSnapshotForTesting() noexcept;

}  // namespace mwtl::detail

#endif
