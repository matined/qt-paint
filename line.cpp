#include "line.h"
#include <QPainter>
#include <cmath>
#include <algorithm>
#include <QDebug>

Line::Line(const QPoint& start, const QPoint& end)
    : m_start(start), m_end(end), m_brush(1)
{
    qDebug() << "Line created from" << start << "to" << end;
}

void Line::setThickness(int thickness)
{
    m_thickness = std::max(1, thickness);
    m_brush = Brush(m_thickness);
}

void Line::draw(QPainter& painter)
{
    if (m_brush.isAntiAliasing()) {
        drawWuLine(painter);
    } else {
        painter.setPen(QPen(m_color, 1)); // Use 1-pixel pen for brush drawing
        drawDDA(painter);
    }
    drawEndpoints(painter);
}

void Line::drawDDA(QPainter& painter)
{
    int x1 = m_start.x();
    int y1 = m_start.y();
    int x2 = m_end.x();
    int y2 = m_end.y();
    
    qDebug() << "Drawing DDA line from" << m_start << "to" << m_end;
    
    // Calculate dx and dy
    int dx = x2 - x1;
    int dy = y2 - y1;
    
    // Calculate steps required for generating pixels
    int steps = std::max(abs(dx), abs(dy));
    
    // Calculate increment in x and y for each step
    float xIncrement = dx / (float)steps;
    float yIncrement = dy / (float)steps;
    
    // Put pixel for each step
    float x = x1;
    float y = y1;
    
    for (int i = 0; i <= steps; i++) {
        drawWithBrush(painter, round(x), round(y));
        x += xIncrement;
        y += yIncrement;
    }
}

void Line::drawWithBrush(QPainter& painter, int x, int y)
{
    const auto& pattern = m_brush.getPattern();
    int size = m_brush.getSize();
    int halfSize = size / 2;
    
    // Draw the brush pattern around the center point
    for (int dy = 0; dy < size; ++dy) {
        for (int dx = 0; dx < size; ++dx) {
            if (pattern[dy][dx]) {
                painter.drawPoint(x + dx - halfSize, y + dy - halfSize);
            }
        }
    }
}

void Line::drawWuLine(QPainter& painter)
{
    int x1 = m_start.x();
    int y1 = m_start.y();
    int x2 = m_end.x();
    int y2 = m_end.y();

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

void Line::drawEndpoints(QPainter& painter)
{
    // Draw black squares at endpoints
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    
    // Draw start point square
    painter.drawRect(m_start.x() - ENDPOINT_SIZE/2, 
                    m_start.y() - ENDPOINT_SIZE/2,
                    ENDPOINT_SIZE, ENDPOINT_SIZE);
    
    // Draw end point square
    painter.drawRect(m_end.x() - ENDPOINT_SIZE/2,
                    m_end.y() - ENDPOINT_SIZE/2,
                    ENDPOINT_SIZE, ENDPOINT_SIZE);
}

bool Line::contains(const QPoint& point) const
{
    // Calculate the distance from the point to the line
    int x = point.x();
    int y = point.y();
    int x1 = m_start.x();
    int y1 = m_start.y();
    int x2 = m_end.x();
    int y2 = m_end.y();
    
    // Calculate the line length
    float lineLength = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    
    // Calculate the distance from the point to the line
    float distance = abs((y2 - y1) * x - (x2 - x1) * y + x2 * y1 - y2 * x1) / lineLength;
    
    // Check if the point is within a certain threshold of the line
    return distance <= m_thickness; // Use thickness as threshold
}

bool Line::isNearEndpoint(const QPoint& point, bool& isStart) const
{
    // Check if point is near start point
    int dxStart = point.x() - m_start.x();
    int dyStart = point.y() - m_start.y();
    int distanceStart = dxStart * dxStart + dyStart * dyStart;
    
    // Check if point is near end point
    int dxEnd = point.x() - m_end.x();
    int dyEnd = point.y() - m_end.y();
    int distanceEnd = dxEnd * dxEnd + dyEnd * dyEnd;
    
    // Check if point is within the endpoint size
    int threshold = ENDPOINT_SIZE * ENDPOINT_SIZE;
    
    if (distanceStart <= threshold) {
        isStart = true;
        return true;
    } else if (distanceEnd <= threshold) {
        isStart = false;
        return true;
    }
    
    return false;
}

void Line::move(const QPoint& offset)
{
    m_start += offset;
    m_end += offset;
}