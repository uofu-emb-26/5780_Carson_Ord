#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

void Error_Handler(void);
void configure_GPIO_AFR(void);
void initialize_USART(void);
void transmit_char(char character);
void transmit_string(char* string);
void toggle_LED(void);
void command_parser(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
