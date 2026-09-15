#include "app_main.h"

#include <atomic>

#include "mspm0_gpio.hpp"
#include "mspm0_timebase.hpp"
#include "mspm0_uart.hpp"
#include "thread.hpp"
#include "ti_msp_dl_config.h"

extern "C" void app_main()
{
  LibXR::MSPM0Timebase timebase;
  UNUSED(timebase);

  LibXR::MSPM0GPIO led_gpio(GPIO_GRP_0_PORT, GPIO_GRP_0_PIN_0_PIN,
                            GPIO_GRP_0_PIN_0_IOMUX);
  (void)led_gpio.SetConfig(
      {LibXR::GPIO::Direction::OUTPUT_PUSH_PULL, LibXR::GPIO::Pull::NONE});

  // DMA channel 0 belongs exclusively to UART_0 TX; Main RX uses byte interrupts.
  alignas(size_t) static uint8_t uart_tx_dma_buffer[1024];
  static LibXR::MSPM0UART uart(
      {UART_0_INST, UART_0_INST_INT_IRQN, UART_0_INST_FREQUENCY,
       LibXR::MSPM0UART::ResolveIndex(UART_0_INST_INT_IRQN),
       LibXR::MSPM0UART::RxMode::MAIN_BYTE_IRQ, false, 0U, DMA_UART0_TX_TRIG,
       LibXR::MSPM0UART::INVALID_DMA_CHANNEL, 0U},
      uart_tx_dma_buffer, {}, 16, 256,
      LibXR::MSPM0UART::BuildConfigFromSysCfg(UART_0_INST, UART_0_BAUD_RATE));

  static const char heartbeat[] = "[mspm0-uart] heartbeat\r\n";
  static uint8_t rx_echo_buffer[1];

  constexpr bool ENABLE_HEARTBEAT = true;
  constexpr uint32_t LOOP_DELAY_MS = 1;
  constexpr uint32_t HEARTBEAT_PERIOD_MS = 1000;
  constexpr uint32_t LED_PERIOD_MS = 500;
  constexpr uint32_t HEARTBEAT_TICKS = HEARTBEAT_PERIOD_MS / LOOP_DELAY_MS;
  constexpr uint32_t LED_TICKS = LED_PERIOD_MS / LOOP_DELAY_MS;

  bool led_on = false;
  uint32_t tick = 0;

  using PollingStatus = LibXR::ReadOperation::OperationPollingStatus;
  std::atomic<PollingStatus> read_status{PollingStatus::READY};
  LibXR::ReadOperation read_op(read_status);
  LibXR::WriteOperation echo_write_op;
  LibXR::WriteOperation heartbeat_write_op;

  LibXR::RawData read_data = {rx_echo_buffer, sizeof(rx_echo_buffer)};

  while (true)
  {
    bool echoed_this_cycle = false;

    if (read_status.load(std::memory_order_acquire) == PollingStatus::DONE)
    {
      const size_t read_size = read_data.size_;
      // Retain the received byte until TX accepts it instead of losing it on FULL.
      if (read_size == 0 ||
          uart.Write({rx_echo_buffer, read_size}, echo_write_op) == LibXR::ErrorCode::OK)
      {
        echoed_this_cycle = true;
        read_status.store(PollingStatus::READY, std::memory_order_relaxed);
      }
    }

    if (read_status.load(std::memory_order_acquire) == PollingStatus::READY)
    {
      (void)uart.Read(read_data, read_op);
    }

    if (ENABLE_HEARTBEAT && !echoed_this_cycle &&
        read_status.load(std::memory_order_acquire) != PollingStatus::DONE &&
        ((tick % HEARTBEAT_TICKS) == 0U))
    {
      (void)uart.Write(
          {reinterpret_cast<const uint8_t*>(heartbeat), sizeof(heartbeat) - 1},
          heartbeat_write_op);
    }

    if ((tick % LED_TICKS) == 0U)
    {
      led_on = !led_on;
      (void)led_gpio.Write(led_on);
    }

    LibXR::Thread::Sleep(LOOP_DELAY_MS);
    tick++;
  }
}
