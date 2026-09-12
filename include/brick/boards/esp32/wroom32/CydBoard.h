#pragma once

#include <cstdint>

#include "brick/boards/esp32/detail/Logger.h"
#include "brick/boards/esp32/detail/Peripheral.h"
#include "brick/interfaces/board/BoardDescriptor.h"
#include "brick/interfaces/board/IBoard.h"
#include "brick/platform/esp32/FreeRtosTime.h"
#include "brick/platform/esp32/touch/Xpt2046Touchscreen.h"
#include "brick/platform/esp32/wroom32/profiles/cyd_ili9341_320x240.h"
#include "brick/platform/esp32/wroom32/profiles/cyd_xpt2046.h"
#include "driver/gpio.h"

namespace brick::platform::esp32
{

struct CydDefaultFeatures
{
    static constexpr bool display = true;
    static constexpr bool touch = true;
    static constexpr bool backlight = true;
    static constexpr bool logging = true;
    static constexpr int log_level = 0;
};

template <typename Features = CydDefaultFeatures> class CydBoardTemplate final : public brick::interfaces::board::IBoard
{
  public:
    CydBoardTemplate() : logger_(Features::log_level)
    {
        if constexpr (Features::display)
            display_.emplace(profiles::cyd_ili9341_320x240());
        if constexpr (Features::touch)
            touch_.emplace(profiles::cyd_xpt2046());
    }

    static constexpr brick::interfaces::board::BoardDescriptor descriptor_static()
    {
        using brick::interfaces::board::Capability;
        return {"CYD 2.8-inch", "ESP32-WROOM",
                (Features::display ? static_cast<std::uint32_t>(Capability::display) : 0U) |
                    (Features::touch ? static_cast<std::uint32_t>(Capability::touchscreen) : 0U) |
                    (Features::backlight ? static_cast<std::uint32_t>(Capability::backlight) : 0U)};
    }

    brick::interfaces::board::BoardDescriptor descriptor() const override { return descriptor_static(); }

    bool begin() override
    {
        bool ok = true;
        if constexpr (Features::display)
            ok = display_.begin() && ok;
        if constexpr (Features::backlight)
        {
            gpio_set_direction(GPIO_NUM_21, GPIO_MODE_OUTPUT);
            gpio_set_level(GPIO_NUM_21, 1);
        }
        if constexpr (Features::touch)
            ok = touch_.begin() && ok;
        return ok;
    }

    brick::interfaces::display::IDisplayDevice* display_device() override
    {
        if constexpr (Features::display)
            return &display_.get();
        return nullptr;
    }

    brick::interfaces::display::ITouchscreen* touchscreen() override
    {
        if constexpr (Features::touch)
            return &touch_.get();
        return nullptr;
    }

    Ili9341SpiDisplay& display() { return display_.get(); }
    touch::Xpt2046Touchscreen& touch() { return touch_.get(); }
    brick::interfaces::time::ITimeProvider& time() { return time_; }
    brick::interfaces::logging::ILogger& logger() { return logger_.get(); }

  private:
    FreeRtosTime time_;
    brick::boards::esp32::detail::Logger<Features::logging> logger_;
    brick::boards::esp32::detail::Peripheral<Features::display, Ili9341SpiDisplay> display_;
    brick::boards::esp32::detail::Peripheral<Features::touch, touch::Xpt2046Touchscreen> touch_;
};

using CydBoard = CydBoardTemplate<>;

} // namespace brick::platform::esp32
