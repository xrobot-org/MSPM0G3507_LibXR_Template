#include <cstdint>

#include "mspm0_gpio.hpp"
#include "mspm0_timebase.hpp"
#include "thread.hpp"
#include "ti_msp_dl_config.h"

constexpr uint32_t DELAY_MS = 125U;

extern "C" void app_main(void) {
  using LibXR::GPIO;
  using LibXR::MSPM0GPIO;
  using LibXR::MSPM0Timebase;
  using LibXR::Thread;

  MSPM0Timebase timebase;
  (void)timebase;
  MSPM0Timebase::Sync(0U);

  MSPM0GPIO led(GPIO_GRP_0_PORT, GPIO_GRP_0_PIN_22_PIN,
                GPIO_GRP_0_PIN_22_IOMUX);
  (void)led.SetConfig({GPIO::Direction::OUTPUT_PUSH_PULL, GPIO::Pull::NONE});

  while (true) {
    led.Write(true);
    Thread::Sleep(DELAY_MS);

    led.Write(false);
    Thread::Sleep(DELAY_MS);
  }
}
