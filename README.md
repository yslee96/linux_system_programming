# linux_system_programming
리눅스의 프로세스 스케줄링 환경에서, time-slice에 따른 문맥전환(Context switching) 오버헤드를 측정 및 분석한다.

## 실험 방법
- 실험환경(OS) : Ubuntu 18.04, Linux(4.20.11), x86_64 에서 진행한다.
- cpu.c 를 통해서 100ms 동안 특정 프로세스가 수행한 연산의 횟수를 측정
-  dmesg 상에 priority 가 10 인 프로세스의 cpu burst time 이 출력되도록 
   커널 소스 코드 수정 및 컴파일

## 요구 사항
- 자식 프로세스 fork() 기능 구현
- 시간 측정 위한 time_diff() 함수 구현
- <sched.h> 활용해 프로세스 스케줄링 방식 round robin 으로 변경
- sched_priority 값 변경

## Result
<img width="706" alt="스크린샷 2022-12-13 오후 5 57 13" src="https://user-images.githubusercontent.com/77106988/217213342-7d53622f-25f8-45d0-8c20-d99e55133f27.png">

<img width="600" alt="image" src="https://user-images.githubusercontent.com/77106988/217214534-7e9ac4dc-e68d-4e6f-aa48-a9e4a3acd84b.png">

<img width="598" alt="image" src="https://user-images.githubusercontent.com/77106988/217211254-55e78208-3f21-41a9-8dda-266b4279b949.png">
