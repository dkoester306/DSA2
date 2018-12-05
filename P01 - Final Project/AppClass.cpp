#include "AppClass.h"
using namespace Simplex;
MyMesh* m_block;
float f_cameraSpeed = .1;
int i_gameTick;

std::vector<vector3> v_v3blockPositions;

void Application::ApplyForceToPlayer(vector3 a_force)
{
	float time = static_cast<float>(i_gameTick % 60) / 100;
	if (time == 0) time = 1.0f;
	m_v3PlayerAcceleration += a_force; //Player mass is 1
	m_v3PlayerVelocity += m_v3PlayerAcceleration;

	//Clamp velocity
	if (m_v3PlayerVelocity.y > 1.5f) m_v3PlayerVelocity.y = 1.5f;
	else if (m_v3PlayerVelocity.y < -1.5f) m_v3PlayerVelocity.y = -1.5f;

	if (m_v3PlayerVelocity.x > 0.5f) m_v3PlayerVelocity.x = 0.5f;
	else if (m_v3PlayerVelocity.x < -0.5f) m_v3PlayerVelocity.x = -0.5f;

	if (m_v3PlayerVelocity.z > 0.5f) m_v3PlayerVelocity.z = 0.5f;
	else if (m_v3PlayerVelocity.z < -0.5f) m_v3PlayerVelocity.z = -0.5f;

	m_v3PlayerPosition += m_v3PlayerVelocity * time;

	//Reset acceleration
	m_v3PlayerAcceleration = vector3(0.0f);
	//pass the player's y-velocity to the camera
	FollowCamera(m_v3PlayerVelocity.y);
}

void Application::FollowCamera(float a_fForce)
{
	//if the player's y-velocity is positive (player is moving up), camera follows them up
	if (a_fForce > 0 && m_v3PlayerPosition.y >= m_pCamera->GetPosition().y + 6.0f) {
		m_pCamera->MoveVertical(a_fForce);
	}
}

void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Team \"Notorious Squad\" - Final Project";

	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 10.0f, ZERO_V3, AXIS_Y);

	//init the camera
	m_pCamera = new MyCamera();
	m_pCamera->SetPositionTargetAndUpward(
		vector3(0.0f, 3.0f, 20.0f), //Where my eyes are
		vector3(0.0f, 3.0f, 19.0f), //where what I'm looking at is
		AXIS_Y);					//what is up

//Get the singleton
	m_pMyMeshMngr = MyMeshManager::GetInstance();
	m_pMyMeshMngr->SetCamera(m_pCamera);

	v_v3blockPositions.push_back(m_pCamera->GetPosition() + vector3(0, 15, -20));

	m_v3PlayerPosition = vector3(m_pCamera->GetPosition() + vector3(0, 0, -25));
	m_v3PlayerVelocity = vector3(0.0f);
	m_v3PlayerAcceleration = vector3(0.0f);
	m_v3Gravity = vector3(0.0f, -0.01f, 0.0f);
}
void Application::Update(void)
{
	if (m_v3PlayerVelocity.x < 0.01f) isMovingRight = false;
	if (m_v3PlayerVelocity.x > -0.01f) isMovingLeft = false;
	if (m_v3PlayerVelocity.y < 0.0f) isJumping = false;

	i_gameTick++;

	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	//CameraRotation();

	if (i_gameTick % 50 == 0)
	{
		float f_randomX = rand() % 10;
		vector3 position = vector3(f_randomX - 5, 0, 0);
		v_v3blockPositions.push_back(m_pCamera->GetPosition() + vector3(0, 15, -20) + position);

		if (v_v3blockPositions.size() > 6)
		{
			v_v3blockPositions.erase(v_v3blockPositions.begin());
		}
	}

	for (auto i = 0; i < v_v3blockPositions.size(); i++)
	{
		vector3 position = v_v3blockPositions[i];
		m_pMyMeshMngr->AddCubeToRenderList(glm::translate(position) * glm::scale(vector3(10, 1, 1)));
	}

	

	//Add the player to the render list and apply "gravity"
	m_pMyMeshMngr->AddCubeToRenderList(glm::translate(m_v3PlayerPosition) * glm::scale(vector3(3)));
	ApplyForceToPlayer(m_v3Gravity);

	//If the player falls under the camera, game is over

	m_pCamera->MoveVertical(f_cameraSpeed);

}
void Application::Display(void)
{
	//Clear the screen
	ClearScreen();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//Render the list of MyMeshManager
	m_pMyMeshMngr->Render();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the MyMeshManager list
	m_pMyMeshMngr->ClearRenderList();

	//draw gui
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release the singleton
	MyMeshManager::ReleaseInstance();

	//release the camera
	SafeDelete(m_pCamera);

	//release GUI
	ShutdownGUI();
}