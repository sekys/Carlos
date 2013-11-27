#pragma once
#include "common_header.h"

enum ETextureFiltering
{
	TEXTURE_FILTER_MAG_NEAREST = 0, 
	TEXTURE_FILTER_MAG_BILINEAR,
	TEXTURE_FILTER_MIN_NEAREST,
	TEXTURE_FILTER_MIN_BILINEAR,
	TEXTURE_FILTER_MIN_NEAREST_MIPMAP,
	TEXTURE_FILTER_MIN_BILINEAR_MIPMAP,
	TEXTURE_FILTER_MIN_TRILINEAR,
};

class CTexture
{
public:
	void createFromData(BYTE* bData, int a_iWidth, int a_iHeight, int a_iBPP, GLenum format, bool bGenerateMipMaps = false);
	void createFromData(BYTE* bData, int a_iWidth, int a_iHeight, int a_iBPP, GLenum internalColourFormat, GLenum inputColourFormat, bool bGenerateMipMaps);
	bool loadTexture2D(string a_sPath, bool bGenerateMipMaps = false);
	void bindTexture(int iTextureUnit = 0);

	void setFiltering(int a_tfMagnification, int a_tfMinification);

	void setSamplerParameter(GLenum parameter, GLenum value);

	int getMinificationFilter();
	int getMagnificationFilter();

	int getWidth();
	int getHeight();
	int getBPP();

	void releaseTexture();

	CTexture();
private:
	int iWidth, iHeight, iBPP; 
	UINT uiTexture; 
	UINT uiSampler; 
	bool bMipMapsGenerated;

	int tfMinification, tfMagnification;

	string sPath;
};

