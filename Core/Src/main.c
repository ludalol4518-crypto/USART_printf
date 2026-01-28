/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 서빙 로봇 시스템 v1.4
  ******************************************************************************
  * 센서 시퀀스:
  * 홈:       (0,0) 출발
  * 테이블1:  (1,1) → (1,0) 경계에서 정지
  * 테이블2:  (1,1) → (1,0) → (0,1) 경계에서 정지
  * 테이블3:  (1,1) → (1,0) → (0,1) → (0,0) 경계에서 정지
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

   // ML과 MR 스왑됨 (좌우 방향 수정)
   void MR()
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
   void ML()
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

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
    // === 센서 변수 ===
    uint8_t SEN1 = 0;
    uint8_t SEN2 = 0;
    uint8_t SEN1_prev = 0;
    uint8_t SEN2_prev = 0;

    // === 서빙 로봇 상태 변수 ===
    // 0: 홈대기, 1: 전진중, 2: 도착-물건제거대기, 3: 복귀중, 4: 수동모드
    uint8_t system_state = 0;
    uint8_t target_table = 0;   // 목표 테이블 번호 (1, 2, 3)
    uint8_t item_sensor = 0;    // 물건 감지 센서 (1: 물건있음, 0: 없음)
    uint8_t item_sensor_prev = 0;

    // === 구간 추적 ===
    // 센서 시퀀스: (0,0) → (1,1) → (1,0) → (0,1) → (0,0)
    //             홈      구간1    구간2    구간3    구간4(테이블3)
    uint8_t zone_count = 0;     // 지나간 센서 변화 횟수

    // === UART 변수 ===
    uint8_t rx_data;
    HAL_StatusTypeDef uart_status;
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
    printf("\n========================================\n");
    printf("       서빙 로봇 시스템 v1.4\n");
    printf("========================================\n");
    printf("[테이블 선택] 1, 2, 3\n");
    printf("[물건 센서]   L: 물건올림, U: 물건내림\n");
    printf("[제어]       0: 강제복귀, X: 비상정지\n");
    printf("[수동조작]   W/A/S/D: 이동, q/e/z/c: 회전\n");
    printf("========================================\n");
    printf("센서 시퀀스:\n");
    printf("홈(0,0)→(1,1)→(1,0)T1→(0,1)T2→(0,0)T3\n");
    printf("========================================\n\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
        // ============================================
        // === 센서 읽기 ===
        // ============================================
        SEN1_prev = SEN1;
        SEN2_prev = SEN2;
        SEN1 = HAL_GPIO_ReadPin(LT1_GPIO_Port, LT1_Pin);
        SEN2 = HAL_GPIO_ReadPin(LT2_GPIO_Port, LT2_Pin);

        // === 물건 감지 센서 읽기 ===
        // 센서 극성: 물건 있음 = 0 (LOAD), 물건 없음 = 1 (UNLOAD)
        item_sensor_prev = item_sensor;
        uint8_t item_raw = HAL_GPIO_ReadPin(ITEM_GPIO_Port, ITEM_Pin);
        item_sensor = (item_raw == 0) ? 1 : 0;

        // 센서 상태 변화 시 LOAD/UNLOAD 출력
        if (item_sensor != item_sensor_prev) {
            if (item_sensor == 1) {
                printf(">>> LOAD\n");
            } else {
                printf(">>> UNLOAD\n");
            }
        }

        // ============================================
        // === 키 입력 처리 ===
        // ============================================
        uart_status = HAL_UART_Receive(&huart2, &rx_data, 1, 10);

        if (uart_status == HAL_OK)
        {
            printf("입력: %c\n", rx_data);

            switch (rx_data)
            {
                // === 테이블 목적지 설정 ===
                case '1':
                    if (system_state == 0 || system_state == 4) {
                        if (system_state == 4) {
                            ST();
                            system_state = 0;
                            printf(">>> 자동 모드로 전환\n");
                        }
                        target_table = 1;
                        zone_count = 0;
                        printf(">>> 목적지: 테이블 1 설정\n");
                    } else {
                        printf(">>> 이동 중에는 목적지 변경 불가\n");
                    }
                    break;

                case '2':
                    if (system_state == 0 || system_state == 4) {
                        if (system_state == 4) {
                            ST();
                            system_state = 0;
                            printf(">>> 자동 모드로 전환\n");
                        }
                        target_table = 2;
                        zone_count = 0;
                        printf(">>> 목적지: 테이블 2 설정\n");
                    } else {
                        printf(">>> 이동 중에는 목적지 변경 불가\n");
                    }
                    break;

                case '3':
                    if (system_state == 0 || system_state == 4) {
                        if (system_state == 4) {
                            ST();
                            system_state = 0;
                            printf(">>> 자동 모드로 전환\n");
                        }
                        target_table = 3;
                        zone_count = 0;
                        printf(">>> 목적지: 테이블 3 설정\n");
                    } else {
                        printf(">>> 이동 중에는 목적지 변경 불가\n");
                    }
                    break;

                // === 물건 센서 시뮬레이션 ===
                case 'L':
                case 'l':
                    item_sensor = 1;
                    printf(">>> LOAD (수동입력)\n");
                    break;

                case 'U':
                case 'u':
                    item_sensor = 0;
                    printf(">>> UNLOAD (수동입력)\n");
                    break;

                // === 강제 홈 복귀 ===
                case '0':
                    if (system_state == 4) {
                        // 수동모드 해제
                        ST();
                        system_state = 0;
                        target_table = 0;
                        zone_count = 0;
                        printf(">>> 수동모드 해제, 홈 대기 상태\n");
                    } else if (system_state == 1 || system_state == 2) {
                        // 이동중/도착 상태에서 강제 복귀
                        system_state = 3;
                        zone_count = 0;  // 복귀용 카운터 리셋
                        printf(">>> 강제 홈 복귀 시작\n");
                    }
                    break;

                // === 비상 정지 ===
                case 'X':
                case 'x':
                    ST();
                    system_state = 0;
                    target_table = 0;
                    zone_count = 0;
                    printf(">>> 비상 정지! 초기화\n");
                    break;

                // === 수동 조작 ===
                case 'W':
                case 'w':
                    system_state = 4;
                    MF();
                    printf(">>> 수동: 전진\n");
                    break;

                case 'S':
                case 's':
                    system_state = 4;
                    MB();
                    printf(">>> 수동: 후진\n");
                    break;

                case 'A':
                case 'a':
                    system_state = 4;
                    ML();
                    printf(">>> 수동: 좌회전\n");
                    break;

                case 'D':
                case 'd':
                    system_state = 4;
                    MR();
                    printf(">>> 수동: 우회전\n");
                    break;

                case 'q':
                case 'Q':
                    system_state = 4;
                    TRF();
                    printf(">>> 수동: 좌회전(전진)\n");
                    break;

                case 'e':
                case 'E':
                    system_state = 4;
                    TLF();
                    printf(">>> 수동: 우회전(전진)\n");
                    break;

                case 'z':
                case 'Z':
                    system_state = 4;
                    TRB();
                    printf(">>> 수동: 좌회전(후진)\n");
                    break;

                case 'c':
                case 'C':
                    system_state = 4;
                    TLB();
                    printf(">>> 수동: 우회전(후진)\n");
                    break;

                default:
                    break;
            }
        }

        // ============================================
        // === 센서 변화 감지 ===
        // ============================================
        uint8_t sensor_changed = (SEN1 != SEN1_prev || SEN2 != SEN2_prev);

        // ============================================
        // === 서빙 로봇 상태 머신 ===
        // ============================================
        /*
         * 전진 센서 시퀀스:
         * 홈(0,0) → (1,1) → (1,0)[T1] → (0,1)[T2] → (0,0)[T3]
         *           zone=1   zone=2      zone=3      zone=4
         *
         * 복귀 센서 시퀀스 (역순):
         * T3(0,0) → (0,1) → (1,0) → (1,1) → (0,0)[홈]
         * T2(0,1) → (1,0) → (1,1) → (0,0)[홈]
         * T1(1,0) → (1,1) → (0,0)[홈]
         */

        switch (system_state)
        {
            // -----------------------------------------
            // 상태 0: 홈 대기
            // -----------------------------------------
            case 0:
                ST();
                if (target_table != 0 && item_sensor == 1) {
                    system_state = 1;
                    zone_count = 0;
                    printf("\n=== 테이블 %d로 출발! ===\n", target_table);
                }
                break;

            // -----------------------------------------
            // 상태 1: 전진 중
            // -----------------------------------------
            case 1:
                MF();

                if (sensor_changed) {
                    // (0,0) → (1,1): zone 1
                    if (SEN1 == 1 && SEN2 == 1 && zone_count == 0) {
                        zone_count = 1;
                        printf(">> 구간1 진입 (1,1)\n");
                    }
                    // (1,1) → (1,0): zone 2 = 테이블1 위치
                    else if (SEN1 == 1 && SEN2 == 0 && zone_count == 1) {
                        zone_count = 2;
                        printf(">> 구간2 진입 (1,0)\n");
                        if (target_table == 1) {
                            ST();
                            system_state = 2;
                            printf("\n=== 테이블 1 도착! ===\n");
                        }
                    }
                    // (1,0) → (0,1): zone 3 = 테이블2 위치
                    else if (SEN1 == 0 && SEN2 == 1 && zone_count == 2) {
                        zone_count = 3;
                        printf(">> 구간3 진입 (0,1)\n");
                        if (target_table == 2) {
                            ST();
                            system_state = 2;
                            printf("\n=== 테이블 2 도착! ===\n");
                        }
                    }
                    // (0,1) → (0,0): zone 4 = 테이블3 위치
                    else if (SEN1 == 0 && SEN2 == 0 && zone_count == 3) {
                        zone_count = 4;
                        printf(">> 구간4 진입 (0,0)\n");
                        if (target_table == 3) {
                            ST();
                            system_state = 2;
                            printf("\n=== 테이블 3 도착! ===\n");
                        }
                    }
                }
                break;

            // -----------------------------------------
            // 상태 2: 도착 - 물건 제거 대기
            // -----------------------------------------
            case 2:
                ST();
                if (item_sensor == 0) {
                    system_state = 3;
                    zone_count = 0;  // 복귀용 카운터 리셋
                    printf("\n=== 물건 제거됨! 홈으로 복귀 시작 ===\n");
                }
                break;

            // -----------------------------------------
            // 상태 3: 복귀 중
            // -----------------------------------------
            case 3:
                MB();

                if (sensor_changed) {
                    /*
                     * 복귀 시퀀스 (현재 위치에 따라 다름):
                     * T1(1,0): (1,0)→(1,1)→(0,0)홈
                     * T2(0,1): (0,1)→(1,0)→(1,1)→(0,0)홈
                     * T3(0,0): (0,0)→(0,1)→(1,0)→(1,1)→(0,0)홈
                     *
                     * 공통: (1,1) 만나면 다음 (0,0)이 홈
                     */

                    // 테이블1 복귀: (1,0) → (1,1) → (0,0)
                    if (target_table == 1) {
                        if (SEN1 == 1 && SEN2 == 1 && zone_count == 0) {
                            zone_count = 1;
                            printf(">> 복귀: (1,1) 진입\n");
                        }
                        else if (SEN1 == 0 && SEN2 == 0 && zone_count == 1) {
                            ST();
                            system_state = 0;
                            target_table = 0;
                            zone_count = 0;
                            printf("\n=== 홈 도착! ===\n");
                        }
                    }
                    // 테이블2 복귀: (0,1) → (1,0) → (1,1) → (0,0)
                    else if (target_table == 2) {
                        if (SEN1 == 1 && SEN2 == 0 && zone_count == 0) {
                            zone_count = 1;
                            printf(">> 복귀: (1,0) 진입\n");
                        }
                        else if (SEN1 == 1 && SEN2 == 1 && zone_count == 1) {
                            zone_count = 2;
                            printf(">> 복귀: (1,1) 진입\n");
                        }
                        else if (SEN1 == 0 && SEN2 == 0 && zone_count == 2) {
                            ST();
                            system_state = 0;
                            target_table = 0;
                            zone_count = 0;
                            printf("\n=== 홈 도착! ===\n");
                        }
                    }
                    // 테이블3 복귀: (0,0) → (0,1) → (1,0) → (1,1) → (0,0)
                    else if (target_table == 3) {
                        if (SEN1 == 0 && SEN2 == 1 && zone_count == 0) {
                            zone_count = 1;
                            printf(">> 복귀: (0,1) 진입\n");
                        }
                        else if (SEN1 == 1 && SEN2 == 0 && zone_count == 1) {
                            zone_count = 2;
                            printf(">> 복귀: (1,0) 진입\n");
                        }
                        else if (SEN1 == 1 && SEN2 == 1 && zone_count == 2) {
                            zone_count = 3;
                            printf(">> 복귀: (1,1) 진입\n");
                        }
                        else if (SEN1 == 0 && SEN2 == 0 && zone_count == 3) {
                            ST();
                            system_state = 0;
                            target_table = 0;
                            zone_count = 0;
                            printf("\n=== 홈 도착! ===\n");
                        }
                    }
                    // 강제 복귀 (target_table이 0이거나 알 수 없는 위치)
                    else {
                        // (0,0)이면서 이전에 (1,1)을 지나온 경우 = 홈
                        if (SEN1 == 1 && SEN2 == 1) {
                            zone_count = 99;  // (1,1) 통과 표시
                            printf(">> 복귀: (1,1) 통과\n");
                        }
                        else if (SEN1 == 0 && SEN2 == 0 && zone_count == 99) {
                            ST();
                            system_state = 0;
                            target_table = 0;
                            zone_count = 0;
                            printf("\n=== 홈 도착! ===\n");
                        }
                    }
                }
                break;

            // -----------------------------------------
            // 상태 4: 수동 모드
            // -----------------------------------------
            case 4:
                // 모터 상태 유지
                break;
        }

        // ============================================
        // === 디버그 출력 ===
        // ============================================
        const char* state_str[] = {"HOME", "FORWARD", "ARRIVED", "RETURN", "MANUAL"};
        const char* item_str = (item_sensor == 1) ? "LOAD" : "UNLOAD";

        printf("ST=%d, TBL=%d, ZONE=%d, SEN=(%d,%d), %s\n",
               system_state, target_table, zone_count, SEN1, SEN2, item_str);

        HAL_Delay(50);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /*Configure GPIO pins : LT1_Pin ITEM_Pin LT2_Pin */
  GPIO_InitStruct.Pin = LT1_Pin|ITEM_Pin|LT2_Pin;
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
