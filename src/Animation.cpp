#include "Animation.h"

Animation::Animation(const std::string& name, const sf::Texture& texture)
	: Animation(name, texture, 1, 0) {
}

Animation::Animation(const std::string& name, const sf::Texture& texture, size_t frame_count, int speed)
		: m_name(name), m_sprite(texture), m_frame_count(frame_count), m_current_frame(0), m_speed(speed) {
		m_size = Vec2(
			static_cast<float>(texture.getSize().x / static_cast<unsigned int>(frame_count)),
			static_cast<float>(texture.getSize().y / static_cast<unsigned int>(frame_count))
		);
		m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
		m_sprite.setTextureRect(sf::IntRect(
			static_cast<int>(std::floor(m_current_frame)) * static_cast<int>(m_size.x),
			0,
			static_cast<int>(m_size.x),
			static_cast<int>(m_size.y)
		));
}

void Animation::update() {
	m_frame_count++;
	// Add the speed variable to the current frame
	m_current_frame += m_speed;

	// animFrame = (gameFrame / speed) % frameCount
	// rectangle = (animFrame*FW, 0, WF, FH)
	// sprite.setTextureRect(rectangle)

	// 1) Calculate the correct frame of animation to play based on m_current_frame and m_speed
	// 2) Set the texture rectangle properly (see constructor for sample)
}

bool Animation::hasEnded() {
	// Detect when animation has ended (last frame was played) and return true

	return false;
}
