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
#include <stdio.h>
#include "project.h"

unsigned int pos = 0;

CY_ISR(isr_rotary_vec)
{
    if (Pin_rotary_2_Read())
        pos++;
    else
        pos--;
}

int main(void)
{
    char temp[64];
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    isr_rotary_StartEx(isr_rotary_vec);
    
    UART_1_Start();

    for(;;)
    {
        sprintf(temp, "%d\n\r", pos);
        UART_1_PutString(temp);
    }
}

/* [] END OF FILE */
