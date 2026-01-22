# 📡 STM32 IR 리모컨 신호 분석 및 복제 (Reverse Engineering)

## 1. 프로젝트 개요 (Overview)
상용 IR(적외선) 리모컨의 동작 원리를 **하드웨어(회로)와 소프트웨어(신호) 관점에서 역분석(Reverse Engineering)**하고, STM32를 이용해 해당 리모컨의 기능을 완벽하게 복제하는 프로젝트입니다.
단순한 라이브러리 사용이 아니라, **로직 분석기를 통해 물리적 파형을 캡처하고 펄스 폭을 분석하여 통신 프로토콜(NEC 포맷 변형)을 규명**하는 과정에 집중했습니다.

---

## 2. 기술 스택 (Tech Stack)
*   **HW:** STM32F103C8T6, IR Transmitter(40kHz)
*   **SW:** C, STM32 HAL
*   **Analysis Tools:** Logic Analyzer (Saleae Logic 2), Oscilloscope
*   **Designing:** KiCad (회로 역설계 및 도면화)

---

## 3. 분석 및 구현 과정 (Process)

### Step 1. 하드웨어 역설계 (Circuit Reverse Engineering)
*   리모컨 PCB의 패턴을 추적하여 회로도를 직접 복원했습니다. (첨부된 `original_ir_remote_controller.png` 참조)
*   신호가 MCU의 어느 핀에서 출력되어 트랜지스터를 거쳐 IR LED로 나가는지 경로를 파악하여 측정 포인트를 확보했습니다.

### Step 2. 신호 캡처 및 프로토콜 분석 (Signal Analysis)
*   **도구:** 로직 분석기(Logic Analyzer)를 사용하여 IR 펄스 파형 캡처
*   **분석:**
    *   Leader Code와 Data Bit의 High/Low 타이밍(펄스 폭) 측정
    *   분석 결과, 표준 **NEC 프로토콜**을 기반으로 커스텀 된 데이터 포맷임을 확인
    *   **Carrier Frequency:** 38kHz PWM 변조 확인

### Step 3. 펌웨어 구현 및 검증 (Emulation)
*   분석된 타이밍 데이터를 바탕으로 STM32 타이머(PWM)를 사용하여 신호 재현
*   실제 가전제품이 정상적으로 동작함을 확인하여 **데이터 무결성 검증 완료**

---

## 4. 프로젝트 구조 (File Structure)

```text
.
├── Circuit Diagram/
│   ├── STM32F103C8T6_IR.png           # STM32 송신 회로도
│   └── original_ir_remote_controller.png  # [역설계] 타겟 리모컨 회로도
├── Sources/
│   ├── My_remote_Control.ioc          # CubeMX 설정 파일
│   └── main.c                         # NEC 프로토콜 송신 로직 구현
└── logic_analyzer/
    └── capture_data.sal               # Logic2 캡처 원본 데이터

```
---

## 5. 결과 (Result)
* 분석 결과: 타겟 리모컨은 Address: 0xXX, Command: 0xXX 구조의 NEC 변형 프로토콜을 사용함.
* 성과: STM32로 복제한 신호를 통해 타겟 디바이스(Barset_4D_Fan) 제어 성공.

---

## 📄 라이선스
MIT 라이선스 – 자세한 내용은 [`LICENSE`](LICENSE)를 참조하십시오.
