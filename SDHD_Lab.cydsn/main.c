/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"

CY_ISR_PROTO(isr_button);

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    UART_LOG_Start();
   
    //WaveDAC_Start();
    WaveDAC_Start();
   
       
    uint16_t ADC_SamplesArray[1024] = {0};
    
    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
