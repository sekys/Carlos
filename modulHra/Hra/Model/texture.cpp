
#include "common_header.h"

#include "texture.h"

#include <FreeImage.h>

#pragma comment(lib, "FreeImage.lib")

CTexture::CTexture()
{
	bMipMapsGenerated = false;
}

void CTexture::createFromData(BYTE* bData, int a_iWidth, int a_iHeight, int a_iBPP, GLenum format, bool bGenerateMipMaps)
{
	GLenum internalColourFormat = format;
	if(format == GL_RGBA || format == GL_BGRA || format == GL_RGB || format == GL_BGR) {
		internalColourFormat = GL_RGB;
	}
	createFromData(bData, a_iWidth, a_iHeight, a_iBPP, internalColourFormat, format, bGenerateMipMaps);
}

void CTexture::createFromData(BYTE* bData, int a_iWidth, int a_iHeight, int a_iBPP, GLenum internalColourFormat, GLenum inputColourFormat, bool bGenerateMipMaps)
{
	glGenTextures(1, &uiTexture);
	glBindTexture(GL_TEXTURE_2D, uiTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalColourFormat, a_iWidth, a_iHeight, 0, inputColourFormat, GL_UNSIGNED_BYTE, bData);
	if(bGenerateMipMaps)glGenerateMipmap(GL_TEXTURE_2D);
	glGenSamplers(1, &uiSampler);

	sPath = "";
	bMipMapsGenerated = bGenerateMipMaps;
	iWidth = a_iWidth;
	iHeight = a_iHeight;
	iBPP = a_iBPP;
}

bool CTexture::loadTexture2D(string a_sPath, bool bGenerateMipMaps)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(a_sPath.c_str(), 0); 
	if(fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(a_sPath.c_str());

	if(fif == FIF_UNKNOWN)
		return false;

	if(FreeImage_FIFSupportsReading(fif)) 
		dib = FreeImage_Load(fif, a_sPath.c_str());
	if(!dib)
		return false;

	BYTE* bDataPointer = FreeImage_GetBits(dib); 

	if(bDataPointer == NULL || FreeImage_GetWidth(dib) == 0 || FreeImage_GetHeight(dib) == 0)
		return false;

	GLenum format;
	int bada = FreeImage_GetBPP(dib);
	if(FreeImage_GetBPP(dib) == 32)format = GL_RGBA;
	if(FreeImage_GetBPP(dib) == 24)format = GL_BGR;
	if(FreeImage_GetBPP(dib) == 8)format = GL_LUMINANCE;
	createFromData(bDataPointer, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), FreeImage_GetBPP(dib), format, bGenerateMipMaps);

	FreeImage_Unload(dib);

	sPath = a_sPath;

	return true;
}

void CTexture::setSamplerParameter(GLenum parameter, GLenum value)
{
	glSamplerParameteri(uiSampler, parameter, value);
}

void CTexture::setFiltering(int a_tfMagnification, int a_tfMinification)
{
	if(a_tfMagnification == TEXTURE_FILTER_MAG_NEAREST)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else if(a_tfMagnification == TEXTURE_FILTER_MAG_BILINEAR)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(a_tfMinification == TEXTURE_FILTER_MIN_NEAREST)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	else if(a_tfMinification == TEXTURE_FILTER_MIN_BILINEAR)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	else if(a_tfMinification == TEXTURE_FILTER_MIN_NEAREST_MIPMAP)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	else if(a_tfMinification == TEXTURE_FILTER_MIN_BILINEAR_MIPMAP)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	else if(a_tfMinification == TEXTURE_FILTER_MIN_TRILINEAR)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	tfMinification = a_tfMinification;
	tfMagnification = a_tfMagnification;
}

void CTexture::bindTexture(int iTextureUnit)
{
	glActiveTexture(GL_TEXTURE0+iTextureUnit);
	glBindTexture(GL_TEXTURE_2D, uiTexture);
	glBindSampler(iTextureUnit, uiSampler);

}

void CTexture::releaseTexture()
{
	glDeleteSamplers(1, &uiSampler);
	glDeleteTextures(1, &uiTexture);
}

int CTexture::getMinificationFilter()
{
	return tfMinification;
}

int CTexture::getMagnificationFilter()
{
	return tfMagnification;
}

int CTexture::getWidth()
{
	return iWidth;
}

int CTexture::getHeight()
{
	return iHeight;
}

int CTexture::getBPP()
{
	return iBPP;
}