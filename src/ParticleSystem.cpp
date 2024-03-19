#include "ParticleSystem.h"

void ParticleSystem::resetParticle(size_t index, bool first) {
    // Initial position at the center of the screen
    float mx = m_window_size.x / 2;
    float my = m_window_size.y / 2;

    m_vertices[4*index + 0].position = sf::Vector2f(mx, my);
    m_vertices[4*index + 1].position = sf::Vector2f(mx + m_size, my);
    m_vertices[4*index + 2].position = sf::Vector2f(mx + m_size, my + m_size);
    m_vertices[4*index + 3].position = sf::Vector2f(mx, my + m_size);

    // Set color
    sf::Color color(
        55,
        55,
        static_cast<sf::Uint8>(55 + rand()%200),
        static_cast<sf::Uint8>(rand()%255)
    );

    if (first) {
        color.a = 0;
    }

    m_vertices[4*index + 0].color = color;
    m_vertices[4*index + 1].color = color;
    m_vertices[4*index + 2].color = color;
    m_vertices[4*index + 3].color = color;

    // Set random velocity
    float rx = static_cast<float>(static_cast<float>(rand()) / RAND_MAX) * 10.0f - 5.0f;
    float ry = static_cast<float>(static_cast<float>(rand()) / RAND_MAX) * 10.0f - 5.0f;

    m_particles[index].velocity = sf::Vector2f(rx, ry);

    // Set random lifespan
    m_particles[index].lifespan = 30 + rand()%60;
}

void ParticleSystem::resetParticles(size_t count, float size) {
    m_particles = std::vector<Particle>(count);
    m_vertices = sf::VertexArray(sf::Quads, count*4);
    m_size = size;

    size_t particles_size = m_particles.size();
    for (size_t i = 0; i < particles_size; i++) {
        resetParticle(i, true);
    }
}

void ParticleSystem::init(sf::Vector2u window_size) {
    m_window_size = window_size;
    resetParticles();
}

void ParticleSystem::update() {
    for (std::size_t i = 0; i < m_particles.size(); ++i) {
        if (m_particles[i].lifespan == 0) {
            resetParticle(i);
        }

        m_vertices[4*i + 0].position += m_particles[i].velocity;
        m_vertices[4*i + 1].position += m_particles[i].velocity;
        m_vertices[4*i + 2].position += m_particles[i].velocity;
        m_vertices[4*i + 3].position += m_particles[i].velocity;

        m_particles[i].lifespan--;
    }
}

void ParticleSystem::draw(sf::RenderWindow& window) const {
    window.draw(m_vertices);
}
