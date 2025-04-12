#ifndef CIRCLE_H
#define CIRCLE_H

#include <QPainter>
#include <QColor>
#include <QPoint>

class Circle {
public:
    Circle(const QPoint& center, int radius);
    
    void draw(QPainter& painter);
    bool contains(const QPoint& point) const;
    void move(const QPoint& offset);
    void setCenter(const QPoint& center) { m_center = center; }
    void setRadius(int radius) { m_radius = radius; }
    QPoint getCenter() const { return m_center; }
    int getRadius() const { return m_radius; }
    bool isNearCenter(const QPoint& point) const;
    bool isNearRadius(const QPoint& point) const;
    
    void setColor(const QColor& color) { m_color = color; }
    QColor getColor() const { return m_color; }
    
private:
    void drawMidpointCircle(QPainter& painter);
    void drawCenter(QPainter& painter);
    void drawRadiusPoint(QPainter& painter);
    
    QPoint m_center;
    int m_radius;
    QColor m_color = Qt::black;
    static const int CENTER_SIZE = 8; // Size of the center point square
    static const int RADIUS_POINT_SIZE = 6; // Size of the radius point square
};

#endif // CIRCLE_H 