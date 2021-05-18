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
#include "rotary_encoder.h"

int main(void)
{
    char temp[64];
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    UART_1_Start();
    InitRotary();

    for(;;)
    {
        sprintf(temp, "%d\n\r", GetRotaryPos());
        UART_1_PutString(temp);
    }
}

/* [] END OF FILE */
