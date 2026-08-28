#pragma once

#include "brick/boards/esp32/p4/Jc8012BoardConfig.h"
#include "brick/interfaces/board/BoardDescriptor.h"
#include "brick/interfaces/board/IBoard.h"
#include "brick/interfaces/display/DisplayTypes.h"
#if BRICK_JC8012_ENABLE_DISPLAY
#include "brick/platform/esp32/p4/MipiDsiDisplay.h"
#include "brick/boards/esp32/p4/profiles/jd9365_800x1280.h"
#endif
#if BRICK_JC8012_ENABLE_TOUCH
#include "brick/platform/esp32/touch/Gsl3680Touchscreen.h"
#include "brick/boards/esp32/p4/profiles/jc8012_gsl3680.h"
#endif

namespace brick::platform::esp32::p4
{

class Jc8012Board final : public brick::interfaces::board::IBoard
{
public:
    explicit Jc8012Board(brick::interfaces::display::Rotation rotation = brick::interfaces::display::Rotation::rotate_0)
#if BRICK_JC8012_ENABLE_DISPLAY
        : display_(profiles::jd9365_800x1280(rotation))
#if BRICK_JC8012_ENABLE_TOUCH
        , touch_(profiles::jc8012_gsl3680(rotation))
#endif
#elif BRICK_JC8012_ENABLE_TOUCH
        : touch_(profiles::jc8012_gsl3680(rotation))
#endif
    {
    }

    static constexpr brick::interfaces::board::BoardDescriptor descriptor_static()
    {
        using brick::interfaces::board::Capability;
        return {"JC8012 10-inch", "ESP32-P4", (BRICK_JC8012_ENABLE_DISPLAY ? static_cast<std::uint32_t>(Capability::display) : 0U) |
                                                        (BRICK_JC8012_ENABLE_TOUCH ? static_cast<std::uint32_t>(Capability::touchscreen) : 0U)};
    }
    brick::interfaces::board::BoardDescriptor descriptor() const override { return descriptor_static(); }

    bool begin()
    {
        bool ok = true;
#if BRICK_JC8012_ENABLE_DISPLAY
        ok = ok && display_.begin();
#endif
#if BRICK_JC8012_ENABLE_TOUCH
        ok = ok && touch_.begin();
#endif
        return ok;
    }

#if BRICK_JC8012_ENABLE_DISPLAY
    MipiDsiDisplay& display() { return display_; }
    brick::interfaces::display::IDisplayDevice* display_device() override { return &display_; }
#else
    brick::interfaces::display::IDisplayDevice* display_device() override { return nullptr; }
#endif
#if BRICK_JC8012_ENABLE_TOUCH
    touch::Gsl3680Touchscreen& touch() { return touch_; }
    brick::interfaces::display::ITouchscreen* touchscreen() override { return &touch_; }
#else
    brick::interfaces::display::ITouchscreen* touchscreen() override { return nullptr; }
#endif

private:
#if BRICK_JC8012_ENABLE_DISPLAY
    MipiDsiDisplay display_;
#endif
#if BRICK_JC8012_ENABLE_TOUCH
    touch::Gsl3680Touchscreen touch_;
#endif
};

}  // namespace brick::platform::esp32::p4
