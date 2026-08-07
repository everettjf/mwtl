#pragma once

#include <windows.h>

#include <concepts>
#include <string_view>
#include <system_error>
#include <utility>

#include <mwtl/dpi.h>
#include <mwtl/events.h>

namespace mwtl {

// Binds repeated control creation to one parent while leaving control lifetime,
// IDs, bounds, and native option structs explicit at the call site.
class ControlHost final {
public:
    explicit ControlHost(HWND parent) noexcept : parent_(parent) {}

    template <typename Parent>
        requires requires(const Parent& value) {
            { value.GetHwnd() } -> std::same_as<HWND>;
        }
    explicit ControlHost(const Parent& parent) noexcept : parent_(parent.GetHwnd()) {}

    template <typename Control, typename... Arguments>
        requires requires(Control& control, HWND parent, Arguments&&... arguments) {
            { control.Create(parent, std::forward<Arguments>(arguments)...) }
                -> std::convertible_to<bool>;
        }
    Control& Add(Control& control, Arguments&&... arguments) const {
        return AddNative(control, std::forward<Arguments>(arguments)...);
    }

    template <typename Control, typename... Options>
    Control& Add(Control& control, ControlId id, RectDip bounds,
                 Options&&... options) const {
        return AddNative(
            control, id, bounds, std::forward<Options>(options)...);
    }

    template <typename Control, typename... Options>
    Control& Add(Control& control, ControlId id, std::wstring_view text,
                 RectDip bounds, Options&&... options) const {
        return AddNative(
            control, id, text, bounds, std::forward<Options>(options)...);
    }

    template <typename Control, typename... Arguments>
        requires requires(Control& control, HWND parent, Arguments&&... arguments) {
            { control.Create(parent, std::forward<Arguments>(arguments)...) }
                -> std::convertible_to<bool>;
        }
    Control& AddNative(Control& control, Arguments&&... arguments) const {
        ::SetLastError(ERROR_SUCCESS);
        if (!control.Create(parent_, std::forward<Arguments>(arguments)...)) {
            DWORD error = ::GetLastError();
            if (error == ERROR_SUCCESS) error = ERROR_CANNOT_MAKE;
            throw std::system_error(
                static_cast<int>(error), std::system_category(),
                "mwtl control creation");
        }
        return control;
    }

    [[nodiscard]] HWND GetParent() const noexcept { return parent_; }

private:
    HWND parent_ = nullptr;
};

}  // namespace mwtl
