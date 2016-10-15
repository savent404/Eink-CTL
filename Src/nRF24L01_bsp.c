#include "stm32f1xx_hal.h"
extern SPI_HandleTypeDef hspi1;
void Delay_ms(int ms){
    HAL_Delay(ms);
}
void nRF24L01_CEN_opra(unsigned char set_1_reset_0){
    if(set_1_reset_0)
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_RESET);
}
void nRF24L01_CE_opra(unsigned char set_1_reset_0){
    if(set_1_reset_0)
				HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_RESET);
}
unsigned char nRF24L01_IRQ_read(void){
    return HAL_GPIO_ReadPin(NRF_IRQ_GPIO_Port, NRF_IRQ_Pin);
}
unsigned char nRF24L01_opra_IO(unsigned char val){
    unsigned char tx[1],rx[1];
    tx[0] = val;
    HAL_SPI_TransmitReceive(&hspi1, tx, rx, 1, 10);
    return *rx;
}
