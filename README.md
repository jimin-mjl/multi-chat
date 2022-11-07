# IOCP Network Library

## 개요

1000 이상의 클라이언트 접속을 받을 수 있는 IOCP 기반 네트워크 라이브러리와 테스트 서버를 만든다.

## 목표

- 비동기 IO 방식과 IOCP의 동작 과정을 이해한다.
- 멀티 스레드 환경에서 데이터의 일관성을 유지할 수 있는 방법을 고민한다.

## 동작 확인

[데모 비디오 링크](https://drive.google.com/file/d/1hb-B9nKAisOzDcoaYL9aSKKVavIUR8eJ/view?usp=sharing)

## 개발 환경

- IDE : Visual Studio 2022
- OS : Windows 11 (64bit)
- 프로그래밍 언어 : C++14
- API : IOCP

## 구현 내용

- 테스트 서버
  - 최대 연결 횟수만큼 클라이언트 연결을 받고 수신한 데이터를 되돌려준다. (에코 서버)
- 더미 클라이언트
  - 1000개의 클라이언트를 생성해서 서버에 연결 요청을 보내고, 연결이 완료되면 데이터를 송신한다.
- 네트워크 라이브러리
  - 윈도우의 IOCP API를 사용해 비동기로 accept, connect, send, receive, disconnect 이벤트를 등록한다. 완료된 이벤트는 이벤트를 소유한 세션으로 보내져 처리된다.
  - 이벤트는 Register(등록) -> Dispatch(세션에 분배) -> Process(처리) 의 과정으로 네트워크 라이브러리에서 처리되고, 이후는 콘텐츠 코드 영역에서 처리된다.

## 코드 기본 흐름

1. Service 클래스가 IOCP 객체를 생성하고 클라이언트의 연결 요청을 받는다.
2. Accept 요청이 들어오면 메인 스레드가 클라이언트 세션을 생성하고 Accept 이벤트를 등록한다.
3. 이벤트가 완료되면 IO worker 스레드가 GQCS 함수를 통해 꺼내온 후 해당 이벤트를 처리한다.
4. 이벤트가 처리되면 SessionManager 클래스가 세션을 등록한다.
5. 코드의 상세 흐름은 [코드 흐름 문서](%EC%BD%94%EB%93%9C%ED%9D%90%EB%A6%84.xlsx) 참고

## 성능을 위해 고려한 부분

- Socket Pooling 구현을 통해 socket 생성/삭제 비용 절약 : AcceptEx와 DisconnectEx 함수를 사용해 소켓을 재사용할 수 있도록 작성
- Accept 요청을 처리하는 스레드와 IO 요청을 처리하는 스레드를 분리해 다수의 요청을 효율적으로 처리할 수 있도록 작성
- Gather-Scatter 기법을 사용해 Send 시의 오버헤드를 줄이도록 구현 : send queue에 보낼 데이터를 모아뒀다가 한꺼번에 WSASend 함수를 사용해 발송

## Next Step

- Send 이벤트 담당 스레드를 분리해서 일정 시간마다 send 스레드가 send queue에 차 있는 데이터를 등록하도록 구현한다.
- 패킷 헤더를 사용해 보낸 데이터를 모두 받았는지 확인하는 작업을 추가한다.
- Broadcast 기능과 Room 관련 로직을 추가해 chat room 방식으로 발전시킨다.
