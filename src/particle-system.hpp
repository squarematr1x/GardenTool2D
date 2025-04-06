#pragma once

#include <vector>

#include "math/vec2.hpp"
#include "core/vertex-array.hpp"

class RenderWindow;

class ParticleSystem {
    struct Particle {
        Vec2 velocity;
        int lifespan{ 0 };
    };

    std::vector<Particle> m_particles;
    VertexArray m_vertices;
    Vec2 m_window_size;
    Vec2 m_emitter; // Emitter could be the mouse position
    float m_size{ 8.0f };

    void resetParticle(size_t index, bool first = false);
    void resetParticles(size_t count = 1024, float size = 8);

public:
    void init(const Vec2& window_size);
    void update();
    void updateEmitter(float x, float y) { m_emitter.x = x; m_emitter.y = y; }
    void draw(RenderWindow& window) const;
};
