#ifndef BRUSH_H
#define BRUSH_H

#include <vector>
#include <cmath>

class Brush {
public:
    Brush(int size);
    
    // Get the brush pattern
    const std::vector<std::vector<bool>>& getPattern() const { return m_pattern; }
    
    // Get the brush size
    int getSize() const { return m_size; }
    
    // Check if a point is within the brush pattern
    bool isInPattern(int x, int y) const;

private:
    void generateCircularPattern();
    
    int m_size;
    std::vector<std::vector<bool>> m_pattern;
};

#endif // BRUSH_H