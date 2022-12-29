#include <MaiSense/KeyboardController.hpp>
#include <utility>

#include "MaiSense/InputManager.hpp"

namespace MaiSense
{
    KeyboardController::KeyboardController()    = default;

    KeyboardController::~KeyboardController()   = default;

    void KeyboardController::SetCallback(std::function<void(KeyEvent)> cb)
    {
        callback = std::move(cb);
    }

    bool KeyboardController::Check(int evCode)
    {
        switch (evCode)
        {
        case WM_KEYDOWN:
        case WM_KEYUP:
            return true;
        default:
            return false;
        }
    }

    void KeyboardController::OnInput(int nCode, WPARAM wParam, LPARAM lParam)
    {
        // Parse keyboard hook event
        auto ev  = KeyEvent();
        auto msg = (LPMSG)lParam;

        // Initialize key event
        ev.KeyCode = msg->wParam;
        ev.Active  = msg->message == WM_KEYDOWN;

        // Pass event into callback
        if (callback)
        {
            callback(ev);
        }
    }

    void KeyboardController::ProcessKeyboardInput(const KeyEvent ev)
    {
        // Set keyboard eventIds to start from -3 and decrease, this will avoid conflicts with the touch events
        // Trigger point will be always static (1,1)

        const auto sensor = InputManager::GetSensor();
        switch (ev.KeyCode)
        {
            case 0x30: // 0
                sensor->Queue(Sensor::C, ev.Active, -3, {1, 1});
                break;
            case 0x31: // 1
                sensor->Queue(Sensor::A1, ev.Active, -4, { 1, 1 });
                break;
            case 0x32: // 2
                sensor->Queue(Sensor::A2, ev.Active, -5, { 1, 1 });
                break;
            case 0x33: // 3
                sensor->Queue(Sensor::A3, ev.Active, -6, { 1, 1 });
                break;
            case 0x34: // 4
                sensor->Queue(Sensor::A4, ev.Active, -7, { 1, 1 });
                break;
            case 0x35: // 5
                sensor->Queue(Sensor::A5, ev.Active, -8, { 1, 1 });
                break;
            case 0x36: // 6
                sensor->Queue(Sensor::A6, ev.Active, -9, { 1, 1 });
                break;
            case 0x37: // 7
                sensor->Queue(Sensor::A7, ev.Active, -10, { 1, 1 });
                break;
            case 0x38: // 8
                sensor->Queue(Sensor::A8, ev.Active, -11, { 1, 1 });
                break;
            case VK_NUMPAD1: // NUMPAD 1
                sensor->Queue(Sensor::B1, ev.Active, -12, { 1, 1 });
                break;
            case VK_NUMPAD2: // NUMPAD 2
                sensor->Queue(Sensor::B2, ev.Active, -13, { 1, 1 });
                break;
            case VK_NUMPAD3: // NUMPAD 3
                sensor->Queue(Sensor::B3, ev.Active, -14, { 1, 1 });
                break;
            case VK_NUMPAD4: // NUMPAD 4
                sensor->Queue(Sensor::B4, ev.Active, -15, { 1, 1 });
                break;
            case VK_NUMPAD5: // NUMPAD 5
                sensor->Queue(Sensor::B5, ev.Active, -16, { 1, 1 });
                break;
            case VK_NUMPAD6: // NUMPAD 6
                sensor->Queue(Sensor::B6, ev.Active, -17, { 1, 1 });
                break;
            case VK_NUMPAD7: // NUMPAD 7
                sensor->Queue(Sensor::B7, ev.Active, -18, { 1, 1 });
                break;
            case VK_NUMPAD8: // NUMPAD 8
                sensor->Queue(Sensor::B8, ev.Active, -19, { 1, 1 });
                break;
        }
    }
}
