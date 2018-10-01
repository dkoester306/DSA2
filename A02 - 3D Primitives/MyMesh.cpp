#include "MyMesh.h"

void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	const float POLY_ANGLE = 360 / a_nSubdivisions;
	float angle = POLY_ANGLE;
	std::vector<vector3> allPoints;

	allPoints.reserve(360);

	

	// Calculate all the points/verticies in the polygon
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float xPos = a_fRadius * cos(angle*(PI / 180));
		float yPos = a_fRadius * sin(angle*(PI / 180));

		if (xPos<0 && xPos>-0.0001f)
			xPos = 0.0f;
		if (yPos <0 && yPos>-0.0001f)
			yPos = 0.0f;

		allPoints.push_back(vector3(xPos, 0.0f, yPos));

		angle += POLY_ANGLE;
	}
	vector3 topVertex(0.0f, a_fHeight, 0.0f);

	// Generate all tris for the polygon
	for (int i = 0; i < allPoints.size(); i++)
	{
		if (i == allPoints.size() - 1)
		{
			AddTri(vector3(0.0f, 0.0f, 0.0f), allPoints[i], allPoints[0]);
			AddTri(topVertex, allPoints[i], allPoints[0]);
			AddTri(allPoints[0], allPoints[i],topVertex);
			AddTri(allPoints[0], allPoints[i], vector3(0.0f, 0.0f, 0.0f));
			continue;
		}
		AddTri(vector3(0.0f, 0.0f, 0.0f), allPoints[i], allPoints[i + 1]);
		AddTri(topVertex, allPoints[i], allPoints[i + 1]);
		AddTri(allPoints[i + 1], allPoints[i], topVertex);
		AddTri(allPoints[i + 1], allPoints[i], vector3(0.0f, 0.0f, 0.0f));

	}

	/*
	Calculate a_nSubdivisions number of points around a center point in a radial manner
	then call the AddTri function to generate a_nSubdivision number of faces
	*/

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	const float POLY_ANGLE = 360 / a_nSubdivisions;
	float angle = POLY_ANGLE;
	std::vector<vector3> bottomPoints;
	std::vector<vector3> topPoints;

	bottomPoints.reserve(360);
	topPoints.reserve(360);

	vector3 bottomCenter(0.0f, 0.0f, 0.0f);
	vector3 topCenter(0.0f, a_fHeight, 0.0f);

	// Calculate all the points/verticies in the polygon
	for (int i = 0; i < a_nSubdivisions*2; i++)
	{
		float xPos = a_fRadius * cos(angle*(PI / 180));
		float yPos = a_fRadius * sin(angle*(PI / 180));

		if (xPos<0 && xPos>-0.0001f)
			xPos = 0.0f;
		if (yPos <0 && yPos>-0.0001f)
			yPos = 0.0f;

		
		// calculate faces for bottom circle
		if (i <a_nSubdivisions)
		{
			bottomPoints.push_back(vector3(xPos, 0.0f, yPos));
		}
		// calculate faces for top circle
		else if (i >=a_nSubdivisions)
		{
			topPoints.push_back(vector3(xPos, a_fHeight, yPos));
		}
		angle += POLY_ANGLE;
	}

	// Generate all tris the bottom
	for (int i = 0; i < bottomPoints.size(); i++)
	{
		if (i == bottomPoints.size() - 1)
		{
			AddTri(bottomCenter, bottomPoints[i], bottomPoints[0]);
			AddTri(bottomPoints[0], bottomPoints[i], bottomCenter);
			AddQuad(bottomPoints[0], bottomPoints[i], topPoints[0], topPoints[i]);
			continue;
		}
		AddTri(bottomCenter, bottomPoints[i], bottomPoints[i + 1]);
		AddTri(bottomPoints[i + 1], bottomPoints[i], bottomCenter);
		AddQuad(bottomPoints[i+1], bottomPoints[i], topPoints[i+1], topPoints[i]);
		
	}
	for (int i = 0; i < topPoints.size(); i++)
	{
		if (i == topPoints.size() - 1)
		{
			AddTri(topCenter, topPoints[i], topPoints[0]);
			AddTri(topPoints[0], topPoints[i], topCenter);
			continue;
		}
		AddTri(topCenter, topPoints[i], topPoints[i + 1]);
		AddTri(topPoints[i + 1], topPoints[i], topCenter);
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	// Replace this with your code
	std::vector<vector3> topInPoints;
	std::vector<vector3> topOutPoints;
	std::vector<vector3> bottomInPoints;
	std::vector<vector3> bottomOutPoints;

	const float SUBD_ANGLES = 360 / a_nSubdivisions;
	float angle = SUBD_ANGLES;

	// generate all points
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float xPosOut = a_fOuterRadius * cos(angle*(PI / 180));
		float zPosOut = a_fOuterRadius * sin(angle*(PI / 180));
		float xPosIn = a_fInnerRadius * cos(angle*(PI / 180));
		float zPosIn = a_fInnerRadius * sin(angle*(PI / 180));

		if (xPosOut<0 && xPosOut>-0.0001f)
			xPosOut = 0.0f;
		else if (xPosIn<0 && xPosIn>-0.0001f)
			xPosIn = 0.0f;
		if (zPosOut <0 && zPosOut>-0.0001f)
			zPosOut = 0.0f;
		else if (zPosIn <0 && zPosIn>-0.0001f)
			zPosIn = 0.0f;

		
		bottomOutPoints.push_back(vector3(xPosOut, 0.0f, zPosOut));
		bottomInPoints.push_back(vector3(xPosIn, 0.0f, zPosIn));
		topOutPoints.push_back(vector3(xPosOut, a_fHeight, zPosOut));
		topInPoints.push_back(vector3(xPosIn, a_fHeight, zPosIn));

		angle += SUBD_ANGLES;
	}

	// create the bottom ring of the tube
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i == a_nSubdivisions - 1)
		{
			AddQuad(bottomInPoints[0], bottomInPoints[i], bottomOutPoints[0], bottomOutPoints[i]);
			AddQuad(bottomOutPoints[0], bottomOutPoints[i], topOutPoints[0], topOutPoints[i]);
			AddQuad(bottomInPoints[i], bottomInPoints[0], topInPoints[i], topInPoints[0]);	// cretes outer face.
			AddQuad(topInPoints[i], topInPoints[0], topOutPoints[i], topOutPoints[0]);	// creates bottom ring
			continue;
		}
		AddQuad(bottomInPoints[i+1], bottomInPoints[i], bottomOutPoints[i+1], bottomOutPoints[i]);	// creates bottom ring
		AddQuad(bottomOutPoints[i + 1], bottomOutPoints[i], topOutPoints[i + 1], topOutPoints[i]);	// cretes outer face.
		AddQuad(bottomInPoints[i], bottomInPoints[i+1], topInPoints[i], topInPoints[i+1]);	// cretes innwe face.
		AddQuad(topInPoints[i], topInPoints[i+1], topOutPoints[i], topOutPoints[i+1]);	// creates bottom ring
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	std::vector<std::vector<vector3>> allPoints;

	const float OUTSIDE_ANGLE = 360 / a_nSubdivisionsA;
	const float INSIDE_ANGLE = 360 / a_nSubdivisionsB;

	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		float outAng = OUTSIDE_ANGLE;

		float xPosOut = a_fOuterRadius * cos(outAng*(PI / 180));
		float zPosOut = a_fOuterRadius * sin(outAng*(PI / 180));

		vector3 insideVertex(xPosOut, 0.0f, zPosOut);

		for (int i = 0; i < a_nSubdivisionsB; i++)
		{
			float inAng = INSIDE_ANGLE;

			float xPosIn = a_fInnerRadius * cos(inAng*(PI / 180));
			float yPosIn = a_fInnerRadius * sin(inAng*(PI / 180));


		}
	}
	

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code
	vector3 topVertex(0.0f, a_fRadius * 2, 0.0f);
	vector3 bottomVertex(0.0f, 0.0f, 0.0f);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCircle(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	Release();
	Init();

	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;


	const float POLY_ANGLE = 360 / a_nSubdivisions;
	float angle = POLY_ANGLE;
	std::vector<vector3> allPoints;

	allPoints.reserve(360);

	// Calculate all the points/verticies in the polygon
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float xPos = a_fRadius * cos(angle*(PI / 180));
		float yPos = a_fRadius * sin(angle*(PI / 180));

		if (xPos<0 && xPos>-0.0001f)
			xPos = 0.0f;
		if (yPos <0 && yPos>-0.0001f)
			yPos = 0.0f;

		allPoints.push_back(vector3(xPos, yPos, 0.0f));

		angle += POLY_ANGLE;
	}

	// Generate all tris for the polygon
	for (int i = 0; i < allPoints.size()-1; i++)
	{
		if (i == allPoints.size() - 2)
		{
			AddTri(vector3(0.0f, 0.0f, 0.0f), allPoints[i], allPoints[0]);
			AddTri(allPoints[0], allPoints[i], vector3(0.0f, 0.0f, 0.0f));
			continue;
		}
		AddTri(vector3(0.0f, 0.0f, 0.0f), allPoints[i], allPoints[i + 1]);
		AddTri(allPoints[i + 1], allPoints[i], vector3(0.0f, 0.0f, 0.0f));

	}

	/*
	Calculate a_nSubdivisions number of points around a center point in a radial manner
	then call the AddTri function to generate a_nSubdivision number of faces
	*/

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}