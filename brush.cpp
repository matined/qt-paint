#include "brush.h"
#include <cmath>

Brush::Brush(int size)
    : m_size(size)
{
    generateCircularPattern();
}

void Brush::generateCircularPattern()
{
    // Initialize pattern with false values
    m_pattern.resize(m_size, std::vector<bool>(m_size, false));
    
    // For size 1, just set the single point
    if (m_size == 1) {
        m_pattern[0][0] = true;
        return;
    }
    
    // For size 2, create a 2x2 square
    if (m_size == 2) {
        m_pattern[0][0] = true;
        m_pattern[0][1] = true;
        m_pattern[1][0] = true;
        m_pattern[1][1] = true;
        return;
    }
    
    // For larger sizes, generate a proper circle
    float center = (m_size - 1) / 2.0f;
    float radius = center;
    
    // Generate circular pattern
    for (int y = 0; y < m_size; ++y) {
        for (int x = 0; x < m_size; ++x) {
            // Calculate distance from center
            float dx = x - center;
            float dy = y - center;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            // If distance is less than or equal to radius, mark as part of pattern
            if (distance <= radius) {
                m_pattern[y][x] = true;
            }
        }
    }
}

bool Brush::isInPattern(int x, int y) const
{
    // Check bounds
    if (x < 0 || x >= m_size || y < 0 || y >= m_size) {
        return false;
    }
    
    return m_pattern[y][x];
}

float Brush::getIntensity(int x, int y) const
{
    if (!m_antiAliasing) {
        return isInPattern(x, y) ? 1.0f : 0.0f;
    }

    // Check bounds
    if (x < 0 || x >= m_size || y < 0 || y >= m_size) {
        return 0.0f;
    }

    float center = (m_size - 1) / 2.0f;
    float radius = center;
    
    // Calculate distance from center
    float dx = x - center;
    float dy = y - center;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    // Calculate intensity based on distance from the edge
    float intensity = 1.0f - std::min(1.0f, std::max(0.0f, (distance - (radius - 1.0f))));
    
    return intensity;
}