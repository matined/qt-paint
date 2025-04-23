#include "pacman.h"
#include <QPainter>
#include <cmath>
#include <QDebug>

Pacman::Pacman(const QPoint& center)
    : m_center(center)
{
    qDebug() << "Pacman created with center:" << center;
}

void Pacman::setRadiusPoint(const QPoint& point)
{
    // Calculate radius based on the distance from center to point
    int dx = point.x() - m_center.x();
    int dy = point.y() - m_center.y();
    m_radius = static_cast<int>(std::sqrt(dx * dx + dy * dy));
    
    // Calculate the angle of this point
    m_startAngle = std::atan2(dy, dx);
    if (m_startAngle < 0) {
        m_startAngle += 2 * M_PI;
    }
    
    qDebug() << "Pacman radius set to:" << m_radius << "starting angle:" << m_startAngle;
}

void Pacman::setMouthPoint(const QPoint& point)
{
    // Calculate the angle of the mouth point
    int dx = point.x() - m_center.x();
    int dy = point.y() - m_center.y();
    m_endAngle = std::atan2(dy, dx);
    if (m_endAngle < 0) {
        m_endAngle += 2 * M_PI;
    }
    
    // We want counterclockwise from start to end
    // In standard angle system, counterclockwise means increasing angle
    
    m_isComplete = true;
    qDebug() << "Pacman completed with angles: start=" << m_startAngle << ", end=" << m_endAngle;
}

void Pacman::draw(QPainter& painter)
{
    // Only draw the pacman when it's complete (all 3 clicks done)
    if (m_radius <= 0 || !m_isComplete) return;
    
    painter.setPen(QPen(m_color, 1));
    painter.setBrush(m_color);
    
    if (m_antiAliasing) {
        drawWuCircle(painter);
    } else {
        drawMidpointCircle(painter);
    }
}

void Pacman::drawMidpointCircle(QPainter& painter)
{
    int x = 0;
    int y = m_radius;
    int d = 1 - m_radius;
    int dE = 3;
    int dSE = 5 - 2 * m_radius;

    // Draw all points that aren't in the "mouth"
    while (y >= x) {
        // Draw the eight octants
        for (int i = 0; i < 8; i++) {
            int px, py;
            
            // Calculate point coordinates for each octant
            switch(i) {
                case 0: px = m_center.x() + x; py = m_center.y() + y; break;
                case 1: px = m_center.x() + y; py = m_center.y() + x; break;
                case 2: px = m_center.x() - y; py = m_center.y() + x; break;
                case 3: px = m_center.x() - x; py = m_center.y() + y; break;
                case 4: px = m_center.x() - x; py = m_center.y() - y; break;
                case 5: px = m_center.x() - y; py = m_center.y() - x; break;
                case 6: px = m_center.x() + y; py = m_center.y() - x; break;
                case 7: px = m_center.x() + x; py = m_center.y() - y; break;
                default: continue;
            }
            
            // Skip points that are in the mouth
            if (!isInMouth(px - m_center.x(), py - m_center.y())) {
                painter.drawPoint(px, py);
            }
        }
        
        // Midpoint circle algorithm steps
        if (d < 0) { // Move to E
            d += dE;
            dE += 2;
            dSE += 2;
        } else { // Move to SE
            d += dSE;
            dE += 2;
            dSE += 4;
            --y;
        }
        ++x;
    }
    
    // Draw lines from center to mouth edges
    int x1 = m_center.x() + static_cast<int>(m_radius * std::cos(m_startAngle));
    int y1 = m_center.y() + static_cast<int>(m_radius * std::sin(m_startAngle));
    int x2 = m_center.x() + static_cast<int>(m_radius * std::cos(m_endAngle));
    int y2 = m_center.y() + static_cast<int>(m_radius * std::sin(m_endAngle));
    
    painter.drawLine(m_center, QPoint(x1, y1));
    painter.drawLine(m_center, QPoint(x2, y2));
}

void Pacman::drawWuCircle(QPainter& painter)
{
    int x = m_radius;
    int y = 0;
    
    while (x > y) {
        y++;
        x = static_cast<int>(std::ceil(std::sqrt(m_radius * m_radius - y * y)));
        
        // Calculate intensity for anti-aliasing
        float T = std::sqrt(m_radius * m_radius - y * y) - (x - 1);
        
        // Draw points with anti-aliasing
        plotPoints(painter, x, y, 1.0f - T);
        plotPoints(painter, x - 1, y, T);
    }
    
    // Draw lines from center to mouth edges with anti-aliasing
    if (m_isComplete) {
        int x1 = m_center.x() + static_cast<int>(m_radius * std::cos(m_startAngle));
        int y1 = m_center.y() + static_cast<int>(m_radius * std::sin(m_startAngle));
        int x2 = m_center.x() + static_cast<int>(m_radius * std::cos(m_endAngle));
        int y2 = m_center.y() + static_cast<int>(m_radius * std::sin(m_endAngle));
        
        painter.drawLine(m_center, QPoint(x1, y1));
        painter.drawLine(m_center, QPoint(x2, y2));
    }
}

void Pacman::plotPoints(QPainter& painter, int x, int y, float intensity)
{
    // Create color with alpha based on intensity
    QColor color = m_color;
    color.setAlphaF(intensity);
    painter.setPen(QPen(color, 1));
    
    // Plot points for all eight octants, skipping those in the mouth
    int dx, dy;
    
    // Octant 1
    dx = x; dy = y;
    if (!isInMouth(dx, dy)) painter.drawPoint(m_center.x() + dx, m_center.y() + dy);
    
    // Octant 2
    dx = y; dy = x;
    if (!isInMouth(dx, dy)) painter.drawPoint(m_center.x() + dx, m_center.y() + dy);
    
    // Octant 3
    dx = -y; dy = x;
    if (!isInMouth(dx, dy)) painter.drawPoint(m_center.x() + dx, m_center.y() + dy);
    
    // Octant 4
    dx = -x; dy = y;
    if (!isInMouth(dx, dy)) painter.drawPoint(m_center.x() + dx, m_center.y() + dy);
    
    // Octant 5
    dx = -x; dy = -y;
    if (!isInMouth(dx, dy)) painter.drawPoint(m_center.x() + dx, m_center.y() + dy);
    
    // Octant 6
    dx = -y; dy = -x;
    if (!isInMouth(dx, dy)) painter.drawPoint(m_center.x() + dx, m_center.y() + dy);
    
    // Octant 7
    dx = y; dy = -x;
    if (!isInMouth(dx, dy)) painter.drawPoint(m_center.x() + dx, m_center.y() + dy);
    
    // Octant 8
    dx = x; dy = -y;
    if (!isInMouth(dx, dy)) painter.drawPoint(m_center.x() + dx, m_center.y() + dy);
}

bool Pacman::isInMouth(int x, int y) const
{
    if (!m_isComplete) return false;
    
    // Calculate the angle of this point
    float angle = std::atan2(y, x);
    if (angle < 0) {
        angle += 2 * M_PI;
    }
    
    // For counterclockwise direction from startAngle to endAngle
    
    if (m_startAngle < m_endAngle) {
        // Counterclockwise is the "short way around" (direct path)
        return (angle > m_startAngle && angle < m_endAngle);
    } else {
        // Counterclockwise means the "long way around"
        return !(angle < m_startAngle && angle > m_endAngle);
    }
} 