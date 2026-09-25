/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Smart Parking System using STM32 HAL (Pure C)
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim2;

#define LCD_ADDR (0x27 << 1) // I2C address shifted for HAL

/* Function Prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);

void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_Init(void);
void LCD_Print(char *str);
void LCD_SetCursor(uint8_t row, uint8_t col);
void Set_Servo_Angle(uint8_t angle);

/* --- LCD Helper Functions (I2C) --- */
void LCD_Send(uint8_t data, uint8_t flags) {
    uint8_t up = data & 0xF0;
    uint8_t lo = (data << 4) & 0xF0;
    uint8_t data_arr[4];
    data_arr[0] = up | flags | 0x08 | 0x04; // En=1, Rs=flags, Backlight=1
    data_arr[1] = up | flags | 0x08;        // En=0
    data_arr[2] = lo | flags | 0x08 | 0x04; // En=1
    data_arr[3] = lo | flags | 0x08;        // En=0
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data_arr, 4, 100);
}

void LCD_SendCommand(uint8_t cmd) {
    LCD_Send(cmd, 0);
}

void LCD_SendData(uint8_t data) {
    LCD_Send(data, 1);
}

void LCD_Init(void) {
    HAL_Delay(50);
    LCD_SendCommand(0x30);
    HAL_Delay(5);
    LCD_SendCommand(0x30);
    HAL_Delay(1);
    LCD_SendCommand(0x32);
    LCD_SendCommand(0x28); // 4-bit, 2-line, 5x8 dots
    LCD_SendCommand(0x0C); // Display ON, Cursor OFF
    LCD_SendCommand(0x06); // Entry mode set
    LCD_SendCommand(0x01); // Clear display
    HAL_Delay(2);
}

void LCD_Print(char *str) {
    while (*str) LCD_SendData((uint8_t)(*str++));
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? (0x80 + col) : (0xC0 + col);
    LCD_SendCommand(address);
}

/* --- Servo PWM Function --- */
void Set_Servo_Angle(uint8_t angle) {
    // 50Hz PWM signal: Maps 0-90 degrees to timer pulse values
    // Pulse value calculation depends on ARR and Prescaler (e.g., 500 = 0 deg, 1500 = 90 deg)
    uint32_t pulse = 500 + ((angle * 1000) / 90);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pulse);
}

/* --- Main Application Logic --- */
int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_TIM2_Init();

    // Start PWM for Servo Motor
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    Set_Servo_Angle(0); // Barrier closed

    // Initialize LCD
    LCD_Init();
    LCD_SetCursor(0, 0);
    LCD_Print(" Smart Parking ");
    LCD_SetCursor(1, 0);
    LCD_Print("   System C    ");
    HAL_Delay(2000);
    LCD_SendCommand(0x01); // Clear screen

    int availableSlots = 2;

    while (1) {
        // Read IR sensors: Active LOW (0 = Vehicle present, 1 = Free)
        uint8_t slot1 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
        uint8_t slot2 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
        uint8_t entryGate = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);

        // Count available slots
        availableSlots = 0;
        if (slot1 == GPIO_PIN_SET) availableSlots++;
        if (slot2 == GPIO_PIN_SET) availableSlots++;

        // Update LCD Display
        LCD_SetCursor(0, 0);
        if (availableSlots == 2) LCD_Print("Available: 2   ");
        else if (availableSlots == 1) LCD_Print("Available: 1   ");
        else LCD_Print("Available: 0   ");

        LCD_SetCursor(1, 0);
        LCD_Print((slot1 == GPIO_PIN_RESET) ? "S1:FULL " : "S1:FREE ");
        LCD_Print((slot2 == GPIO_PIN_RESET) ? "S2:FULL" : "S2:FREE");

        // Gate Logic
        if (entryGate == GPIO_PIN_RESET) { // Car detected at entrance
            if (availableSlots > 0) {
                LCD_SendCommand(0x01);
                LCD_SetCursor(0, 0);
                LCD_Print("Welcome!");
                LCD_SetCursor(1, 0);
                LCD_Print("Gate Opening...");

                Set_Servo_Angle(90); // Open gate
                HAL_Delay(3000);     // Wait for vehicle to pass

                Set_Servo_Angle(0);  // Close gate
                LCD_SendCommand(0x01);
            } else {
                LCD_SendCommand(0x01);
                LCD_SetCursor(0, 0);
                LCD_Print("Parking Full!");
                LCD_SetCursor(1, 0);
                LCD_Print("Please Wait.");
                HAL_Delay(2000);
                LCD_SendCommand(0x01);
            }
        }

        HAL_Delay(200);
    }
}
