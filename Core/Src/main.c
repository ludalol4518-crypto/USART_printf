/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif
int dir = 0;
int LF(int dir){
      switch(dir){
         case 1:
            HAL_GPIO_WritePin(LFF_GPIO_Port, LFF_Pin, 1);
            HAL_GPIO_WritePin(LFB_GPIO_Port, LFB_Pin, 0);
         break;

         case 0:
            HAL_GPIO_WritePin(LFF_GPIO_Port, LFF_Pin, 0);
            HAL_GPIO_WritePin(LFB_GPIO_Port, LFB_Pin, 1);
         break;

         case 2:
            HAL_GPIO_WritePin(LFF_GPIO_Port, LFF_Pin, 0);
            HAL_GPIO_WritePin(LFB_GPIO_Port, LFB_Pin, 0);
         break;
   }
      return 0;
}
int LB(int dir){
      switch(dir){
         case 1:
            HAL_GPIO_WritePin(LBF_GPIO_Port, LBF_Pin, 1);
            HAL_GPIO_WritePin(LBB_GPIO_Port, LBB_Pin, 0);
         break;

         case 0:
            HAL_GPIO_WritePin(LBF_GPIO_Port, LBF_Pin, 0);
            HAL_GPIO_WritePin(LBB_GPIO_Port, LBB_Pin, 1);
         break;

         case 2:
            HAL_GPIO_WritePin(LBF_GPIO_Port, LBF_Pin, 0);
            HAL_GPIO_WritePin(LBB_GPIO_Port, LBB_Pin, 0);
         break;
   }
      return 0;
}
int RF(int dir){
      switch(dir){
         case 1:
            HAL_GPIO_WritePin(RFF_GPIO_Port, RFF_Pin, 1);
            HAL_GPIO_WritePin(RFB_GPIO_Port, RFB_Pin, 0);
         break;

         case 0:
            HAL_GPIO_WritePin(RFF_GPIO_Port, RFF_Pin, 0);
            HAL_GPIO_WritePin(RFB_GPIO_Port, RFB_Pin, 1);
         break;

         case 2:
            HAL_GPIO_WritePin(RFF_GPIO_Port, RFF_Pin, 0);
            HAL_GPIO_WritePin(RFB_GPIO_Port, RFB_Pin, 0);
         break;
   }
      return 0;
}
int RB(int dir){
      switch(dir){
         case 1:
            HAL_GPIO_WritePin(RBF_GPIO_Port, RBF_Pin, 1);
            HAL_GPIO_WritePin(RBB_GPIO_Port, RBB_Pin, 0);
         break;

         case 0:
            HAL_GPIO_WritePin(RBF_GPIO_Port, RBF_Pin, 0);
            HAL_GPIO_WritePin(RBB_GPIO_Port, RBB_Pin, 1);
         break;

         case 2:
            HAL_GPIO_WritePin(RBF_GPIO_Port, RBF_Pin, 0);
            HAL_GPIO_WritePin(RBB_GPIO_Port, RBB_Pin, 0);
         break;
   }
      return 0;
}

