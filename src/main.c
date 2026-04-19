#include "ti_msp_dl_config.h"

#ifdef __cplusplus
extern "C" {
#endif
void app_main(void);
#ifdef __cplusplus
}
#endif

int main(void)
{
  SYSCFG_DL_init();
  DL_SYSTICK_init(CPUCLK_FREQ / 1000U);
  DL_SYSTICK_disableInterrupt();
  DL_SYSTICK_enable();
  app_main();
  while (1)
  {
  }
}
