#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <QPainter>
#include <QColor>
#include <QPoint>
#include <vector>
#include "brush.h"

class Rectangle {
public:
    // Construct rectangle from two opposite corners (any order)
    Rectangle(const QPoint& firstCorner, const QPoint& oppositeCorner);

    // Rendering
    void draw(QPainter& painter);

    // Geometry helpers
    bool contains(const QPoint& point) const;
    bool isNearVertex(const QPoint& point, int& vertexIndex) const;   // returns true if near vertex, sets index 0-3
    bool isNearEdge(const QPoint& point, int& edgeIndex) const;       // returns true if near edge, sets index 0-3

    // Manipulation
    void move(const QPoint& offset);                                  // move whole rectangle
    void moveVertex(int vertexIndex, const QPoint& newPos);           // move selected vertex
    void moveEdge(int edgeIndex, const QPoint& offset);               // translate an edge along its normal (axis-aligned)

    // Corner setters (used while drawing)
    void setFirstCorner(const QPoint& p);
    void setOppositeCorner(const QPoint& p);

    // Appearance
    void setColor(const QColor& color) { m_color = color; }
    QColor getColor() const { return m_color; }

    void setThickness(int thickness);
    int getThickness() const { return m_thickness; }

    void setAntiAliasing(bool enabled) { m_brush.setAntiAliasing(enabled); }
    bool isAntiAliasing() const { return m_brush.isAntiAliasing(); }

    // Debug/helper
    QPoint getVertex(int index) const;                                // returns vertex coordinates (0-3)

private:
    void updateVertices();                                            // recompute 4 vertices from the two stored corners

    // Drawing helpers
    void drawEdges(QPainter& painter);
    void drawVertices(QPainter& painter);
    void drawWithBrush(QPainter& painter, int x, int y);
    void drawWuLine(QPainter& painter, const QPoint& start, const QPoint& end);

    // Data
    QPoint m_firstCorner;         // one corner selected first (does not have to be top-left)
    QPoint m_oppositeCorner;      // opposite corner (second corner)
    std::vector<QPoint> m_vertices; // 4 vertices in clockwise order starting at top-left

    QColor m_color = Qt::black;
    int m_thickness = 1;
    Brush m_brush;

    static const int VERTEX_SIZE = 8; // square size for vertex handles
};

#endif // RECTANGLE_H 