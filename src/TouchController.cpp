#include <MaiSense/TouchController.hpp>
#include <MaiSense/Config.hpp>
#include <utility>

namespace MaiSense
{
    TouchController::TouchController() = default;

    TouchController::~TouchController() = default;

    void TouchController::SetCallback(std::function<void(TouchEvent)> cb)
    {
        callback = std::move(cb);
    }

    bool TouchController::Check(const int evCode)
    {
        switch (evCode)
        {
        case WM_POINTERENTER:
        case WM_NCPOINTERDOWN:
        case WM_NCPOINTERUP:
        case WM_NCPOINTERUPDATE:
        case WM_POINTERACTIVATE:
        case WM_POINTERCAPTURECHANGED:
        case WM_POINTERDOWN:
        case WM_POINTERLEAVE:
        case WM_POINTERUP:
        case WM_POINTERUPDATE:
            return true;
        default:
            return false;
        }
    }

    void TouchController::OnInput(int nCode, WPARAM wParam, LPARAM lParam)
    {
        // Parse message
        auto msg = (LPMSG)lParam;

        // Declare pointer info
        const UINT32       pointer_id = GET_POINTERID_WPARAM(msg->wParam);
        POINTER_INFO       pointer_info = { sizeof(POINTER_INFO) };
        POINTER_INPUT_TYPE pointer_type = 0;

        // Parse pointer info
        if (GetPointerType(pointer_id, &pointer_type) && GetPointerInfo(pointer_id, &pointer_info))
        {
            // Only retrieve touch event and map coordinate into client coordinate
            POINT point = pointer_info.ptPixelLocation;
            if (pointer_type == PT_TOUCH && ScreenToClient(msg->hwnd, &point))
            {
                // Initialize touch event
                auto ev = TouchEvent();
                ev.Id = pointer_info.pointerId;
                ev.X = point.x;
                ev.Y = point.y;
                ev.Flag = pointer_info.pointerFlags;
                    
                // Pass event into callback
                if (callback)
                {
                    callback(ev);
                }
            }
        }
    }
}
 