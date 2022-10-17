# IOCP Network Library 

## 클래스 설명
- Service : 소켓 생성 및 연결 이벤트 등록 
    - ServerService
    - ClientService
- IocpHandler : IOCP에 등록된 이벤트 처리
    - Session
- IocpCore : IOCP 관리(소켓 등록 및 이벤트 전달)
- IocpEvent : IO 이벤트 컨텍스트 관리 

## 코드 기본 흐름
1. IOCP 생성
2. 클라이언트 세션 생성 및 연결/IO Event 등록 (`RegisterEventName()`)
3. 각 스레드가 GetQueuedCompletionStatus() 함수를 호출해서 꺼내온 이벤트를 처리(`ProcessEventName()`)

## 코드 상세 흐름
[코드 흐름](%EC%BD%94%EB%93%9C%ED%9D%90%EB%A6%84.xlsx) 참고
