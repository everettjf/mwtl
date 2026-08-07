#pragma once

#include <windows.h>

#include <concepts>
#include <source_location>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace mwtl {

namespace detail {

inline std::string DescribeOperation(
    std::string_view operation,
    const std::source_location& where) {
    std::ostringstream stream;
    stream << (operation.empty() ? "mwtl operation" : operation)
           << " at " << where.file_name() << ':' << where.line();
    return stream.str();
}

template <typename Result>
concept DetailedCheckResult = requires(const Result& result) {
    { result.Succeeded() } -> std::same_as<bool>;
    result.completed;
    result.failed_at;
    result.native_result;
};

}  // namespace detail

inline void Must(
    bool succeeded,
    std::string_view operation = {},
    const std::source_location where = std::source_location::current()) {
    if (succeeded) return;
    DWORD error = ::GetLastError();
    if (error == ERROR_SUCCESS) error = ERROR_GEN_FAILURE;
    throw std::system_error(
        static_cast<int>(error), std::system_category(),
        detail::DescribeOperation(operation, where));
}

template <detail::DetailedCheckResult Result>
Result Must(
    Result result,
    std::string_view operation = {},
    const std::source_location where = std::source_location::current()) {
    if (result.Succeeded()) return result;
    std::ostringstream stream;
    stream << detail::DescribeOperation(operation, where)
           << " failed at item " << result.failed_at
           << " after " << result.completed
           << " completed; native result " << result.native_result;
    throw std::runtime_error(stream.str());
}

}  // namespace mwtl
