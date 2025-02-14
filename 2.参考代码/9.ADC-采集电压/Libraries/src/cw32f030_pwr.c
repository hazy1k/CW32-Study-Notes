/**
 * @file cw32f030_pwr.c
 * @author P&S (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-08-09
 *
 * @copyright Copyright (c) 2021
 *
 */
/*******************************************************************************
*
* 代码许可和免责信息
* 武汉力源半导体有限公司授予您使用所有编程代码示例的非专属的版权许可，您可以由此
* 生成根据您的特定需要而定制的相似功能。根据不能被排除的任何法定保证，武汉力源半
* 导体有限公司及其程序开发商和供应商对程序或技术支持（如果有）不提供任何明示或暗
* 含的保证或条件，包括但不限于暗含的有关适销性、适用于某种特定用途和非侵权的保证
* 或条件。
* 无论何种情形，武汉力源半导体有限公司及其程序开发商或供应商均不对下列各项负责，
* 即使被告知其发生的可能性时，也是如此：数据的丢失或损坏；直接的、特别的、附带的
* 或间接的损害，或任何后果性经济损害；或利润、业务、收入、商誉或预期可节省金额的
* 损失。
* 某些司法辖区不允许对直接的、附带的或后果性的损害有任何的排除或限制，因此某些或
* 全部上述排除或限制可能并不适用于您。
*
*******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "cw32f030.h"
#include "cw32f030_rcc.h"
#include "cw32f030_pwr.h"

/**
 *******************************************************************************
 ** \addtogroup LpmGroup
 ******************************************************************************/
//@{

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/*
#define IS_VALID_SEVONPEND(x)           (SevPndDisable == (x) ||\
                                         SevPndEnable  == (x))
#define IS_VALID_SLEEPDEEP(x)           (SlpDpDisable  == (x) ||\
                                         SlpDpEnable   == (x))
#define IS_VALID_SLEEPONEXIT(x)         (SlpExtDisable == (x) ||\
                                         SlpExtEnable  == (x))                                         
*/

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief 低功耗模式配置
 *
 * @param PWR_InitStruct      低功耗模式配置结构体指针
 *
 * @return
 */
void PWR_Config(PWR_InitTypeDef* PWR_InitStruct)
{
    ASSERT(NULL != PWR_InitStruct);
    assert_param(IS_PWR_SEVONPEND(PWR_InitStruct->PWR_Sevonpend));
    assert_param(IS_PWR_SLEEPDEEP(PWR_InitStruct->PWR_SleepDeep));
    assert_param(IS_PWR_SLEEPONEXIT(PWR_InitStruct->PWR_SleepOnExit));

    REGBITS_MODIFY(SCB->SCR, SCB_SCR_SEVONPEND_Msk | \
					SCB_SCR_SLEEPDEEP_Msk | \
					SCB_SCR_SLEEPONEXIT_Msk, \
					PWR_InitStruct->PWR_Sevonpend | \
					PWR_InitStruct->PWR_SleepDeep | \
					PWR_InitStruct->PWR_SleepOnExit);
}

/**
 * @brief 进入睡眠模式
 *       注：1.进入睡眠模式前，HCLK的时钟频率需要小于或等于4MHz
 *           2.需等待VCx_ISR.Ready标志置1后，才可进入睡眠模式。
 * @return
 */
void PWR_GotoLpmMode(void)
{
    static uint32_t RegSavedCR0, RegSavedCR1;
    static uint32_t RegSavedHSI;

    RegSavedCR0 = CW_SYSCTRL->CR0;
    RegSavedCR1 = CW_SYSCTRL->CR1;   
    RegSavedHSI = CW_SYSCTRL->HSI;      

    if (CW_SYSCTRL->APBEN2 & RCC_APB2_PERIPH_VC)
    {
        if (CW_VC1->CR0_f.EN)
        {
            while (!CW_VC1->SR_f.READY);
        }

        if (CW_VC2->CR0_f.EN)
        {
            while (!CW_VC2->SR_f.READY);
        }        
    }

    RCC_HSI_Enable(RCC_HSIOSC_DIV12);
    RCC_SysClk_Switch(RCC_SYSCLKSRC_HSI);
    __WFI();

    CW_SYSCTRL->CR1 = (RegSavedCR1 & (~SYSCTRL_BYPASS_MASK)) | SYSCTRL_BYPASS;
    CW_SYSCTRL->HSI = RegSavedHSI;
    for (uint32_t i; i < 30000; i++);
    CW_SYSCTRL->CR0 = (RegSavedCR0 & (~SYSCTRL_BYPASS_MASK)) | SYSCTRL_BYPASS;
}
                        
//@} // LpmGroup                                                                           

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
