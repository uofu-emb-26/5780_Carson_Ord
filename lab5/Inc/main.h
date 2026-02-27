#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

void Error_Handler(void);
void Init_GPIO(void);
void Init_I2C(void);
void Setup_I2C_Transaction(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
