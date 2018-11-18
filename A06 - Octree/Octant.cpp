#include "Simplex/Physics/Octant.h"
using namespace Simplex;

// Constructor
Octant::Octant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	Init();
	m_pRoot = this;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;

	// set the max and min vectors to the center coordinates to start.
	m_v3Max = m_pEntityMngr->GetRigidBody()->GetCenterGlobal();
	m_v3Min = m_pEntityMngr->GetRigidBody()->GetCenterGlobal();

	// only runs if there is at least 1 entity
	for (uint i = 0; i < m_EntityList.size(); i++)
	{
		// add the first entity index to the list
		m_EntityList.push_back(i);

		// get the max and min of the added entity
		vector3 max = m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal();
		vector3 min = m_pEntityMngr->GetRigidBody(i)->GetMinGlobal();

		// Set new max and min values for the rigidbody
		// Set X values
		if (max.x > m_v3Max.x)
		{
			m_v3Max.x = max.x;
		}
		if (min.x > m_v3Min.x)
		{
			m_v3Min.x = min.x;
		}

		// Set Y values
		if (max.y > m_v3Max.y)
		{
			m_v3Max.y = max.y;
		}
		if (min.y < m_v3Min.y)
		{
			m_v3Min.y = min.y;
		}

		// Set Z values
		if (max.z > m_v3Max.z)
		{
			m_v3Max.z = max.z;
		}
		if (min.z < m_v3Min.z)
		{
			m_v3Min.z = min.z;
		}

	}

	m_v3Center = (m_v3Max + m_v3Min) / 2.0f;	// set the new center of the octant
	m_vSize = m_v3Max - m_v3Min;				// set corresponding size to octant
}

// Constructor using the center and size vectors
Octant::Octant(vector3 a_v3Center, vector3 a_vSize)
{
	Init();
	m_vSize = a_vSize;
	m_v3Center = a_v3Center;
	m_v3Max = m_v3Center + m_vSize / 2.f;
	m_v3Min = m_v3Center - m_vSize / 2.f;

	
}

// Copy Constructor
Octant::Octant(Octant const& other)
{
	Init();

	m_v3Center = other.m_v3Center;
	m_v3Max = other.m_v3Max;
	m_v3Min = other.m_v3Min;
	m_vSize = other.m_vSize;
	m_pParent = other.m_pParent;
	m_uLevel = other.m_uLevel;
	

	Release();	
	for (int i = 0; i < m_EntityList.size(); i++)
	{
		m_EntityList.push_back(other.m_EntityList[i]);
	}

	m_pRoot = other.m_pRoot;
	if (m_pRoot == this)
	{
		for (int i = 0; i < other.m_lChild.size(); i++)
		{
			m_lChild.push_back(other.m_lChild[i]);
		}
	}
	


	
}




// destructor
Octant::~Octant(void)
{
	Release();
}

// Swap method
void Octant::Swap(Octant &other)
{

}

#pragma region Getters
uint Octant::GetOctantCount(void)
{
	return m_uOctantCount;
}
// return the size
vector3 Octant::GetSize()
{
	return m_vSize;
}

vector3 Octant::GetCenterGlobal()
{
	return m_v3Center;
}

vector3 Octant::GetMaxGlobal()
{
	return m_v3Max;
}

vector3 Octant::GetMinGlobal()
{
	return m_v3Min;
}

Octant* Octant::GetChild(uint a_nChild)
{
	if (m_uChildren <= 0)
		return nullptr;
	else
		return m_pChild[a_nChild];
}

Octant* Octant::GetParent(void)
{
	return m_pParent;
}
#pragma endregion


bool Octant::IsColliding(uint a_uRBIndex)
{
	RigidBody* myRB = m_pEntityMngr->GetRigidBody(a_uRBIndex);	// get the rigid body at the specified index

	// get the global max and min coordinates for the rigidbody
	vector3 max = myRB->GetMaxGlobal();
	vector3 min = myRB->GetMinGlobal();

	// check if x of rigidbody is colliding
	if (max.x < m_v3Min.x)
	{
		return false;
	}
	if (min.x > m_v3Max.x)
	{
		return false;
	}

	// check if y of rigidbody is colliding
	if (max.y < m_v3Min.y)
	{
		return false;
	}
	if (min.y > m_v3Max.y)
	{
		return false;
	}

	// check if z of rigidboyd is colliding
	if (max.z < m_v3Min.z)
	{
		return false;
	}
	if (min.z > m_v3Max.z)
	{
		return false;
	}
	return true;
}

