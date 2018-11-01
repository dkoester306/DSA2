#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
	m_entityList.clear();
	m_uEntityCount = 0;
}
void MyEntityManager::Release(void)
{
	for (uint i = 0; i < m_uEntityCount; i++)
	{
		MyEntity* currentEnt = m_entityList[i];
		SafeDelete(currentEnt);
	}
	Init();	// reinitalize members
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	for (int i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID()==a_sUniqueID)
			return i;
	}
	return 0;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	// check if list is empty
	if (m_uEntityCount == 0)
		return nullptr;
	// out of bounds exception
	if(a_uIndex>=m_uEntityCount)
		return m_entityList[m_uEntityCount-1]->GetModel();

	return m_entityList[a_uIndex]->GetModel();
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	MyEntity* refE = MyEntity::GetEntity(a_sUniqueID);	// get the model with that unique ID

	if (refE == nullptr)
		return nullptr;

	return refE->GetModel();
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	// check if list is empty
	if (m_uEntityCount == 0)
		return nullptr;
	// out of bounds exception
	if (a_uIndex >= m_uEntityCount)
		return m_entityList[m_uEntityCount - 1]->GetRigidBody();

	return m_entityList[a_uIndex]->GetRigidBody();
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	MyEntity* refE = MyEntity::GetEntity(a_sUniqueID);

	if (refE == nullptr)
		return nullptr;

	return refE->GetRigidBody();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	// check if list is empty
	if (m_uEntityCount == 0)
		return IDENTITY_M4;
	// out of bounds exception
	if (a_uIndex >= m_uEntityCount)
		return m_entityList[m_uEntityCount - 1]->GetModelMatrix();

	return m_entityList[a_uIndex]->GetModelMatrix();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	MyEntity* refE = MyEntity::GetEntity(a_sUniqueID);

	if (refE == nullptr)
		return IDENTITY_M4;

	return refE->GetModelMatrix();
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	// check if list is empty
	if (m_uEntityCount == 0)
		return;
	// out of bounds exception
	if (a_uIndex >= m_uEntityCount)
		return m_entityList[m_uEntityCount - 1]->SetModelMatrix(a_m4ToWorld);

	m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	MyEntity* refE = MyEntity::GetEntity(a_sUniqueID);

	if (refE == nullptr)
		return;

	refE->SetModelMatrix(a_m4ToWorld);

}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	// check if the any entities are colliding
	for (int i = 0; i < m_uEntityCount; i++)
	{
		for (int j = 0; j < m_uEntityCount; j++)
			m_entityList[i]->IsColliding(m_entityList[j]);
	}	// currently not working
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	MyEntity* refE = new MyEntity(a_sFileName, a_sUniqueID);

	// check if the object initialized properly
	if (refE->IsInitialized())
	{
		m_entityList.push_back(refE);
		m_uEntityCount++;
	}
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	uint index = a_uIndex;
	if (m_uEntityCount == 0)
		return;

	if (a_uIndex >= m_uEntityCount)
		index = m_uEntityCount - 1;

	MyEntity* refE = m_entityList[index];
	SafeDelete(refE);
	m_entityList.erase(m_entityList.begin() + index);
	m_uEntityCount--;
	return;

}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	int index = GetEntityIndex(a_sUniqueID);
	RemoveEntity((uint)index);
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	if (m_uEntityCount == 0)
		return "";

	if (a_uIndex >= m_uEntityCount)
		return m_entityList[m_uEntityCount-1]->GetUniqueID();

	return m_entityList[a_uIndex]->GetUniqueID();
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	if (m_uEntityCount == 0)
		return nullptr;

	if (a_uIndex >= m_uEntityCount)
		return m_entityList[m_uEntityCount - 1];

	return m_entityList[a_uIndex];
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	// if index is out of bounds, just add all the objects.
	if (a_uIndex >= m_uEntityCount)
	{
		for (int i = 0; i < m_uEntityCount; i++)
			m_entityList[i]->AddToRenderList(a_bRigidBody);
		return;
	}
	m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	MyEntity* refE = MyEntity::GetEntity(a_sUniqueID);

	if (refE == nullptr)
		return;

	refE->AddToRenderList(a_bRigidBody);
}