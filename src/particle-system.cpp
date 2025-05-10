#include "particle-system.hpp"

#include "core/render-window.hpp"

void ParticleSystem::resetParticle(size_t index, bool first) {
    // Initial position at the center of the screen
    const auto mx = m_emitter.x;
    const auto my = m_emitter.y;

    m_vertices.setPosAt(6*index + 0, Vec2(mx, my));
    m_vertices.setPosAt(6*index + 1, Vec2(mx + m_size, my));
    m_vertices.setPosAt(6*index + 2, Vec2(mx + m_size, my + m_size));
    m_vertices.setPosAt(6*index + 3, Vec2(mx, my));
    m_vertices.setPosAt(6*index + 4, Vec2(mx + m_size, my + m_size));
    m_vertices.setPosAt(6*index + 5, Vec2(mx, my + m_size));

    // Set color
    Color color(
        55,
        55,
        static_cast<channel>(55 + rand()%200),
        static_cast<channel>(rand()%255)
    );

    if (first) {
        color.setA(0);
    }

    m_vertices.setColorAt(6*index + 0, color);
    m_vertices.setColorAt(6*index + 1, color);
    m_vertices.setColorAt(6*index + 2, color);
    m_vertices.setColorAt(6*index + 3, color);
    m_vertices.setColorAt(6*index + 4, color);
    m_vertices.setColorAt(6*index + 5, color);

    // Set random velocity
    const auto rx = static_cast<float>(static_cast<float>(rand())/RAND_MAX)*10.0f - 5.0f;
    const auto ry = static_cast<float>(static_cast<float>(rand())/RAND_MAX)*10.0f - 5.0f;

    m_particles[index].velocity = Vec2(rx, ry);

    // Set random lifespan
    m_particles[index].lifespan = 30 + rand() % 60;
}

void ParticleSystem::resetParticles(size_t count, float size) {
    m_particles = std::vector<Particle>(count);
    m_vertices = VertexArray(TRIANGLE, count*6);
    m_size = size;

    const auto particles_size = m_particles.size();
    for (size_t i = 0; i < particles_size; i++) {
        resetParticle(i, true);
    }
}

void ParticleSystem::init(const Vec2& window_size) {
    m_emitter.x = m_window_size.x/2;
    m_emitter.y = m_window_size.y/2;
    m_window_size = window_size;
    resetParticles();
}

void ParticleSystem::update() {
    for (std::size_t i = 0; i < m_particles.size(); ++i) {
        if (m_particles[i].lifespan == 0) {
            resetParticle(i);
        }

        m_vertices.incPosAt(6*i + 0, m_particles[i].velocity);
        m_vertices.incPosAt(6*i + 1, m_particles[i].velocity);
        m_vertices.incPosAt(6*i + 2, m_particles[i].velocity);
        m_vertices.incPosAt(6*i + 3, m_particles[i].velocity);
        m_vertices.incPosAt(6*i + 4, m_particles[i].velocity);
        m_vertices.incPosAt(6*i + 5, m_particles[i].velocity);

        m_particles[i].lifespan--;
    }
}

void ParticleSystem::draw(RenderWindow& window) const {
    window.draw(m_vertices);
}