int TLF(){
   RF(1); RB(1); LF(2); LB(2);
   return 0;
}
int TRF(){
   RF(2); RB(2); LF(1); LB(1);
   return 0;
}
int TLB(){
   RF(0); RB(0); LF(2); LB(2);
   return 0;
}
int TRB(){
   RF(2); RB(2); LF(0); LB(0);
   return 0;
}
   void MF()
   {
      HAL_GPIO_WritePin(LFF_GPIO_Port, LFF_Pin, 1);
      HAL_GPIO_WritePin(LFB_GPIO_Port, LFB_Pin, 0);
      HAL_GPIO_WritePin(RFF_GPIO_Port, RFF_Pin, 1);
      HAL_GPIO_WritePin(RFB_GPIO_Port, RFB_Pin, 0);
       HAL_GPIO_WritePin(LBF_GPIO_Port, LBF_Pin, 1);
      HAL_GPIO_WritePin(LBB_GPIO_Port, LBB_Pin, 0);
      HAL_GPIO_WritePin(RBF_GPIO_Port, RBF_Pin, 1);
      HAL_GPIO_WritePin(RBB_GPIO_Port, RBB_Pin, 0);
   }

   void MB()
   {
      HAL_GPIO_WritePin(LFF_GPIO_Port, LFF_Pin, 0);
      HAL_GPIO_WritePin(LFB_GPIO_Port, LFB_Pin, 1);
      HAL_GPIO_WritePin(RFF_GPIO_Port, RFF_Pin, 0);
      HAL_GPIO_WritePin(RFB_GPIO_Port, RFB_Pin, 1);
      HAL_GPIO_WritePin(LBF_GPIO_Port, LBF_Pin, 0);
      HAL_GPIO_WritePin(LBB_GPIO_Port, LBB_Pin, 1);
      HAL_GPIO_WritePin(RBF_GPIO_Port, RBF_Pin, 0);
      HAL_GPIO_WritePin(RBB_GPIO_Port, RBB_Pin, 1);
   }
   void ML()
   {
      HAL_GPIO_WritePin(LFF_GPIO_Port, LFF_Pin, 0);
      HAL_GPIO_WritePin(LFB_GPIO_Port, LFB_Pin, 0);
      HAL_GPIO_WritePin(RFF_GPIO_Port, RFF_Pin, 1);
      HAL_GPIO_WritePin(RFB_GPIO_Port, RFB_Pin, 0);
      HAL_GPIO_WritePin(LBF_GPIO_Port, LBF_Pin, 0);
      HAL_GPIO_WritePin(LBB_GPIO_Port, LBB_Pin, 0);
      HAL_GPIO_WritePin(RBF_GPIO_Port, RBF_Pin, 1);
      HAL_GPIO_WritePin(RBB_GPIO_Port, RBB_Pin, 0);
   }
   void MR()
   {
      HAL_GPIO_WritePin(LFF_GPIO_Port, LFF_Pin, 1);
      HAL_GPIO_WritePin(LFB_GPIO_Port, LFB_Pin, 0);
      HAL_GPIO_WritePin(RFF_GPIO_Port, RFF_Pin, 0);
      HAL_GPIO_WritePin(RFB_GPIO_Port, RFB_Pin, 0);
      HAL_GPIO_WritePin(LBF_GPIO_Port, LBF_Pin, 1);
      HAL_GPIO_WritePin(LBB_GPIO_Port, LBB_Pin, 0);
      HAL_GPIO_WritePin(RBF_GPIO_Port, RBF_Pin, 0);
      HAL_GPIO_WritePin(RBB_GPIO_Port, RBB_Pin, 0);
   }
   void ST()
   {
      HAL_GPIO_WritePin(LFF_GPIO_Port, LFF_Pin, 0);
      HAL_GPIO_WritePin(LFB_GPIO_Port, LFB_Pin, 0);
      HAL_GPIO_WritePin(RFF_GPIO_Port, RFF_Pin, 0);
      HAL_GPIO_WritePin(RFB_GPIO_Port, RFB_Pin, 0);
      HAL_GPIO_WritePin(LBF_GPIO_Port, LBF_Pin, 0);
      HAL_GPIO_WritePin(LBB_GPIO_Port, LBB_Pin, 0);
      HAL_GPIO_WritePin(RBF_GPIO_Port, RBF_Pin, 0);
      HAL_GPIO_WritePin(RBB_GPIO_Port, RBB_Pin, 0);
   }
PUTCHAR_PROTOTYPE
{
if (ch == '\n')
HAL_UART_Transmit(&huart2, (uint8_t*)"\r", 1, HAL_MAX_DELAY);
HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
return ch;
}

