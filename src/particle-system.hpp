#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

class ParticleSystem
{
    struct Particle
    {
        sf::Vector2f velocity;
        int lifespan{ 0 };
    };

    std::vector<Particle> m_particles;
    sf::VertexArray m_vertices;
    sf::Vector2u m_window_size;
    sf::Vector2f m_emitter; // Emitter could be the mouse position
    float m_size{ 8.0f };

    void resetParticle(size_t index, bool first = false);
    void resetParticles(size_t count = 1024, float size = 8);

public:
    void init(sf::Vector2u window_size);
    void update();
    void draw(sf::RenderWindow& window) const;
};
