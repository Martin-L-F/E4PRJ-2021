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
#include "rotary_encoder.h"

unsigned int _pos = 0;

CY_ISR(isr_rotary_1_vec)
{
    _pos++;
}

CY_ISR(isr_rotary_2_vec)
{
    _pos--;
}

void InitRotary()
{
    isr_rotary_1_StartEx(isr_rotary_1_vec);
    isr_rotary_2_StartEx(isr_rotary_2_vec);
}

unsigned int GetRotaryPos()
{
    return _pos;
}

/* [] END OF FILE */
