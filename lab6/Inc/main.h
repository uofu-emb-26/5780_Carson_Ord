#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

void Error_Handler(void);
void Init_GPIO(void);
void Init_ADC(void);
void Calibrate_ADC(void);
void Pot_LED_CTRL(void);
void Waveform_Gen(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
