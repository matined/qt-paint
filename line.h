#ifndef LINE_H
#define LINE_H

#include <QPainter>
#include <QColor>
#include <QPoint>
#include "brush.h"

class Line {
public:
    Line(const QPoint& start, const QPoint& end);
    
    void draw(QPainter& painter);
    bool contains(const QPoint& point) const;
    void move(const QPoint& offset);
    QPoint getStartPoint() const { return m_start; }
    QPoint getEndPoint() const { return m_end; }
    void setStartPoint(const QPoint& point) { m_start = point; }
    void setEndPoint(const QPoint& point) { m_end = point; }
    bool isNearEndpoint(const QPoint& point, bool& isStart) const;
    
    void setColor(const QColor& color) { m_color = color; }
    QColor getColor() const { return m_color; }
    
    void setThickness(int thickness);
    int getThickness() const { return m_thickness; }

    void setAntiAliasing(bool enabled) { m_brush.setAntiAliasing(enabled); }
    bool isAntiAliasing() const { return m_brush.isAntiAliasing(); }
    
private:
    void drawDDA(QPainter& painter);
    void drawEndpoints(QPainter& painter);
    void drawWithBrush(QPainter& painter, int x, int y);
    void drawWuLine(QPainter& painter);
    
    QPoint m_start;
    QPoint m_end;
    QColor m_color = Qt::black;
    int m_thickness = 1;
    Brush m_brush;
    static const int ENDPOINT_SIZE = 8; // Size of the endpoint squares
};

#endif // LINE_H 