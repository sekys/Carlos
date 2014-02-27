
#include "common_header.h"
#include "objModel.h"

CObjModel::CObjModel()
{
	bLoaded = false;
	iAttrBitField = 0;
}
void CObjModel::setTexture(CTexture texture) {
	tAmbientTexture = texture;
}
CTexture CObjModel::getTexture() {
	return tAmbientTexture;
}
vector<string> split(string s, string t)
{
	vector<string> res;
	while(1)
	{
		int pos = s.find(t);
		if(pos == -1){res.push_back(s); break;}
		res.push_back(s.substr(0, pos));
		s = s.substr(pos+1, ESZ(s)-pos-1);
	}
	return res;

}

string getDirectoryPath(string sFilePath)
{

	string sDirectory = "";
	RFOR(i, ESZ(sFilePath)-1)if(sFilePath[i] == '\\' || sFilePath[i] == '/')
	{
		sDirectory = sFilePath.substr(0, i+1);
		break;
	}
	return sDirectory;
}


bool CObjModel::loadModel(string sFileName, string sMtlFileName)
{
	FILE* fp = fopen(sFileName.c_str(), "rt");

	if(fp == NULL)return false;

	char line[255];

	vector<glm::vec3> vVertices;
	vector<glm::vec2> vTexCoords;
	vector<glm::vec3> vNormals;

	iNumFaces = 0;

	while(fgets(line, 255, fp))
	{

		bool bError = false;

		if(strlen(line) <= 1)
			continue;

		stringstream ss(line);
		string sType;
		ss >> sType;

		if(sType == "#")
			continue;

		else if(sType == "v")
		{
			glm::vec3 vNewVertex;
			int dim = 0;
			while(dim < 3 && ss >> vNewVertex[dim])dim++;
			vVertices.push_back(vNewVertex);
			iAttrBitField |= 1;
		}

		else if(sType == "vt")
		{
			glm::vec2 vNewCoord;
			int dim = 0;
			while(dim < 2 && ss >> vNewCoord[dim])dim++;
			vTexCoords.push_back(vNewCoord);
			iAttrBitField |= 2;
		}

		else if(sType == "vn")
		{
			glm::vec3 vNewNormal;
			int dim = 0;
			while(dim < 3 && ss >> vNewNormal[dim])dim++;
			vNewNormal = glm::normalize(vNewNormal);
			vNormals.push_back(vNewNormal);
			iAttrBitField |= 4;
		}

		else if(sType == "f")
		{
			string sFaceData;

			while(ss >> sFaceData)
			{
				vector<string> data = split(sFaceData, "/");
				int iVertIndex = -1, iTexCoordIndex = -1, iNormalIndex = -1;

				if(iAttrBitField&1)
				{
					if(ESZ(data[0]) > 0)sscanf(data[0].c_str(), "%d", &iVertIndex);
					else bError = true;
				}

				if(iAttrBitField&2 && !bError)
				{
					if(ESZ(data) >= 1)
					{

						if(ESZ(data[1]) > 0)sscanf(data[1].c_str(), "%d", &iTexCoordIndex);
						else bError = true;
					}
					else bError = true;
				}

				if(iAttrBitField&4 && !bError)
				{
					if(ESZ(data) >= 2)
					{
						if(ESZ(data[2]) > 0)sscanf(data[2].c_str(), "%d", &iNormalIndex);
						else bError = true;
					}
					else bError = true;
				}
				if(bError)
				{
					fclose(fp);
					return false;
				}


				if(iVertIndex > 0 && iVertIndex <= ESZ(vVertices))
					vboModelData.addData(&vVertices[iVertIndex-1], sizeof(glm::vec3));
				if(iTexCoordIndex > 0 && iTexCoordIndex <= ESZ(vTexCoords))
					vboModelData.addData(&vTexCoords[iTexCoordIndex-1], sizeof(glm::vec2));
				if(iNormalIndex > 0 && iNormalIndex <= ESZ(vNormals))
					vboModelData.addData(&vNormals[iNormalIndex-1], sizeof(glm::vec3));
			}
			iNumFaces++;
		}

		else if(sType == "s")
		{

		}

		else if(sType == "usemtl")
		{

		}
	}

	fclose(fp);

	if(iAttrBitField == 0)
		return false;

	vboModelData.createVBO();
	vboModelData.bindVBO();

	vboModelData.uploadDataToGPU(GL_STATIC_DRAW);

	glGenVertexArrays(1, &uiVAO); 
	glBindVertexArray(uiVAO);
	int iDataStride = 0;
	if(iAttrBitField&1)iDataStride += sizeof(glm::vec3);
	if(iAttrBitField&2)iDataStride += sizeof(glm::vec2);
	if(iAttrBitField&4)iDataStride += sizeof(glm::vec3);

	if(iAttrBitField&1)
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, iDataStride, 0);
	}

	if(iAttrBitField&2)
	{
		glEnableVertexAttribArray(1);
		int iDataOffset = 0;
		if(iAttrBitField&1)iDataOffset += sizeof(glm::vec3);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, iDataStride, (void*)iDataOffset);
	}

	if(iAttrBitField&4)
	{
		glEnableVertexAttribArray(2);
		int iDataOffset = 0;
		if(iAttrBitField&1)iDataOffset += sizeof(glm::vec3);
		if(iAttrBitField&2)iDataOffset += sizeof(glm::vec2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, iDataStride, (void*)iDataOffset);
	}
	bLoaded = true;

	loadMaterial(getDirectoryPath(sFileName)+sMtlFileName);

	return true;
}

void CObjModel::renderModel()
{
	if(!bLoaded)return;
	glBindVertexArray(uiVAO);
	tAmbientTexture.bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, iNumFaces*3);
}


bool CObjModel::loadMaterial(string sFullMtlFileName)
{

	FILE* fp = fopen(sFullMtlFileName.c_str(), "rt");

	if(fp == NULL)return false;

	char line[255];

	while(fgets(line, 255, fp))
	{
		stringstream ss(line);
		string sType;
		ss >> sType;
		if(sType == "map_Kd")
		{
			string sLine = line;

			int from = sLine.find("map_Kd")+6+1;
			string sTextureName = sLine.substr(from, ESZ(sLine)-from-1);

			tAmbientTexture.loadTexture2D(getDirectoryPath(sFullMtlFileName)+sTextureName, true);
			tAmbientTexture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_NEAREST_MIPMAP);
			break;
		}
	}
	fclose(fp);

	return true;
}

void CObjModel::releaseModel()
{
	if(!bLoaded)return;
	tAmbientTexture.releaseTexture();
	glDeleteVertexArrays(1, &uiVAO);
	vboModelData.releaseVBO();
	bLoaded = false;
}

int CObjModel::getPolygonCount()
{
	return iNumFaces;
}