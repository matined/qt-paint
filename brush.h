#ifndef BRUSH_H
#define BRUSH_H

#include <vector>
#include <cmath>
#include <QColor>

class Brush {
public:
    Brush(int size);
    
    // Get the brush pattern
    const std::vector<std::vector<bool>>& getPattern() const { return m_pattern; }
    
    // Get the brush size
    int getSize() const { return m_size; }
    
    // Check if a point is within the brush pattern
    bool isInPattern(int x, int y) const;

    // Anti-aliasing support
    void setAntiAliasing(bool enabled) { m_antiAliasing = enabled; }
    bool isAntiAliasing() const { return m_antiAliasing; }
    float getIntensity(int x, int y) const;

private:
    void generateCircularPattern();
    
    int m_size;
    std::vector<std::vector<bool>> m_pattern;
    bool m_antiAliasing = false;
};

#endif // BRUSH_H