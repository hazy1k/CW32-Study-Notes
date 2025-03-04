/*
 * cw32f030_pwr_modes.h
 *
 *  Created on: 2021��6��04��
 *      Author: Socrates
 */

#ifndef _CW32F030_PWR_MODES_H
#define _CW32F030_PWR_MODES_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#if !defined (SLEEP_MODE) && !defined (DEEPSLEEP_MODE)

 /* Uncomment the corresponding line to select the CW32F030 Low Power mode
   you want to mesure  */
//#define SLEEP_MODE
//#define DEEPSLEEP_MODE
#endif

//#define SLEEP_LSE_EN

#if !defined (SLEEP_MODE) && !defined (DEEPSLEEP_MODE)
#error "Please select first the target CW32F030 Low Power mode to be measured (in cw32f030_pwr_modes.h file)"
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SleepModeTest(void);
void DeepSleepModeTest(void);

void HclkOutTest(void);
#endif /* _CW32F030_PWR_MODES_H */
