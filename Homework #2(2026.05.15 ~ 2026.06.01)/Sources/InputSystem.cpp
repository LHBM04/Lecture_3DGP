#include "Precompiled.h"
#include "InputSystem.h"

void InputSystem::Reset()
{
	keyStates.fill(false);
	prevKeyStates.fill(false);
	
	buttonStates.fill(false);
	prevButtonStates.fill(false);
}

void InputSystem::Update()
{
	prevKeyStates = keyStates;
	keyStates.fill(false);

	prevButtonStates = buttonStates;
	buttonStates.fill(false);
}