GETCHAR_PROTOTYPE
{
uint8_t ch;
HAL_UART_Receive(&huart2, &ch, 1, HAL_MAX_DELAY);
return ch;
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//#ifdef __GNUC__
//#define PUTCHAR_PROTOTYPE int _io__putchar(int ch)
//#else
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif
//
//PUTCHAR_PROTOTYPE
//{
//   if (ch == '\n')
//      HAL_UART_Transmit (&huart2, (uint8_t*) "\r", 1, 0xFFFF);
//   HAL_UART_Transmit(&huart2, (uint8_t*) &ch, 1, 0xFFFF);
//
//   return ch;
//}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

   /* USER CODE BEGIN 1 */
   uint8_t SEN1 = 0;
   uint8_t SEN2 = 0;
   uint8_t mode = 0;       // 0: 수동, 1: 1번위치, 2: 2번위치, 3: 3번위치
   uint8_t seq_state = 0;  // 시퀀스 진행 단계
   uint8_t returning = 0;  // 0: 전진 중, 1: 복귀 중
   /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  printf("\n=== WASD 입력 테스트 ===\n");
  printf("W: 위, A: 왼쪽, S: 아래, D: 오른쪽\n");
  printf("q: 종료\n\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t rx_data;
  HAL_StatusTypeDef status;
//  while (1)
//  {
//     printf("방향 입력: ");
//
//  // 타임아웃을 1초로 설정하여 테스트
//  status = HAL_UART_Receive(&huart2, &rx_data, 1, 1000);
//  SEN1 = HAL_GPIO_ReadPin(LT1_GPIO_Port, LT1_Pin);
//  SEN2 = HAL_GPIO_ReadPin(LT2_GPIO_Port, LT2_Pin);
//  printf("L1=%d,L2=%d",SEN1,SEN2);
//
//  if (status == HAL_OK)
//  {
//     printf("%c\n", rx_data);
//     printf("수신 성공! 값: 0x%02X ('%c')\n", rx_data, rx_data);
//
//
////     //if문을 사용한 제어
////     if (rx_data == 'w')
////        MF(]);
////     else if (rx_data == 'a')
////        ML();
////     else if (rx_data == 'd')
////        MR();
////     else if (rx_data == 's')
////        MB();
////     else
////        ST();
//     switch (rx_data){
//     case '1':
//        printf("1\n");
//          if(SEN1 == 1 && SEN2 == 1) MF();
//          else if(SEN1 == 0 && SEN2 == 0) MF();
//          else if(SEN1 == 1 && SEN2 == 0) ST();
//          else ST();
//        break;
//
//     case 'O':
//        printf("1\n");
//        if(SEN1 == '0' && SEN2 == '0') {MF();}
//        else if(SEN1 == '1' && SEN2 == '1') {MF();}
//        else if(SEN1 == '1' && SEN2 == '0') {MF();}
//        else if(SEN1 == '0' && SEN2 == '1') {ST();}
//        else {ST();}
//        break;
//
//     case 'P':
//        printf("1\n");
//           if(SEN1 == '0' && SEN2 == '0') {MF();}
//           else if(SEN1 == '1' && SEN2 == '1') {MF();}
//           else if(SEN1 == '1' && SEN2 == '0') {MF();}
//           else if(SEN1 == '0' && SEN2 == '1') {MF();}
//           else if(SEN1 == '0' && SEN2 == '0') {ST();}
//           else {ST();}
//        break;
//
//     case 'W':
//        //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
//        MF();
//        //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
//        break;
//     case 'A':
//       //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
//        ML();
//        //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
//        break;
//     case 'D':
//       //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
//        MR();
//        //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
//        break;
//     case 'S':
//        //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
//        MB();
//        //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
//        break;
//     case 'X':
//        ST();
//        break;
//     case 'q':
//        //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
//        TLF();
//        //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
//        break;
//     case 'e':
//        //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
//        TRF();
//        //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
//        break;
//     case 'z':
//        //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
//        TLB();
//        //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
//        break;
//     case 'c':
//        //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
//        TRB();
//        //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
//        break;
//     default:
//          //ST();
//          break;}
//     }
//
//  else if (status == HAL_TIMEOUT)
//  {
//     printf("\n타임아웃 - 입력 없음\n");
//     //ST();
//  }
//  else
//  {
//     printf("\n에러 발생: %d\n", status);
//
//}
//  HAL_Delay(50);
//     /*printf ("Hello World!\n");
//     HAL_Delay(1000);*/
//    /* USER CODE END WHILE */
//
//    /* USER CODE BEGIN 3 */
//  }
  while (1)
  {
      // 센서 읽기
      SEN1 = HAL_GPIO_ReadPin(LT1_GPIO_Port, LT1_Pin);
      SEN2 = HAL_GPIO_ReadPin(LT2_GPIO_Port, LT2_Pin);

      // 키 입력 체크 (Non-blocking)
      status = HAL_UART_Receive(&huart2, &rx_data, 1, 10);

      if (status == HAL_OK)
      {
          printf("입력: %c\n", rx_data);

          switch (rx_data)
          {
              case 'I':  // 1번 위치로 이동
                  mode = 1;
                  seq_state = 0;
                  returning = 0;
                  printf("1번 위치로 이동 시작\n");
                  break;
              case 'O':  // 2번 위치로 이동
                  mode = 2;
                  seq_state = 0;
                  returning = 0;
                  printf("2번 위치로 이동 시작\n");
                  break;
              case 'P':  // 3번 위치로 이동
                  mode = 3;
                  seq_state = 0;
                  returning = 0;
                  printf("3번 위치로 이동 시작\n");
                  break;

              // 수동 조작 (모드 해제)
              case 'W': mode = 0; returning = 0; MF(); break;
              case 'A': mode = 0; returning = 0; ML(); break;
              case 'D': mode = 0; returning = 0; MR(); break;
              case 'S': mode = 0; returning = 0; MB(); break;
              case 'X': mode = 0; returning = 0; ST(); break;
              case 'q': mode = 0; returning = 0; TLF(); break;
              case 'e': mode = 0; returning = 0; TRF(); break;
              case 'z': mode = 0; returning = 0; TLB(); break;
              case 'c': mode = 0; returning = 0; TRB(); break;
              default: break;
          }
      }

      // === 자동 모드 ===
      if (mode != 0)
      {
          if (returning == 0)
          {
              // ===== 전진: 목표 위치로 이동 =====
              MF();

              switch (seq_state)
              {
                  case 0:  // (1,1) → (0,0) 기다림
                      if (SEN1 == 0 && SEN2 == 0) {
                          seq_state = 1;
                          printf("전진 시퀀스 1: (0,0) 진입\n");
                      }
                      break;

                  case 1:  // (0,0) → (1,0) 기다림
                      if (SEN1 == 1 && SEN2 == 0) {
                          if (mode == 1) {  // 1번 위치 도착
                              ST();
                              printf("1번 위치 도착! 3초 후 복귀\n");
                              HAL_Delay(3000);
                              returning = 1;
                              seq_state = 0;
                          } else {
                              seq_state = 2;
                              printf("전진 시퀀스 2: (1,0) 진입\n");
                          }
                      }
                      break;

                  case 2:  // (1,0) → (0,1) 기다림
                      if (SEN1 == 0 && SEN2 == 1) {
                          if (mode == 2) {  // 2번 위치 도착
                              ST();
                              printf("2번 위치 도착! 3초 후 복귀\n");
                              HAL_Delay(3000);
                              returning = 1;
                              seq_state = 0;
                          } else {
                              seq_state = 3;
                              printf("전진 시퀀스 3: (0,1) 진입\n");
                          }
                      }
                      break;

                  case 3:  // (0,1) → (1,1) 기다림
                      if (SEN1 == 1 && SEN2 == 1) {
                          if (mode == 3) {  // 3번 위치 도착
                              ST();
                              printf("3번 위치 도착! 3초 후 복귀\n");
                              HAL_Delay(3000);
                              returning = 1;
                              seq_state = 0;
                          }
                      }
                      break;
              }
          }
          else
          {
              // ===== 후진: 초기 위치로 복귀 =====
              MB();

              // 복귀 시퀀스 (역순)
              // 1번에서 복귀: (1,0) → (0,0) → (1,1)
              // 2번에서 복귀: (0,1) → (1,0) → (0,0) → (1,1)
              // 3번에서 복귀: (1,1) → (0,1) → (1,0) → (0,0) → (1,1)

              switch (mode)
              {
                  case 1:  // 1번 위치에서 복귀
                      switch (seq_state)
                      {
                          case 0:  // (1,0) → (0,0) 기다림
                              if (SEN1 == 0 && SEN2 == 0) {
                                  seq_state = 1;
                                  printf("복귀 1: (0,0) 진입\n");
                              }
                              break;
                          case 1:  // (0,0) → (1,1) 기다림
                              if (SEN1 == 1 && SEN2 == 1) {
                                  ST();
                                  mode = 0;
                                  returning = 0;
                                  printf("초기 위치 복귀 완료!\n");
                              }
                              break;
                      }
                      break;

                  case 2:  // 2번 위치에서 복귀
                      switch (seq_state)
                      {
                          case 0:  // (0,1) → (1,0) 기다림
                              if (SEN1 == 1 && SEN2 == 0) {
                                  seq_state = 1;
                                  printf("복귀 1: (1,0) 진입\n");
                              }
                              break;
                          case 1:  // (1,0) → (0,0) 기다림
                              if (SEN1 == 0 && SEN2 == 0) {
                                  seq_state = 2;
                                  printf("복귀 2: (0,0) 진입\n");
                              }
                              break;
                          case 2:  // (0,0) → (1,1) 기다림
                              if (SEN1 == 1 && SEN2 == 1) {
                                  ST();
                                  mode = 0;
                                  returning = 0;
                                  printf("초기 위치 복귀 완료!\n");
                              }
                              break;
                      }
                      break;

                  case 3:  // 3번 위치에서 복귀
                      switch (seq_state)
                      {
                          case 0:  // (1,1) → (0,1) 기다림
                              if (SEN1 == 0 && SEN2 == 1) {
                                  seq_state = 1;
                                  printf("복귀 1: (0,1) 진입\n");
                              }
                              break;
                          case 1:  // (0,1) → (1,0) 기다림
                              if (SEN1 == 1 && SEN2 == 0) {
                                  seq_state = 2;
                                  printf("복귀 2: (1,0) 진입\n");
                              }
                              break;
                          case 2:  // (1,0) → (0,0) 기다림
                              if (SEN1 == 0 && SEN2 == 0) {
                                  seq_state = 3;
                                  printf("복귀 3: (0,0) 진입\n");
                              }
                              break;
                          case 3:  // (0,0) → (1,1) 기다림
                              if (SEN1 == 1 && SEN2 == 1) {
                                  ST();
                                  mode = 0;
                                  returning = 0;
                                  printf("초기 위치 복귀 완료!\n");
                              }
                              break;
                      }
                      break;
              }
          }

          printf("mode=%d, ret=%d, seq=%d, SEN=(%d,%d)\n",
                 mode, returning, seq_state, SEN1, SEN2);
      }
      printf("mode=%d, ret=%d, seq=%d, SEN=(%d,%d)\n",
             mode, returning, seq_state, SEN1, SEN2);

      HAL_Delay(50);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|LBF_Pin|LFB_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LBB_Pin|LFF_Pin|RFF_Pin|RFB_Pin
                          |RBF_Pin|RBB_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin LBF_Pin LFB_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|LBF_Pin|LFB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LBB_Pin LFF_Pin RFF_Pin RFB_Pin
                           RBF_Pin RBB_Pin */
  GPIO_InitStruct.Pin = LBB_Pin|LFF_Pin|RFF_Pin|RFB_Pin
                          |RBF_Pin|RBB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LT1_Pin LT2_Pin */
  GPIO_InitStruct.Pin = LT1_Pin|LT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
