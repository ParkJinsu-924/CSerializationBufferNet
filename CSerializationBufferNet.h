#pragma once
#include <iostream>
#include <Windows.h>
#include "CMemoryPoolTLS\CMemoryPoolTLS.h"


class CSerializationBuffer
{
	//메모리풀은 생성자를 호출해야하기 때문에
	friend class CMemoryPoolTLS<CSerializationBuffer>;
	friend class CNetServer;
	enum enPACKET
	{
		eBUFFER_DEFAULT = 1400
	};
public:

	//패킷파괴
	void Release();

	//패킷청소
	void Clear();

	//버퍼사이즈얻기
	int GetBufferSize();

	//버퍼포인터얻기
	char* GetContentBufPtr();

	int AddRef();
	void DeqRef();

	CSerializationBuffer& operator=(CSerializationBuffer& clSrcPacket);

	//int GetData(char* chpDest, int iSize);

	//int PutData(char* chpSrc, int iSrcSize);

	void PutContentData(char* chpSrc, int iSize);

	void GetContentData(char* chpSrc, int iSize);

	int GetContentUseSize();

	int GetTotalUseSize();

	int GetFreeSize();

	static CSerializationBuffer* Alloc();

	bool Free();

private:
#pragma pack(1)
	struct stHEADER
	{
		UCHAR code;
		USHORT length;
		UCHAR randKey;
		UCHAR checkSum;
	};
#pragma pack()
private:
	CSerializationBuffer();

	virtual ~CSerializationBuffer();

private:
	int refCount;
	int iBufferSize;
	char* chpReadPos;
	char* chpWritePos;
	//버퍼의 시작점
	char* chpBufferPtr;
	//컨텐츠 버퍼의 시작점
	char* chpContentBufferPtr;
	bool bEncodeFlag;
	int headerSize;

	static CMemoryPoolTLS<CSerializationBuffer> memoryPool;

private:
	//버퍼pos이동, 음수이동은 안됨
	int MoveWritePos(int iSize);
	int MoveReadPos(int iSize);

	static bool Encode(CSerializationBuffer*);
	static bool Decode(CSerializationBuffer*);
	
	void PutNetworkHeader(char* chpSrc, int iSize);
	void GetNetworkHeader(char* chpSrc, int iSize);
	
	char* GetBufferPtr();
};
