#pragma once
#include <glm/glm.hpp>

class CShader
{
public:
	bool loadShader(string sFile, int a_iType);
	void deleteShader();

	bool getLinesFromFile(string sFile, bool bIncludePart, vector<string>* vResult);

	bool isLoaded();
	UINT getShaderID();

	CShader();

private:
	UINT uiShader; 
	int iType; 
	bool bLoaded; 
};

class CShaderProgram
{
public:
	void createProgram();
	void deleteProgram();

	bool addShaderToProgram(CShader* shShader);
	bool linkProgram();

	void useProgram();

	UINT getProgramID();


	void setUniform(string sName, glm::vec2* vVectors, int iCount = 1);
	void setUniform(string sName, const glm::vec2 vVector);
	void setUniform(string sName, glm::vec3* vVectors, int iCount = 1);
	void setUniform(string sName, const glm::vec3 vVector);
	void setUniform(string sName, glm::vec4* vVectors, int iCount = 1);
	void setUniform(string sName, const glm::vec4 vVector);


	void setUniform(string sName, float* fValues, int iCount = 1);
	void setUniform(string sName, const float fValue);

	void setUniform(string sName, glm::mat3* mMatrices, int iCount = 1);
	void setUniform(string sName, const glm::mat3 mMatrix);

	void setUniform(string sName, glm::mat4* mMatrices, int iCount = 1);
	void setUniform(string sName, const glm::mat4 mMatrix);

	void setUniform(string sName, int* iValues, int iCount = 1);
	void setUniform(string sName, const int iValue);

	CShaderProgram();

private:
	UINT uiProgram; 
	bool bLinked; 
};