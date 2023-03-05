#include "pch.h"
#include "Input.h"


Input::Input()
{
}

Input::~Input()
{
}

void Input::Initialise(HWND window)
{
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(window);
	m_quitApp = false;

	m_GameInput.forward		= false;
	m_GameInput.back		= false;
	m_GameInput.right		= false;
	m_GameInput.left		= false;
	//m_GameInput.rotRight	= false;
	//m_GameInput.rotLeft	= false;
	m_GameInput.moveUp		= false;
	m_GameInput.moveDown	= false;
	
	prevX = 0;
	xChange = 0;
}

void Input::Update()
{
	
	auto kb = m_keyboard->GetState();	//updates the basic keyboard state
	m_KeyboardTracker.Update(kb);		//updates the more feature filled state. Press / release etc. 
	auto mouse = m_mouse->GetState();   //updates the basic mouse state
	m_MouseTracker.Update(mouse);		//updates the more advanced mouse state. 

	if (kb.Escape)// check has escape been pressed.  if so, quit out. 
	{
		m_quitApp = true;
	}

	//A key
	if (kb.A)	m_GameInput.left = true;
	else		m_GameInput.left = false;
	
	//D key
	if (kb.D)	m_GameInput.right = true;
	else		m_GameInput.right = false;

	//W key
	if (kb.W)	m_GameInput.forward	 = true;
	else		m_GameInput.forward = false;

	//S key
	if (kb.S)	m_GameInput.back = true;
	else		m_GameInput.back = false;
    
	/*
	m_GameInput.rotLeft = true;
	float newX = mouse.x;
	if (prevX != 0) {
		if (newX != prevX) {
			xChange = newX - prevX;
		}
	}
	prevX = mouse.x;
	*/
	if (kb.X)	m_GameInput.moveUp = true;
	else m_GameInput.moveUp = false;

	if (kb.Z)	m_GameInput.moveDown = true;
	else m_GameInput.moveDown = false;
	
	if (m_KeyboardTracker.pressed.T)  m_GameInput.toggleRoomba = true;
	else m_GameInput.toggleRoomba = false;
}

bool Input::Quit()
{
	return m_quitApp;
}

InputCommands Input::getGameInput()
{
	return m_GameInput;
}
float Input::getXChange() {
	return xChange;
}

