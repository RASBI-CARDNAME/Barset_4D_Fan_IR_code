/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  *Made By RASBI
  *
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* 타이머 최대 카운트 값 (16비트 타이머 기준) */
#define MAX_TIMER_COUNTER 65535

/* IR 프로토콜 타이밍 상수 정의 (단위: us) */
#define SYNC_TIME 7700       // 초기 동기화 신호의 High 구간 길이
#define TIME_CASE_1 500      // 짧은 펄스/지연 시간 (비트 0 또는 1의 구성 요소)
#define TIME_CASE_2 1000     // 긴 펄스/지연 시간 (비트 0 또는 1의 구성 요소)
#define END_SYNC_TIME 900    // 종료 신호 관련 시간 (현재 코드에선 직접 사용 안됨)

/* IR 프로토콜의 시작 및 종료 코드 패턴 */
const uint8_t start_code = 0xAA; // 2진수: 10101010
const uint8_t end_code = 0xA;    // 2진수: 1010

/* 전송할 IR 명령어 데이터 배열 (12비트 데이터로 추정) */
const uint16_t function_ir [] = {0x209, 0x40E, 0x10B, 0x806};

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
/* 사용자 정의 함수 원형 선언 */
void delay_us(uint32_t us);             // 마이크로초 단위 딜레이
void sync_signal();                     // 시작(Sync) 신호 전송
void end_signal();                      // 종료(End) 신호 전송
void output_a_signal(uint16_t logic);   // 전체 IR 패킷 전송

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
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  /* 마이크로초 딜레이용 타이머(TIM2) 시작 */
  HAL_TIM_Base_Start(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      /* 버튼 입력 처리 (Active Low 방식: 버튼 누르면 GND 연결 -> 0 인식) */
      
      // PB3 버튼이 눌렸을 때 -> 첫 번째 IR 코드 전송
	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET) {
		  output_a_signal(function_ir[0]);
		  HAL_Delay(100); // 디바운싱 및 중복 전송 방지
	  } 
      // PB4 버튼이 눌렸을 때 -> 두 번째 IR 코드 전송
      else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == GPIO_PIN_RESET) {
		  output_a_signal(function_ir[1]);
		  HAL_Delay(100);
	  } 
      // PB5 버튼이 눌렸을 때 -> 세 번째 IR 코드 전송
      else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET) {
		  output_a_signal(function_ir[2]);
		  HAL_Delay(100);
	  } 
      // PB6 버튼이 눌렸을 때 -> 네 번째 IR 코드 전송
      else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == GPIO_PIN_RESET) {
		  output_a_signal(function_ir[3]);
		  HAL_Delay(100);
	  }
  }
  /* USER CODE END 3 */
}

// ... (중간 HAL 생성 코드 생략) ...

/* USER CODE BEGIN 4 */

/**
  * @brief  마이크로초(us) 단위로 대기하는 함수
  * @param  us: 대기할 시간 (마이크로초)
  * @note   TIM2 타이머의 카운터를 사용하여 정확한 시간을 측정함
  */
void delay_us(uint32_t us) {
    // 타이머 최대값(16비트)을 초과하지 않도록 제한
	if (us > MAX_TIMER_COUNTER) {
		us = MAX_TIMER_COUNTER - 1;
	}
    // 타이머 카운터를 0으로 초기화
	__HAL_TIM_SET_COUNTER(&htim2, 0);
    // 카운터 값이 목표 시간(us)에 도달할 때까지 대기 (Blocking Delay)
	while((uint32_t)__HAL_TIM_GET_COUNTER(&htim2) < us);
}

/**
  * @brief  IR 통신의 시작을 알리는 Sync 신호 전송 함수
  * @note   긴 PWM 신호 후 start_code 패턴을 보냄
  */
void sync_signal() {
    // 1. 리더 코드 (Leader Code): PWM 7.7ms 출력
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	delay_us(SYNC_TIME);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

    // 2. Start Code 패턴 전송 (4회 반복)
    // 패턴: [Space(긴) -> PWM(짧은)] -> [Space(짧은) -> PWM(긴)]
	for (int i = 0; i<4; i++) {
        // 첫 번째 부분
		delay_us(TIME_CASE_2);                      // 1000us 대기 (신호 없음)
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);   // PWM 시작
		delay_us(TIME_CASE_1);                      // 500us 유지
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);    // PWM 정지

        // 두 번째 부분
		delay_us(TIME_CASE_1);                      // 500us 대기 (신호 없음)
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);   // PWM 시작
		delay_us(TIME_CASE_2);                      // 1000us 유지
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);    // PWM 정지
	}
}

/**
  * @brief  IR 통신의 종료를 알리는 End 신호 전송 함수
  * @note   end_code 패턴을 보냄 (2회 반복)
  */
void end_signal() {
	for (int i = 0; i<2; i++) {
        // sync_signal 내부 루프와 동일한 패턴을 2번 반복
		delay_us(TIME_CASE_2);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		delay_us(TIME_CASE_1);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

		delay_us(TIME_CASE_1);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		delay_us(TIME_CASE_2);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	}
}

/**
  * @brief  전체 IR 신호 패킷을 전송하는 함수
  * @param  logic: 전송할 12비트 데이터
  * @note   신뢰성을 위해 전체 패킷을 3회 반복 전송함
  */
void output_a_signal(uint16_t logic) {
    // 전체 패킷을 3회 반복 전송
	for (int k = 0; k < 3; k ++){
		sync_signal(); // 1. 헤더(Sync) 전송

        // 2. 12비트 데이터 전송 (MSB부터 LSB 순서로 추정, i=11 downto 0)
		for (int i = 11; i>=0; i--) {
            // 해당 비트가 1인 경우
			if ((logic >> i) & 0x01) {
                // 비트 1 인코딩: 긴 Space(1000us) + 짧은 PWM(500us)
				delay_us(TIME_CASE_2);
				HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
				delay_us(TIME_CASE_1);
				HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			} 
            // 해당 비트가 0인 경우
            else {
                // 비트 0 인코딩: 짧은 Space(500us) + 긴 PWM(1000us)
