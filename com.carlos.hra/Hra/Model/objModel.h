#pragma once

#include "texture.h"
#include "vertexBufferObject.h"


class CObjModel
{
public:
	bool loadModel(string sFileName, string sMtlFileName);
	void renderModel();
	void releaseModel();

	int getPolygonCount();
	void setTexture(CTexture texture);
	CTexture getTexture();

	CObjModel();
private:
	bool bLoaded;
	int iAttrBitField;
	int iNumFaces;

	bool loadMaterial(string sFullMtlFileName);

	CVertexBufferObject vboModelData;
	UINT uiVAO;
	CTexture tAmbientTexture;
};