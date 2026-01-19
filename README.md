# 프로젝트 개요 (Overview)
리모컨의 ir 패킷을 로직 분석기로 캡처하고 stm32와 ir 블라스터를 통해 리모컨의 동작을 모방합니다.  

---

# 🔧기술 스택 (Tech Stack)
- HW: STM32F103C8T6
- SW: C, Kicad
- Tools: Logic Analyzer, J-link OB

---

# 📂프로젝트 구조

```text
Circuit Diagram
├── STM32F103C8T6_IR.png (프로젝트 회로도)
└── original_ir_remote_controller.png (리모컨 회로도)
Sources
├── My_remote_Control.ioc (STM32CUBEMX 파일)
└── main.c (메인 코드)
logic_analyzer
└── 로직 분석기 캡처 파일 (logic2)
```
