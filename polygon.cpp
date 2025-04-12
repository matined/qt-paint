#include "polygon.h"
#include <QPainter>
#include <cmath>
#include <algorithm>
#include <QDebug>

Polygon::Polygon()
    : m_brush(1)
{
}

void Polygon::setThickness(int thickness)
{
    m_thickness = std::max(1, thickness);
    m_brush = Brush(m_thickness);
}

void Polygon::draw(QPainter& painter)
{
    painter.setPen(QPen(m_color, 1)); // Use 1-pixel pen for brush drawing
    drawEdges(painter);
    drawVertices(painter);
}

void Polygon::drawEdges(QPainter& painter)
{
    if (m_vertices.size() < 2) return;

    for (size_t i = 0; i < m_vertices.size(); ++i) {
        const QPoint& start = m_vertices[i];
        const QPoint& end = m_vertices[(i + 1) % m_vertices.size()];
        
        if (!m_isClosed && i == m_vertices.size() - 1) break;
        
        if (m_brush.isAntiAliasing()) {
            drawWuLine(painter, start, end);
        } else {
            // Use DDA algorithm for line drawing
            int x1 = start.x();
            int y1 = start.y();
            int x2 = end.x();
            int y2 = end.y();
            
            int dx = x2 - x1;
            int dy = y2 - y1;
            
            int steps = std::max(abs(dx), abs(dy));
            
            float xIncrement = dx / (float)steps;
            float yIncrement = dy / (float)steps;
            
            float x = x1;
            float y = y1;
            
            for (int j = 0; j <= steps; j++) {
                drawWithBrush(painter, round(x), round(y));
                x += xIncrement;
                y += yIncrement;
            }
        }
    }
}

void Polygon::drawWuLine(QPainter& painter, const QPoint& start, const QPoint& end)
{
    int x1 = start.x();
    int y1 = start.y();
    int x2 = end.x();
    int y2 = end.y();

    // Ensure we're always drawing from left to right
    if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    int dx = x2 - x1;
    int dy = y2 - y1;

    if (dx == 0) {
        // Vertical line
        int y = std::min(y1, y2);
        int yEnd = std::max(y1, y2);
        while (y <= yEnd) {
            painter.setPen(QPen(m_color, 1));
            painter.drawPoint(x1, y);
            y++;
        }
        return;
    }

    float gradient = static_cast<float>(dy) / dx;
    float y = y1;

    // Main loop
    for (int x = x1; x <= x2; x++) {
        // Calculate the two y values
        int yFloor = static_cast<int>(y);
        int yCeil = yFloor + 1;
        float intensity = y - yFloor;

        // Draw the two pixels
        QColor color1 = m_color;
        QColor color2 = m_color;
        color1.setAlphaF(1.0f - intensity);
        color2.setAlphaF(intensity);

        painter.setPen(QPen(color1, 1));
        painter.drawPoint(x, yFloor);
        painter.setPen(QPen(color2, 1));
        painter.drawPoint(x, yCeil);

        y += gradient;
    }
}

void Polygon::drawWithBrush(QPainter& painter, int x, int y)
{
    const auto& pattern = m_brush.getPattern();
    int size = m_brush.getSize();
    int halfSize = size / 2;
    
    for (int dy = 0; dy < size; ++dy) {
        for (int dx = 0; dx < size; ++dx) {
            if (pattern[dy][dx]) {
                painter.drawPoint(x + dx - halfSize, y + dy - halfSize);
            }
        }
    }
}

void Polygon::drawVertices(QPainter& painter)
{
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    
    for (const auto& vertex : m_vertices) {
        painter.drawRect(vertex.x() - VERTEX_SIZE/2,
                        vertex.y() - VERTEX_SIZE/2,
                        VERTEX_SIZE, VERTEX_SIZE);
    }
}

void Polygon::addVertex(const QPoint& vertex)
{
    m_vertices.push_back(vertex);
}

void Polygon::close()
{
    if (m_vertices.size() >= 3) {
        m_isClosed = true;
    }
}

void Polygon::setVertex(int index, const QPoint& point)
{
    if (index >= 0 && index < static_cast<int>(m_vertices.size())) {
        m_vertices[index] = point;
    }
}

QPoint Polygon::getVertex(int index) const
{
    if (index >= 0 && index < static_cast<int>(m_vertices.size())) {
        return m_vertices[index];
    }
    return QPoint();
}

bool Polygon::isNearVertex(const QPoint& point, int& vertexIndex) const
{
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        int dx = point.x() - m_vertices[i].x();
        int dy = point.y() - m_vertices[i].y();
        int distanceSquared = dx * dx + dy * dy;
        
        if (distanceSquared <= (VERTEX_SIZE * VERTEX_SIZE)) {
            vertexIndex = static_cast<int>(i);
            return true;
        }
    }
    return false;
}

bool Polygon::isNearEdge(const QPoint& point, int& edgeIndex) const
{
    if (m_vertices.size() < 2) return false;
    
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        const QPoint& start = m_vertices[i];
        const QPoint& end = m_vertices[(i + 1) % m_vertices.size()];
        
        if (!m_isClosed && i == m_vertices.size() - 1) break;
        
        // Calculate distance from point to line segment
        int x = point.x();
        int y = point.y();
        int x1 = start.x();
        int y1 = start.y();
        int x2 = end.x();
        int y2 = end.y();
        
        float lineLength = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
        float distance = abs((y2 - y1) * x - (x2 - x1) * y + x2 * y1 - y2 * x1) / lineLength;
        
        // Check if point is within a certain threshold of the line
        if (distance <= m_thickness) {
            edgeIndex = static_cast<int>(i);
            return true;
        }
    }
    return false;
}

bool Polygon::contains(const QPoint& point) const
{
    if (!m_isClosed || m_vertices.size() < 3) return false;
    
    // Check if point is near any vertex or edge
    int vertexIndex, edgeIndex;
    if (isNearVertex(point, vertexIndex) || isNearEdge(point, edgeIndex)) {
        return true;
    }
    
    // Ray casting algorithm for point-in-polygon test
    bool inside = false;
    for (size_t i = 0, j = m_vertices.size() - 1; i < m_vertices.size(); j = i++) {
        if (((m_vertices[i].y() > point.y()) != (m_vertices[j].y() > point.y())) &&
            (point.x() < (m_vertices[j].x() - m_vertices[i].x()) * (point.y() - m_vertices[i].y()) / 
            (m_vertices[j].y() - m_vertices[i].y()) + m_vertices[i].x())) {
            inside = !inside;
        }
    }
    return inside;
}

void Polygon::move(const QPoint& offset)
{
    for (auto& vertex : m_vertices) {
        vertex += offset;
    }
} 