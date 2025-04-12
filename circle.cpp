#include "circle.h"
#include <QPainter>
#include <cmath>
#include <QDebug>

Circle::Circle(const QPoint& center, int radius)
    : m_center(center), m_radius(radius)
{
    qDebug() << "Circle created with center:" << center << "and radius:" << radius;
}

void Circle::draw(QPainter& painter)
{
    if (m_antiAliasing) {
        drawWuCircle(painter);
    } else {
        painter.setPen(QPen(m_color, 1));
        drawMidpointCircle(painter);
    }
    drawCenter(painter);
    drawRadiusPoint(painter);
}

void Circle::drawMidpointCircle(QPainter& painter)
{
    int x = 0;
    int y = m_radius;
    int d = 1 - m_radius;
    int dE = 3;
    int dSE = 5 - 2 * m_radius;

    // Draw the initial points
    painter.drawPoint(m_center.x() + x, m_center.y() + y);
    painter.drawPoint(m_center.x() - x, m_center.y() + y);
    painter.drawPoint(m_center.x() + x, m_center.y() - y);
    painter.drawPoint(m_center.x() - x, m_center.y() - y);
    painter.drawPoint(m_center.x() + y, m_center.y() + x);
    painter.drawPoint(m_center.x() - y, m_center.y() + x);
    painter.drawPoint(m_center.x() + y, m_center.y() - x);
    painter.drawPoint(m_center.x() - y, m_center.y() - x);

    while (y > x) {
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

        // Draw all eight octants
        painter.drawPoint(m_center.x() + x, m_center.y() + y);
        painter.drawPoint(m_center.x() - x, m_center.y() + y);
        painter.drawPoint(m_center.x() + x, m_center.y() - y);
        painter.drawPoint(m_center.x() - x, m_center.y() - y);
        painter.drawPoint(m_center.x() + y, m_center.y() + x);
        painter.drawPoint(m_center.x() - y, m_center.y() + x);
        painter.drawPoint(m_center.x() + y, m_center.y() - x);
        painter.drawPoint(m_center.x() - y, m_center.y() - x);
    }
}

void Circle::drawWuCircle(QPainter& painter)
{
    int x = m_radius;
    int y = 0;
    
    // Draw the initial points
    plotPoints(painter, x, y, 1.0f);
    
    while (x > y) {
        y++;
        x = static_cast<int>(std::ceil(std::sqrt(m_radius * m_radius - y * y)));
        
        // Calculate intensity
        float T = std::sqrt(m_radius * m_radius - y * y) - (x - 1);
        
        // Draw the points with anti-aliasing
        plotPoints(painter, x, y, 1.0f - T);
        plotPoints(painter, x - 1, y, T);
    }
}

void Circle::plotPoints(QPainter& painter, int x, int y, float intensity)
{
    // Create color with alpha based on intensity
    QColor color = m_color;
    color.setAlphaF(intensity);
    painter.setPen(QPen(color, 1));
    
    // Plot all eight octants
    painter.drawPoint(m_center.x() + x, m_center.y() + y);
    painter.drawPoint(m_center.x() - x, m_center.y() + y);
    painter.drawPoint(m_center.x() + x, m_center.y() - y);
    painter.drawPoint(m_center.x() - x, m_center.y() - y);
    painter.drawPoint(m_center.x() + y, m_center.y() + x);
    painter.drawPoint(m_center.x() - y, m_center.y() + x);
    painter.drawPoint(m_center.x() + y, m_center.y() - x);
    painter.drawPoint(m_center.x() - y, m_center.y() - x);
}

void Circle::drawCenter(QPainter& painter)
{
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    painter.drawRect(m_center.x() - CENTER_SIZE/2,
                    m_center.y() - CENTER_SIZE/2,
                    CENTER_SIZE, CENTER_SIZE);
}

void Circle::drawRadiusPoint(QPainter& painter)
{
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    QPoint radiusPoint = m_center + QPoint(m_radius, 0);
    painter.drawRect(radiusPoint.x() - RADIUS_POINT_SIZE/2,
                    radiusPoint.y() - RADIUS_POINT_SIZE/2,
                    RADIUS_POINT_SIZE, RADIUS_POINT_SIZE);
}

bool Circle::contains(const QPoint& point) const
{
    int dx = point.x() - m_center.x();
    int dy = point.y() - m_center.y();
    int distanceSquared = dx * dx + dy * dy;
    int radiusSquared = m_radius * m_radius;
    
    // Check if point is within the circle with a small margin for selection
    return std::abs(distanceSquared - radiusSquared) <= 100;
}

bool Circle::isNearCenter(const QPoint& point) const
{
    int dx = point.x() - m_center.x();
    int dy = point.y() - m_center.y();
    return (dx * dx + dy * dy) <= (CENTER_SIZE * CENTER_SIZE);
}

bool Circle::isNearRadius(const QPoint& point) const
{
    QPoint radiusPoint = m_center + QPoint(m_radius, 0);
    int dx = point.x() - radiusPoint.x();
    int dy = point.y() - radiusPoint.y();
    return (dx * dx + dy * dy) <= (RADIUS_POINT_SIZE * RADIUS_POINT_SIZE);
}

void Circle::move(const QPoint& offset)
{
    m_center += offset;
} 