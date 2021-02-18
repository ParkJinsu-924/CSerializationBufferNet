#pragma once
#include <iostream>
#include <Windows.h>
#include "CMemoryPoolTLS\CMemoryPoolTLS.h"


class CSerializationBuffer
{
	//�޸�Ǯ�� �����ڸ� ȣ���ؾ��ϱ� ������
	friend class CMemoryPoolTLS<CSerializationBuffer>;
	friend class CNetServer;
	enum enPACKET
	{
		eBUFFER_DEFAULT = 1400
	};
public:

	//��Ŷ�ı�
	void Release();

	//��Ŷû��
	void Clear();

	//���ۻ�������
	int GetBufferSize();

	//���������;��
	char* GetContentBufPtr();

	//����pos�̵�, �����̵��� �ȵ�
	int MoveWritePos(int iSize);
	int MoveReadPos(int iSize);

	int AddRef();
	void DeqRef();

	CSerializationBuffer& operator=(CSerializationBuffer& clSrcPacket);

	//int GetData(char* chpDest, int iSize);

	//int PutData(char* chpSrc, int iSrcSize);

	int PutContentData(char* chpSrc, int iSize);

	int GetContentData(char* chpSrc, int iSize);

	int PutNetworkHeader(char* chpSrc, int iSize);

	int GetNetworkHeader(char* chpSrc, int iSize);

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
	//������ ������
	char* chpBufferPtr;
	//������ ������ ������
	char* chpContentBufferPtr;
	
	bool bEncodeFlag;
	
	int headerSize;

	static CMemoryPoolTLS<CSerializationBuffer> memoryPool;

private:
	static bool Encode(CSerializationBuffer*);
	static bool Decode(CSerializationBuffer*);
	char* GetBufferPtr();
};