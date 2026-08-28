#pragma once
#include "brick/boards/esp32/s3/Panel480BoardConfig.h"
#include "brick/interfaces/board/BoardDescriptor.h"
#include "brick/interfaces/display/DisplayTypes.h"
#if BRICK_PANEL480_ENABLE_DISPLAY
#include "brick/platform/esp32/s3/St7701sRgbDisplay.h"
#include "brick/platform/esp32/s3/profiles/st7701s_480x480.h"
#endif
#if BRICK_PANEL480_ENABLE_TOUCH
#include "brick/platform/esp32/touch/Gt911Touchscreen.h"
#include "brick/platform/esp32/s3/profiles/st7701s_gt911.h"
#endif
namespace brick::platform::esp32::s3 {
class Panel480Board final {
public:
    explicit Panel480Board(brick::interfaces::display::Rotation rotation = brick::interfaces::display::Rotation::rotate_0)
#if BRICK_PANEL480_ENABLE_DISPLAY
        : display_(profiles::st7701s_480x480())
#if BRICK_PANEL480_ENABLE_TOUCH
        , touch_(profiles::st7701s_gt911())
#endif
#elif BRICK_PANEL480_ENABLE_TOUCH
        : touch_(profiles::st7701s_gt911())
#endif
    { (void)rotation; }
    static constexpr brick::interfaces::board::BoardDescriptor descriptor() {
        using brick::interfaces::board::Capability;
        return {"480x480 panel", "ESP32-S3", (BRICK_PANEL480_ENABLE_DISPLAY ? static_cast<std::uint32_t>(Capability::display) : 0U) | (BRICK_PANEL480_ENABLE_TOUCH ? static_cast<std::uint32_t>(Capability::touchscreen) : 0U)};
    }
    bool begin() {
        bool ok = true;
#if BRICK_PANEL480_ENABLE_DISPLAY
        ok = ok && display_.begin();
#endif
#if BRICK_PANEL480_ENABLE_TOUCH
        ok = ok && touch_.begin();
#endif
        return ok;
    }
#if BRICK_PANEL480_ENABLE_DISPLAY
    St7701sRgbDisplay& display() { return display_; }
#endif
#if BRICK_PANEL480_ENABLE_TOUCH
    touch::Gt911Touchscreen& touch() { return touch_; }
#endif
private:
#if BRICK_PANEL480_ENABLE_DISPLAY
    St7701sRgbDisplay display_;
#endif
#if BRICK_PANEL480_ENABLE_TOUCH
    touch::Gt911Touchscreen touch_;
#endif
};
} // namespace brick::platform::esp32::s3
