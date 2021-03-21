# CSerializationBufferNet
* 외부통신용 직렬화버퍼
* CMemoryPoolTLS 라이브러리를 사용합니다.
* 외부통신용이므로 패킷 인코딩함수와 디코딩함수가 있습니다.
``` C
  //직렬화버퍼를 암호화합니다.
  //같은 패킷을 여러명에게 송신할 경우 중복 암호화를 방지하기 위한 예외처리가 있습니다. 이 경우 암호화 실패(false)를 반환합니다.
  //반환값 : 암호화 성공여부
  bool Encode(CSerializationBuffer*);
  //직렬화버퍼를 복호화합니다.
  //반환값 : 복호화 성공여부
  bool Decode(CSerializationBuffer*);
```
* CNetServer라이브러리에서 사용합니다.
* 참조카운트개념이 있습니다. 똑같은 패킷을 여러명에게 보낼 경우, 보낸만큼 직렬화버퍼의 참조카운트를 증가시켜 하나의 패킷으로 여러명에게 안전하게 송신할 수 있습니다. 
패킷의 반환 시점은 참조카운트가 0 이 되었을 때 반환합니다.
``` C
  //참조카운트 증가
  //반환값 : 증가 후 참조카운트
  int AddRef();
  //참조카운트 감소, 0이 될 시 내부적으로 반환
  //반환값 : 없음
  void DeqRef();
```
* CSerializationBuffer::Alloc()함수로 직렬화버퍼를 할당받을 수 있습니다. Free()함수로 해제할 수 있습니다.
``` C
  //직렬화버퍼를 할당합니다.
  //반환값 : 직렬화버퍼 포인터
  static CSerializationBuffer* Alloc();
  //직렬화버퍼를 반환합니다.
  //반환값 : 반환 성공여부
  bool Free();
```
* 나머지 전체 함수
``` C
  //쓰기포인터를 옮깁니다.
  //반환값 : 옮긴 크기
  int MoveWritePos(int);
  //읽기포인터를 옮깁니다.
  //반환값 : 옮긴 크기
  int MoveReadPos(int);
  //CNetServer에서 사용하는 네트워크 헤더를 세팅합니다.
  //반환값 : 없음
  void PutNetworkHeader(char*, int);
  //CNetServer에서 사용하는 네트워크 헤더를 가져옵니다.
  //반환값 : 없음
  void GetNetworkHeader(char*, int);
  //컨텐츠 데이터를 세팅합니다.
  //반환값 : 없음
  void PutContentData(char*, int);
  //컨텐츠 데이터를 가져옵니다.
  //반환값 : 없음
  void GetContentData(char*, int);
  //컨텐츠 사용 사이즈를 가져옵니다.
  //반환값 : 사이즈 크기
  int GetContentUseSize();
  //반환값 : writepos - bufferptr
  int GetTotalUseSize();
  //컨텐츠버퍼의 시작점 포인터를 얻습니다.
  //반환값 : 버퍼의 포인터
  char* GetContentBufPtr();
  //반환값 : 직렬화버퍼의 사이즈
  int GetBufferSize();
  //직렬화버퍼를 초기화합니다.
  //반환값 : 없음
  void Clear();
  //소멸자를 호출해줍니다.
  //반환값 : 없음
  void Release();
```
