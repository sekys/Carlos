
#include "common_header.h"

#include "vertexBufferObject.h"

CVertexBufferObject::CVertexBufferObject()
{
	bDataUploaded = false;
}

void CVertexBufferObject::createVBO(int a_iSize)
{
	glGenBuffers(1, &uiBuffer);
	data.reserve(a_iSize);
	iSize = a_iSize;
}

void CVertexBufferObject::releaseVBO()
{
	glDeleteBuffers(1, &uiBuffer);
	bDataUploaded = false;
	data.clear();
}

void* CVertexBufferObject::mapBufferToMemory(int iUsageHint)
{
	if(!bDataUploaded)return NULL;
	void* ptrRes = glMapBuffer(iBufferType, iUsageHint);
	return ptrRes;
}

void* CVertexBufferObject::mapSubBufferToMemory(int iUsageHint, UINT uiOffset, UINT uiLength)
{
	if(!bDataUploaded)return NULL;
	void* ptrRes = glMapBufferRange(iBufferType, uiOffset, uiLength, iUsageHint);
	return ptrRes;
}

void CVertexBufferObject::unmapBuffer()
{
	glUnmapBuffer(iBufferType);
}


void CVertexBufferObject::bindVBO(int a_iBufferType)
{
	iBufferType = a_iBufferType;
	glBindBuffer(iBufferType, uiBuffer);
}

void CVertexBufferObject::uploadDataToGPU(int iDrawingHint)
{
	glBufferData(iBufferType, data.size(), &data[0], iDrawingHint);
	bDataUploaded = true;
	data.clear();
}


void CVertexBufferObject::addData(void* ptrData, UINT uiDataSize)
{
	data.insert(data.end(), (BYTE*)ptrData, (BYTE*)ptrData+uiDataSize);
}

void* CVertexBufferObject::getDataPointer()
{
	if(bDataUploaded)return NULL;
	return (void*)data[0];
}


UINT CVertexBufferObject::getBuffer()
{
	return uiBuffer;
}


