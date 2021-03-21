#pragma once
#include <iostream>
#include <Windows.h>
#include "CMemoryPoolTLS\CMemoryPoolTLS.h"


class CSerializationBuffer
{
	//�޸�Ǯ�� �����ڸ� ȣ���ؾ��ϱ� ������
	friend class CMemoryPoolTLS<CSerializationBuffer>;
	friend class CNetServer;
	friend class CNetClient;
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

	//��������
	int AddRef();

	//��������
	void DeqRef();

	//������ ������ �߰�
	void PutContentData(char* chpSrc, int iSize);

	//������ ������ ��������
	void GetContentData(char* chpSrc, int iSize);

	//������ ũ��
	int GetContentUseSize();

	//���λ��, ������ �� ũ��
	int GetTotalUseSize();

	//FreeSize��������
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
	int mRefCount;
	int mBufferSize;
	int mHeaderSize;
	char* mpReadPos;
	char* mpWritePos;
	char* mpBuffer;
	char* mpContentBuffer;
	bool mEncodeFlag;

	static CMemoryPoolTLS<CSerializationBuffer> memoryPool;

private:
	//����pos�̵�, �����̵��� �ȵ�
	int MoveWritePos(int iSize);
	int MoveReadPos(int iSize);

	static bool Encode(CSerializationBuffer*);
	static bool Decode(CSerializationBuffer*);
	
	void PutNetworkHeader(char* chpSrc, int iSize);
	void GetNetworkHeader(char* chpSrc, int iSize);
	
	char* GetBufferPtr();
};