void Octant::Display(uint a_nIndex, vector3 a_v3Color)
{
	// display all the leaves if index overflows or is equal to the octant count
	if (m_uOctantCount <= a_nIndex)
		DisplayLeaves(a_v3Color);	// call other display method.
	else
		m_lChild[a_nIndex]->Display(a_v3Color);		// recursive call to display

}


void Octant::Display(vector3 a_v3Color)
{
	// add a wire cube at the center of the octant with the correct size.
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center)*glm::scale(IDENTITY_M4, m_vSize), a_v3Color);
}

void Octant::DisplayLeaves(vector3 a_v3Color)
{
	// if this octant is already a leaf, display it immediately
	if (IsLeaf())
	{
		Display(a_v3Color);
	}
	else  // if not a leaf, call display methods of all children.
	{
		for (int i = 0; i < m_uChildren; i++)
		{
			m_pChild[i]->DisplayLeaves(a_v3Color);	// recursive call 
		}
	}
}

void Octant::ClearEntityList(void)
{
	// clear the entity list for all of the children
	for each(Octant* o in m_pChild)
	{
		o->ClearEntityList();
	}

	// double check and clear the root entity list
	m_EntityList.clear();
}

void Octant::Subdivide(void)
{
	// if this octant is not a leaf, go back.
	if (m_uChildren == 8)
	{
		return;
	}
	// if the current level is greater or equal than the max level, add the subdivisions to the root and exit
	if (m_uMaxLevel <= m_uLevel)
	{
		m_pRoot->m_lChild.push_back(this);
		return;
	}

	uint xNeg, yNeg, zNeg = 1;

	for (int i = 0; i < 8; i++)
	{
		switch (i)
		{
		case 0:
			xNeg, zNeg = -1;
			yNeg = 1;
			break;

		case 1:
			xNeg = -1;
			yNeg, zNeg = 1;
			break;

		case 2:
			xNeg, yNeg, zNeg = -1;
			break;

		case 3:
			xNeg, yNeg = -1;
			zNeg = 1;
			break;
			
		case 4:
			xNeg = -1;
			yNeg, zNeg = 1;
			break;

		case 5:
			yNeg = -1;
			xNeg, zNeg = 1;
			break;

		case 6:
			zNeg = -1;
			xNeg, yNeg = 1;
			break;

		case 7:
			xNeg, yNeg, zNeg = 1;
			break;
		}

		m_pChild[i] = new Octant(m_v3Center + vector3((xNeg*m_vSize.x) / 4.0f, (yNeg*m_vSize.y) / 4.0f, (zNeg*m_vSize.z) / 4.0f), m_vSize / 2.0f);

	}

	// initialize the values of the children
	for (int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_uLevel = m_uLevel + 1;

		// add proper rigidbodies that are collidiing
		for (int j = 0; i < m_EntityList.size(); i++)
		{
			if (m_pChild[i]->IsColliding(m_EntityList[j]))
				m_pChild[i]->m_EntityList.push_back(m_EntityList[j]);
		}

		m_pChild[i]->Subdivide();	// recursive call
	}
	
}

bool Octant::IsLeaf(void)
{
	if (m_uChildren == 0)
		return true;
	return false;
}

bool Octant::ContainsMoreThan(uint a_nEntities)
{
	if (m_EntityList.size() > a_nEntities)
		return true;
	return false;
}

void Octant::KillBranches(void)
{
	// check if the octant is a leaf or not
	if (IsLeaf())
		return;

	for each(Octant* o in m_pChild)
	{
		o->KillBranches();
		SafeDelete(o);
	}
}

void Octant::ConstructTree(uint a_nMaxLevel)
{

}

void Octant::AssignIDtoEntity(void)
{

}


#pragma region Private Methods
void Octant::Release(void)
{
	// if the current object is the root, kill the rest of the branches
	if (this == m_pRoot)
		KillBranches();
}

void Octant::Init(void)
{
	// set id of octant, then increase count by 1
	m_uID = m_uOctantCount;
	m_uOctantCount++;

	// set both the mesh and entity managers
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();
}

void Octant::ConstructList(void)
{
	// check if we are at a leaf
	if (IsLeaf())
	{
		for each (uint i in m_EntityList)
		{
			// add each entity
			m_pEntityMngr->AddDimension(i, m_uID);
		}
	}
	// recursive call for each child of this octant
	else
	{
		for(uint i=0;i<m_uChildren;i++)
		{
			m_pChild[i]->ConstructList();
		}
	}
}
#pragma endregion





