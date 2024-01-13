#include "Animation.h"

Animation::Animation(const std::string& name, const sf::Texture& texture)
	: Animation(name, texture, 1, 0) {
}

Animation::Animation(const std::string& name, const sf::Texture& texture, size_t frame_count, int speed)
		: m_name(name), m_sprite(texture), m_frame_count(frame_count), m_current_frame(0), m_speed(speed) {
		m_size = Vec2(
			static_cast<float>(texture.getSize().x / static_cast<unsigned int>(frame_count)),
			static_cast<float>(texture.getSize().y)
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
	m_current_frame++;

	auto animation_frame = (m_current_frame / m_speed) % static_cast<int>(m_frame_count);
	m_sprite.setTextureRect(sf::IntRect(
		static_cast<int>(static_cast<int>(animation_frame)) * static_cast<int>(m_size.x),
		0,
		static_cast<int>(m_size.x),
		static_cast<int>(m_size.y)
	));
}

bool Animation::hasEnded() {
	// Detect when animation has ended (last frame was played) and return true
	return static_cast<size_t>(m_current_frame) > m_frame_count;
}
