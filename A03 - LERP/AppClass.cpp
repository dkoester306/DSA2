#include "AppClass.h"
#include "Simplex/Physics/Octant.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Danny Koester - dsk6539@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}

	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		
		float rotAngle = 360.0f / i;	// how much to increment the angle
		float currentAngle = 0.0f;			// angle where on the unit cirlce the stop point is.

		std::vector<vector3> shapeStopPoints;	// keeps track of all the stop points for the current shape.

		// find the location of each vertex.
		for (int j = 0; j < i; j++)
		{
			float x = fSize * cos(currentAngle*(PI / 180));
			float y = fSize * sin(currentAngle*(PI / 180));
			
			shapeStopPoints.push_back(vector3(x, y, 0.0f));
			currentAngle += rotAngle;
		}

		// add the vector will all the current stops into the global list
		m_allStopPoints.push_back(shapeStopPoints);
		
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
}
void Application::Update(void)
{
	
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system

	//Get a timer
	static double fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

	
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);

	// draw a shapes

	static std::vector<int> stopCounters;	// store all stop counters.
	static std::vector<float> fPercentages;

	if (stopCounters.size() == 0||fPercentages.size()==0)
	{
		for (int i = 0; i < m_uOrbits; i++)
		{
			fPercentages.push_back(0);
			stopCounters.push_back(0);
		}
	}
	

	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));
		
		vector3 v3CurrentPos = ZERO_V3;	//calculate the current position
	
		if (fPercentages[i] >= 1.0f)
		{
			if (stopCounters[i] >= m_allStopPoints[i].size() - 1)
				stopCounters[i] = 0;
			else
				stopCounters[i]++;
			fPercentages[i] = 0;
		}

		if (stopCounters[i] >= m_allStopPoints[i].size()-1)
			v3CurrentPos = glm::lerp(m_allStopPoints[i][stopCounters[i]], m_allStopPoints[i][0], fPercentages[i]);
		else
			v3CurrentPos = glm::lerp(m_allStopPoints[i][stopCounters[i]], m_allStopPoints[i][stopCounters[i] + 1], fPercentages[i]);
		
		// increase the fPercentage value.
		fPercentages[i] += 0.015f;

		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}