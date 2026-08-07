#include <mwtl/mwtl.h>

#include <chrono>
#include <cstdlib>
#include <stdexcept>

using namespace std::chrono_literals;
using mwtl::operator""_dip;

namespace {

bool command_seen = false;
bool key_seen = false;
bool custom_seen = false;
bool timer_seen = false;

class ModernApiWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        if (!label_.Create(
                *this, kLabel, L"Modern API test",
                {{8.0_dip, 8.0_dip}, {180.0_dip, 24.0_dip}}) ||
            !text_.Create(
                *this, kText, L"native edit",
                {{8.0_dip, 40.0_dip}, {180.0_dip, 28.0_dip}}) ||
            !button_.Create(
                *this, kButton, L"Verify",
                {{8.0_dip, 76.0_dip}, {100.0_dip, 28.0_dip}}) ||
            !check_.Create(
                *this, kCheck, L"Checked",
                {{120.0_dip, 76.0_dip}, {100.0_dip, 28.0_dip}}) ||
            !combo_.Create(
                *this, kCombo,
                {{8.0_dip, 112.0_dip}, {180.0_dip, 120.0_dip}}) ||
            !progress_.Create(
                *this, kProgress,
                {{8.0_dip, 148.0_dip}, {180.0_dip, 20.0_dip}}) ||
            !timer_.Start(*this, kTimer, 1ms)) {
            throw std::runtime_error("modern API fixture setup failed");
        }

        check_.SetChecked(true);
        static_cast<void>(combo_.AddItem(L"first"));
        static_cast<void>(combo_.AddItem(L"second"));
        progress_.SetRange(0, 100);
        progress_.SetValue(64);
        if (!combo_.SetSelection(1) || combo_.GetSelection() != 1 ||
            !check_.IsChecked() || progress_.GetValue() != 64) {
            throw std::runtime_error("modern controls state verification failed");
        }

        button_.Click();
        ::SendMessageW(GetHwnd(), WM_KEYDOWN, VK_SPACE, 1);
        ::SendMessageW(GetHwnd(), kCustomMessage, 42, 0);
    }

    mwtl::EventResult OnCommand(const mwtl::CommandEvent& event) override {
        command_seen = event.IsClicked(button_) &&
            text_.GetText() == L"native edit";
        return mwtl::EventResult::Handled();
    }

    mwtl::EventResult OnKeyDown(const mwtl::KeyEvent& event) override {
        key_seen = event.virtual_key == VK_SPACE && event.repeat_count == 1;
        return mwtl::EventResult::Handled();
    }

    mwtl::EventResult OnMessage(const mwtl::WindowMessage& message) override {
        if (message.id == kCustomMessage) {
            custom_seen = message.wparam == 42;
            return mwtl::EventResult::Handled(77);
        }
        return mwtl::EventResult::Propagate();
    }

    mwtl::EventResult OnTimer(mwtl::TimerId id) override {
        if (id == kTimer) {
            timer_seen = true;
            timer_.Stop();
            ::PostMessageW(GetHwnd(), WM_CLOSE, 0, 0);
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
    }

private:
    static constexpr UINT kCustomMessage = WM_APP + 76;
    static constexpr mwtl::ControlId kLabel{200};
    static constexpr mwtl::ControlId kText{201};
    static constexpr mwtl::ControlId kButton{202};
    static constexpr mwtl::ControlId kCheck{203};
    static constexpr mwtl::ControlId kCombo{204};
    static constexpr mwtl::ControlId kProgress{205};
    static constexpr mwtl::TimerId kTimer{9};

    mwtl::Label label_;
    mwtl::TextBox text_;
    mwtl::Button button_;
    mwtl::CheckBox check_;
    mwtl::ComboBox combo_;
    mwtl::ProgressBar progress_;
    mwtl::UiTimer timer_;
};

}  // namespace

int main() {
    const int result =
        mwtl::Application(::GetModuleHandleW(nullptr)).Run<ModernApiWindow>(SW_HIDE);
    return result == 0 && command_seen && key_seen && custom_seen && timer_seen
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}
