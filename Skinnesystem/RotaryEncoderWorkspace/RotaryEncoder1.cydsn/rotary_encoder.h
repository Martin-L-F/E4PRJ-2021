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
#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H
    
#include "project.h"
    

CY_ISR_PROTO(isr_rotary_1_vec);
CY_ISR_PROTO(isr_rotary_2_vec);

void InitRotary();
unsigned int GetRotaryPos();

    
#endif /* CYAPICALLBACKS_H */   
/* [] END OF FILE */
