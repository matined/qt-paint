#ifndef PACMAN_H
#define PACMAN_H

#include <QPainter>
#include <QColor>
#include <QPoint>
#include <cmath>

class Pacman {
public:
    Pacman(const QPoint& center);
    
    void draw(QPainter& painter);
    void setCenter(const QPoint& center) { m_center = center; }
    void setRadiusPoint(const QPoint& point);
    void setMouthPoint(const QPoint& point);
    
    QPoint getCenter() const { return m_center; }
    int getRadius() const { return m_radius; }
    float getStartAngle() const { return m_startAngle; }
    float getEndAngle() const { return m_endAngle; }
    
    bool isComplete() const { return m_isComplete; }
    
    void setAntiAliasing(bool enabled) { m_antiAliasing = enabled; }
    bool isAntiAliasing() const { return m_antiAliasing; }
    
private:
    void drawMidpointCircle(QPainter& painter);
    void drawWuCircle(QPainter& painter);
    void plotPoints(QPainter& painter, int x, int y, float intensity);
    bool isInMouth(int x, int y) const;
    
    QPoint m_center;
    int m_radius = 0;
    float m_startAngle = 0.0f;  // Start angle of the mouth in radians
    float m_endAngle = 0.0f;    // End angle of the mouth in radians
    bool m_isComplete = false;
    bool m_antiAliasing = false;
    QColor m_color = Qt::black;
};

#endif // PACMAN_H 