#ifndef POLYGON_H
#define POLYGON_H

#include <QPainter>
#include <QColor>
#include <QPoint>
#include <vector>
#include "brush.h"

class Polygon {
public:
    Polygon();
    
    void draw(QPainter& painter);
    bool contains(const QPoint& point) const;
    void move(const QPoint& offset);
    void addVertex(const QPoint& vertex);
    void close();
    bool isClosed() const { return m_isClosed; }
    void setVertex(int index, const QPoint& point);
    QPoint getVertex(int index) const;
    int getVertexCount() const { return m_vertices.size(); }
    bool isNearVertex(const QPoint& point, int& vertexIndex) const;
    bool isNearEdge(const QPoint& point, int& edgeIndex) const;
    
    void setColor(const QColor& color) { m_color = color; }
    QColor getColor() const { return m_color; }
    
    void setThickness(int thickness);
    int getThickness() const { return m_thickness; }

    void setAntiAliasing(bool enabled) { m_brush.setAntiAliasing(enabled); }
    bool isAntiAliasing() const { return m_brush.isAntiAliasing(); }

    // New methods for edge manipulation
    std::pair<QPoint, QPoint> getEdgePoints(int edgeIndex) const;
    void moveEdge(int edgeIndex, const QPoint& offset);

private:
    void drawEdges(QPainter& painter);
    void drawVertices(QPainter& painter);
    void drawWithBrush(QPainter& painter, int x, int y);
    void drawWuLine(QPainter& painter, const QPoint& start, const QPoint& end);
    
    std::vector<QPoint> m_vertices;
    bool m_isClosed = false;
    QColor m_color = Qt::black;
    int m_thickness = 1;
    Brush m_brush;
    static const int VERTEX_SIZE = 8; // Size of the vertex squares
};

#endif // POLYGON_H 