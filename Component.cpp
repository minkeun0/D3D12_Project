#include "Component.h"
#include "Object.h"
#include "Scene.h"

void StateMachine::HandleKeyBuffer()
{
	auto keyBuffer = mRoot->GetParent()->GetKeyBuffer();

	if (keyBuffer[static_cast<int>(eKeyTable::Up)] == eKeyState::Pressed ||
		keyBuffer[static_cast<int>(eKeyTable::Down)] == eKeyState::Pressed ||
		keyBuffer[static_cast<int>(eKeyTable::Left)] == eKeyState::Pressed ||
		keyBuffer[static_cast<int>(eKeyTable::Right)] == eKeyState::Pressed)
	{
		mEventQueue.push(eEvent::MoveKeyPressed);
	}

	if (keyBuffer[static_cast<int>(eKeyTable::Up)] == eKeyState::Released &&
		keyBuffer[static_cast<int>(eKeyTable::Down)] == eKeyState::Released &&
		keyBuffer[static_cast<int>(eKeyTable::Left)] == eKeyState::Released &&
		keyBuffer[static_cast<int>(eKeyTable::Right)] == eKeyState::Released)
	{
		mEventQueue.push(eEvent::MoveKeyReleased);
	}

	if (keyBuffer[static_cast<int>(eKeyTable::Shift)] == eKeyState::Pressed) {
		mEventQueue.push(eEvent::ShiftKeyPressed);
	}

	if (keyBuffer[static_cast<int>(eKeyTable::Shift)] == eKeyState::Released) {
		mEventQueue.push(eEvent::ShiftKeyReleased);
	}
}

void StateMachine::HandleEventQueue()
{
	while (mEventQueue.size() > 0) {
		auto it = mTransitions[mCurrentState].find(mEventQueue.front());
		if (it != mTransitions[mCurrentState].end()) {
			mCurrentState = it->second;
			mEventQueue.pop();
			break;
		}
		mEventQueue.pop();
	}
}
