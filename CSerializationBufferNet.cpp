#include "CSerializationBufferNet.h"

CMemoryPoolTLS<CSerializationBuffer> CSerializationBuffer::memoryPool;

CSerializationBuffer::CSerializationBuffer()
{
	chpBufferPtr = new char[eBUFFER_DEFAULT];
	chpContentBufferPtr = chpWritePos = chpReadPos = chpBufferPtr + 5;
	iBufferSize = eBUFFER_DEFAULT;
	refCount = 0;
}

CSerializationBuffer::~CSerializationBuffer()
{
	delete chpBufferPtr;
}

void CSerializationBuffer::Release()
{
	this->~CSerializationBuffer();
}

void CSerializationBuffer::Clear()
{
	chpWritePos = chpReadPos = chpBufferPtr + 5;
	bEncodeFlag = false;
}

int CSerializationBuffer::GetBufferSize()
{
	return iBufferSize;
}

bool CSerializationBuffer::Encode(CSerializationBuffer* pPacket)
{
	if (pPacket->bEncodeFlag)
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

	pPacket->bEncodeFlag = true;
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
	return chpBufferPtr;
}

char* CSerializationBuffer::GetContentBufPtr()
{
	return chpContentBufferPtr;
}

int CSerializationBuffer::MoveWritePos(int iSize)
{
	chpWritePos += iSize;
	return iSize;
}

int CSerializationBuffer::MoveReadPos(int iSize)
{
	chpReadPos += iSize;
	return iSize;
}

int CSerializationBuffer::AddRef()
{
	InterlockedIncrement((long*)&refCount);
	return refCount;
}

void CSerializationBuffer::DeqRef()
{
	if (InterlockedDecrement((long*)&refCount) == 0)
	{
		Free();
	}
}

CSerializationBuffer& CSerializationBuffer::operator=(CSerializationBuffer& clSrcPacket)
{
	// TODO: 여기에 return 문을 삽입합니다.
	return *this;
}

//int CSerializationBuffer::GetData(char* chpDest, int iSize)
//{
//	memcpy(chpDest, chpReadPos, iSize);
//	chpReadPos += iSize;
//	return *chpDest;
//}
//
//int CSerializationBuffer::PutData(char* chpSrc, int iSrcSize)
//{
//	memcpy(chpWritePos, chpSrc, iSrcSize);
//	chpWritePos += iSrcSize;
//	return *chpSrc;
//}

void CSerializationBuffer::PutContentData(char* chpSrc, int iSize)
{
	memcpy(chpWritePos, chpSrc, iSize);
	chpWritePos += iSize;
}

void CSerializationBuffer::GetContentData(char* chpSrc, int iSize)
{
	memcpy(chpSrc, chpReadPos, iSize);
	chpReadPos += iSize;
}

void CSerializationBuffer::PutNetworkHeader(char* chpSrc, int iSize)
{
	memcpy(chpBufferPtr, chpSrc, iSize);
}

void CSerializationBuffer::GetNetworkHeader(char* chpSrc, int iSize)
{
	memcpy(chpSrc, chpBufferPtr, iSize);
}

int CSerializationBuffer::GetContentUseSize()
{
	int retVal = chpWritePos - chpReadPos;
	return retVal;
}

int CSerializationBuffer::GetTotalUseSize()
{
	return chpWritePos - chpBufferPtr;
}

int CSerializationBuffer::GetFreeSize()
{
	return chpBufferPtr + iBufferSize - chpWritePos;
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
