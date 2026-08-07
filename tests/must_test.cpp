#include <mwtl/must.h>

#include <cstdlib>
#include <system_error>

int main() {
    ::SetLastError(ERROR_ACCESS_DENIED);
    try {
        mwtl::Must(false, "plain failure");
        return 1;
    } catch (const std::system_error& error) {
        if (error.code().value() != ERROR_GEN_FAILURE) return 2;
    }

    try {
        mwtl::MustInvoke([] {
            ::SetLastError(ERROR_FILE_NOT_FOUND);
            return false;
        }, "captured failure");
        return 3;
    } catch (const std::system_error& error) {
        if (error.code().value() != ERROR_FILE_NOT_FOUND) return 4;
    }

    if (!mwtl::MustInvoke([] { return true; }, "success")) return 5;
    return EXIT_SUCCESS;
}
