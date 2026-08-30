#pragma once
#include "brick/boards/esp32/s3/Panel480BoardConfig.h"
#include "brick/interfaces/board/BoardDescriptor.h"
#include "brick/interfaces/board/IBoard.h"
#include "brick/interfaces/display/DisplayTypes.h"
#if BRICK_PANEL480_ENABLE_DISPLAY
#include "brick/platform/esp32/s3/St7701sRgbDisplay.h"
#include "brick/platform/esp32/s3/profiles/st7701s_480x480.h"
#endif
#if BRICK_PANEL480_ENABLE_TOUCH
#include "brick/platform/esp32/touch/Gt911Touchscreen.h"
#include "brick/platform/esp32/s3/profiles/st7701s_gt911.h"
#endif
#if BRICK_PANEL480_ENABLE_SD
#include "brick/platform/esp32/SdSpiFileSystem.h"
#endif
#include "brick/platform/esp32/FreeRtosTime.h"
#include "brick/platform/esp32/EspIdfLogger.h"
namespace brick::platform::esp32::s3 {
class Panel480Board final : public brick::interfaces::board::IBoard {
public:
    explicit Panel480Board(brick::interfaces::display::Rotation rotation = brick::interfaces::display::Rotation::rotate_0)
#if BRICK_PANEL480_ENABLE_DISPLAY
        : display_(profiles::st7701s_480x480())
#if BRICK_PANEL480_ENABLE_TOUCH
        , touch_(profiles::st7701s_gt911())
#endif
#if BRICK_PANEL480_ENABLE_SD
        , sd_(brick::platform::esp32::SdSpiFileSystemConfig{GPIO_NUM_42, GPIO_NUM_48, GPIO_NUM_47, GPIO_NUM_41})
#endif
#elif BRICK_PANEL480_ENABLE_TOUCH
        : touch_(profiles::st7701s_gt911())
#endif
    { (void)rotation; }

#if BRICK_PANEL480_ENABLE_DISPLAY
    explicit Panel480Board(St7701sRgbPanelConfig display_config
#if BRICK_PANEL480_ENABLE_TOUCH
                           , brick::platform::esp32::touch::Gt911Config touch_config = brick::platform::esp32::s3::profiles::st7701s_gt911()
#endif
                           )
        : display_(display_config)
#if BRICK_PANEL480_ENABLE_TOUCH
        , touch_(touch_config)
#endif
    {}
    static St7701sRgbPanelConfig double_buffered_config()
    {
        auto config = profiles::st7701s_480x480();
        config.pixel_clock_hz = 12'000'000;
        config.frame_buffer_count = 2;
        return config;
    }
#endif
    static constexpr brick::interfaces::board::BoardDescriptor descriptor_static() {
        using brick::interfaces::board::Capability;
        return {"480x480 panel", "ESP32-S3", (BRICK_PANEL480_ENABLE_DISPLAY ? static_cast<std::uint32_t>(Capability::display) : 0U) | (BRICK_PANEL480_ENABLE_TOUCH ? static_cast<std::uint32_t>(Capability::touchscreen) : 0U) | (BRICK_PANEL480_ENABLE_SD ? static_cast<std::uint32_t>(Capability::sd_card) : 0U)};
    }
    brick::interfaces::board::BoardDescriptor descriptor() const override { return descriptor_static(); }
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
    brick::interfaces::display::IDisplayDevice* display_device() override { return &display_; }
#endif
#if BRICK_PANEL480_ENABLE_TOUCH
    touch::Gt911Touchscreen& touch() { return touch_; }
    brick::interfaces::display::ITouchscreen* touchscreen() override { return &touch_; }
#else
    brick::interfaces::display::ITouchscreen* touchscreen() override { return nullptr; }
#endif
    brick::interfaces::time::ITimeProvider& time() { return time_; }
    brick::interfaces::logging::ILogger& logger() { return logger_; }
#if BRICK_PANEL480_ENABLE_SD
    brick::platform::esp32::SdSpiFileSystem& sd_card() { return sd_; }
#endif
#if !BRICK_PANEL480_ENABLE_DISPLAY
    brick::interfaces::display::IDisplayDevice* display_device() override { return nullptr; }
#endif
private:
    brick::platform::esp32::FreeRtosTime time_;
#if BRICK_PANEL480_ENABLE_LOGGING
    brick::platform::esp32::EspIdfLogger logger_{BRICK_PANEL480_LOG_LEVEL};
#else
    brick::platform::esp32::NullLogger logger_;
#endif
#if BRICK_PANEL480_ENABLE_DISPLAY
    St7701sRgbDisplay display_;
#endif
#if BRICK_PANEL480_ENABLE_TOUCH
    touch::Gt911Touchscreen touch_;
#endif
#if BRICK_PANEL480_ENABLE_SD
    brick::platform::esp32::SdSpiFileSystem sd_{brick::platform::esp32::SdSpiFileSystemConfig{GPIO_NUM_42, GPIO_NUM_48, GPIO_NUM_47, GPIO_NUM_41}};
#endif
};
} // namespace brick::platform::esp32::s3
