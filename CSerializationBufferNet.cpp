#include "CSerializationBufferNet.h"

CMemoryPoolTLS<CSerializationBuffer> CSerializationBuffer::memoryPool;

CSerializationBuffer::CSerializationBuffer()
{
	mpBuffer = new char[eBUFFER_DEFAULT];
	mpContentBuffer = mpWritePos = mpReadPos = mpBuffer + 5;
	mBufferSize = eBUFFER_DEFAULT;
	mRefCount = 0;
}

CSerializationBuffer::~CSerializationBuffer()
{
	delete mpBuffer;
}

void CSerializationBuffer::Release()
{
	this->~CSerializationBuffer();
}

void CSerializationBuffer::Clear()
{
	mpWritePos = mpReadPos = mpBuffer + 5;
	mEncodeFlag = false;
}

int CSerializationBuffer::GetBufferSize()
{
	return mBufferSize;
}

bool CSerializationBuffer::Encode(CSerializationBuffer* pPacket)
{
	if (pPacket->mEncodeFlag)
		return false;

	stHEADER header;

	header.code = 0x77;
	header.length = pPacket->GetContentUseSize();
	header.randKey = rand() % 256;
	int sumCount = 0;
	char* pContent = pPacket->GetContentBufPtr();
	for (int i = 0; i < header.length; i++)
	{
		sumCount += *pContent;
		pContent++;
	}

	header.checkSum = sumCount % 256;

	pPacket->PutNetworkHeader((char*)&header, sizeof(header));

	char key = 0x32;
	char randomKey = header.randKey;
	char* pPacketBuf = pPacket->GetContentBufPtr() - sizeof(UCHAR);
	char thep = 0;
	char resultBuf = 0;

	for (int i = 1; i <= header.length + 1; i++)
	{
		thep = *pPacketBuf ^ (thep + randomKey + i);
		resultBuf = *pPacketBuf = thep ^ (resultBuf + key + i);
		pPacketBuf += 1;
	}

	pPacket->mEncodeFlag = true;
	return true;
}

bool CSerializationBuffer::Decode(CSerializationBuffer* pPacket)
{
	stHEADER header;
	pPacket->GetNetworkHeader((char*)&header, sizeof(header));

	char key = 0x32;
	char randomKey = header.randKey;
	char* pPacketBuf = pPacket->GetContentBufPtr() - sizeof(UCHAR);
	char thep = 0;
	char ebuf, pbuf;

	ebuf = *pPacketBuf;
	pbuf = thep = *pPacketBuf ^ (key + 1);
	*pPacketBuf = thep ^ (randomKey + 1);
	pPacketBuf += 1;

	for (int i = 2; i <= header.length + 1; i++)
	{
		thep = *pPacketBuf ^ (ebuf + key + i);
		ebuf = *pPacketBuf;
		*pPacketBuf = thep ^ (pbuf + randomKey + i);
		pbuf = thep;
		pPacketBuf += 1;
	}

	pPacket->GetNetworkHeader((char*)&header, sizeof(header));

	UCHAR checkSum = 0;
	int sumCount = 0;
	char* pContent = pPacket->GetContentBufPtr();

	for (int i = 0; i < header.length; i++)
	{
		sumCount += *pContent;
		pContent++;
	}
	checkSum = sumCount % 256;
	if (header.checkSum != checkSum)
	{
		return false;
	}

	return true;
}

char* CSerializationBuffer::GetBufferPtr()
{
	return mpBuffer;
}

char* CSerializationBuffer::GetContentBufPtr()
{
	return mpContentBuffer;
}

int CSerializationBuffer::MoveWritePos(int iSize)
{
	mpWritePos += iSize;
	return iSize;
}

int CSerializationBuffer::MoveReadPos(int iSize)
{
	mpReadPos += iSize;
	return iSize;
}

int CSerializationBuffer::AddRef()
{
	InterlockedIncrement((long*)&mRefCount);
	return mRefCount;
}

void CSerializationBuffer::DeqRef()
{
	if (InterlockedDecrement((long*)&mRefCount) == 0)
	{
		Free();
	}
}

void CSerializationBuffer::PutContentData(char* chpSrc, int iSize)
{
	memcpy(mpWritePos, chpSrc, iSize);
	mpWritePos += iSize;
}

void CSerializationBuffer::GetContentData(char* chpSrc, int iSize)
{
	memcpy(chpSrc, mpReadPos, iSize);
	mpReadPos += iSize;
}

void CSerializationBuffer::PutNetworkHeader(char* chpSrc, int iSize)
{
	memcpy(mpBuffer, chpSrc, iSize);
}

void CSerializationBuffer::GetNetworkHeader(char* chpSrc, int iSize)
{
	memcpy(chpSrc, mpBuffer, iSize);
}

int CSerializationBuffer::GetContentUseSize()
{
	int retVal = mpWritePos - mpReadPos;
	return retVal;
}

int CSerializationBuffer::GetTotalUseSize()
{
	return mpWritePos - mpBuffer;
}

int CSerializationBuffer::GetFreeSize()
{
	return mpBuffer + mBufferSize - mpWritePos;
}

CSerializationBuffer* CSerializationBuffer::Alloc()
{
	CSerializationBuffer* pSerialBuf = memoryPool.Alloc();

	pSerialBuf->Clear();

	pSerialBuf->AddRef();

	return pSerialBuf;
}

bool CSerializationBuffer::Free()
{
	return memoryPool.Free(this);
}
