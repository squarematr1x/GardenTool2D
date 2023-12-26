#include "Animation.h"

void Animation::update()
{
	m_game_frame++;
	// Add the speed variable to the current frame
	m_current_frame += m_speed;

	// animFrame = (gameFrame / speed) % frameCount
	// rectangle = (animFrame*FW, 0, WF, FH)
	// sprite.setTextureRect(rectangle)

	// 1) Calculate the correct frame of animation to play based on m_current_frame and m_speed
	// 2) Set the texture rectangle properly (see constructor for sample)
}

bool Animation::hasEnded()
{
	// Detect when animation has ended (last frame was played) and return true

	return false;
}
