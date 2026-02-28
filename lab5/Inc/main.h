#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void Error_Handler(void);
void Init_GPIO(void);
void Init_I2C(void);
void Setup_I2C_Transaction(void);
void I2C_Reg_Write(uint8_t reg_addr, uint8_t data);
void I2C_Reg_Read(uint8_t reg_addr, uint8_t data);
void I2C_X_Y_Axis_Read();
void Determine_Active_LED();

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
