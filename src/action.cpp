#include "animation.hpp"

Animation::Animation(const std::string& name, const sf::Texture& texture)
	: Animation(name, texture, 1, 0) {
}

Animation::Animation(const std::string& name, const sf::Texture& texture, size_t frame_count, int speed, Vec2 size, Vec2 offset)
	: m_name(name), m_sprite(texture), m_frame_count(frame_count), m_current_frame(0), m_speed(speed), m_offset(offset) {
		m_size = Vec2(
			static_cast<float>(size.x / static_cast<unsigned int>(frame_count)),
			static_cast<float>(size.y)
		);
		m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
		m_sprite.setTextureRect(sf::IntRect(
			static_cast<int>(m_offset.x) + static_cast<int>(std::floor(m_current_frame)) * static_cast<int>(m_size.x),
			static_cast<int>(m_offset.y),
			static_cast<int>(m_size.x),
			static_cast<int>(m_size.y)
		));
}

void Animation::update() {
	if (m_frame_count == 0) {
		return;
	}
	m_current_frame++;

	auto animation_frame = (m_current_frame / m_speed) % static_cast<int>(m_frame_count);
	m_sprite.setTextureRect(sf::IntRect(
		static_cast<int>(m_offset.x) + animation_frame * static_cast<int>(m_size.x),
		static_cast<int>(m_offset.y),
		static_cast<int>(m_size.x),
		static_cast<int>(m_size.y)
	));
}

bool Animation::hasEnded() {
	return static_cast<size_t>(m_current_frame / m_speed) >= m_frame_count;
}
