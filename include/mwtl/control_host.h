#pragma once

#include <windows.h>

#include <string_view>
#include <system_error>
#include <utility>

#include <mwtl/dpi.h>
#include <mwtl/concepts.h>
#include <mwtl/events.h>

namespace mwtl {

// Binds repeated control creation to one parent while leaving control lifetime,
// IDs, bounds, and native option structs explicit at the call site.
class ControlHost final {
public:
    explicit ControlHost(HWND parent) noexcept : parent_(parent) {}

    template <WindowLike Parent>
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
        requires requires(
            Control& control, HWND parent, ControlId id, Options&&... options) {
            { control.Create(
                parent, id, RectDip{}, std::forward<Options>(options)...) }
                -> std::convertible_to<bool>;
        }
    Control& Add(
        Control& control, ControlId id, Options&&... options) const {
        return AddNative(
            control, id, RectDip{}, std::forward<Options>(options)...);
    }

    template <typename Control, typename... Options>
        requires requires(
            Control& control, HWND parent, ControlId id,
            std::wstring_view text, Options&&... options) {
            { control.Create(
                parent, id, text, RectDip{},
                std::forward<Options>(options)...) }
                -> std::convertible_to<bool>;
        }
    Control& Add(
        Control& control, ControlId id, std::wstring_view text,
        Options&&... options) const {
        return AddNative(
            control, id, text, RectDip{},
            std::forward<Options>(options)...);
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
