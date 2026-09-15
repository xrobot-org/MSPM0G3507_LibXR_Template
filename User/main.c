#include "app_main.h"
#include "ti_msp_dl_config.h"

int main(void)
{
  SYSCFG_DL_init();

  NVIC_SetPriority(SysTick_IRQn, 0U);
  NVIC_SetPriority(UART_0_INST_INT_IRQN, 1U);
  DL_SYSTICK_init(CPUCLK_FREQ / 1000U);
  DL_SYSTICK_enableInterrupt();
  DL_SYSTICK_enable();

  app_main();

  return 0;
}
