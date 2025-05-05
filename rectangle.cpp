#include "rectangle.h"
#include <algorithm>
#include <cmath>

Rectangle::Rectangle(const QPoint& firstCorner, const QPoint& oppositeCorner)
    : m_firstCorner(firstCorner), m_oppositeCorner(oppositeCorner), m_brush(1)
{
    updateVertices();
}

void Rectangle::setFirstCorner(const QPoint& p) {
    m_firstCorner = p;
    updateVertices();
}

void Rectangle::setOppositeCorner(const QPoint& p) {
    m_oppositeCorner = p;
    updateVertices();
}

void Rectangle::updateVertices()
{
    // Determine top-left and bottom-right points regardless of order
    int left   = std::min(m_firstCorner.x(), m_oppositeCorner.x());
    int right  = std::max(m_firstCorner.x(), m_oppositeCorner.x());
    int top    = std::min(m_firstCorner.y(), m_oppositeCorner.y());
    int bottom = std::max(m_firstCorner.y(), m_oppositeCorner.y());

    QPoint topLeft(left, top);
    QPoint topRight(right, top);
    QPoint bottomRight(right, bottom);
    QPoint bottomLeft(left, bottom);

    if (m_vertices.empty()) {
        m_vertices.resize(4);
    }
    m_vertices[0] = topLeft;
    m_vertices[1] = topRight;
    m_vertices[2] = bottomRight;
    m_vertices[3] = bottomLeft;
}

void Rectangle::setThickness(int thickness)
{
    m_thickness = std::max(1, thickness);
    m_brush = Brush(m_thickness);
}

void Rectangle::draw(QPainter& painter)
{
    painter.setPen(QPen(m_color, 1)); // pen width 1, brush handles thickness
    drawEdges(painter);
    drawVertices(painter);
}

void Rectangle::drawEdges(QPainter& painter)
{
    if (m_vertices.size() != 4) return;
    for (int i = 0; i < 4; ++i) {
        const QPoint& start = m_vertices[i];
        const QPoint& end   = m_vertices[(i + 1) % 4];
        if (m_brush.isAntiAliasing()) {
            drawWuLine(painter, start, end);
        } else {
            // DDA similar to polygon
            int x1 = start.x();
            int y1 = start.y();
            int x2 = end.x();
            int y2 = end.y();
            int dx = x2 - x1;
            int dy = y2 - y1;
            int steps = std::max(std::abs(dx), std::abs(dy));
            float xInc = dx / static_cast<float>(steps);
            float yInc = dy / static_cast<float>(steps);
            float x = x1;
            float y = y1;
            for (int s = 0; s <= steps; ++s) {
                drawWithBrush(painter, std::round(x), std::round(y));
                x += xInc;
                y += yInc;
            }
        }
    }
}

void Rectangle::drawWithBrush(QPainter& painter, int x, int y)
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

void Rectangle::drawVertices(QPainter& painter)
{
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    for (const auto& v : m_vertices) {
        painter.drawRect(v.x() - VERTEX_SIZE/2, v.y() - VERTEX_SIZE/2, VERTEX_SIZE, VERTEX_SIZE);
    }
}

// Wu line algorithm identical to polygon implementation, copy code
void Rectangle::drawWuLine(QPainter& painter, const QPoint& start, const QPoint& end)
{
    int x1 = start.x();
    int y1 = start.y();
    int x2 = end.x();
    int y2 = end.y();

    int dx = x2 - x1;
    int dy = y2 - y1;

    if (dx == 0) {
        int y = std::min(y1, y2);
        int yEnd = std::max(y1, y2);
        while (y <= yEnd) {
            painter.setPen(QPen(m_color, 1));
            painter.drawPoint(x1, y);
            y++;
        }
        return;
    }

    if (dy == 0) {
        int x = std::min(x1, x2);
        int xEnd = std::max(x1, x2);
        while (x <= xEnd) {
            painter.setPen(QPen(m_color, 1));
            painter.drawPoint(x, y1);
            x++;
        }
        return;
    }

    bool steep = std::abs(dy) > std::abs(dx);
    if (steep) {
        std::swap(x1, y1);
        std::swap(x2, y2);
        std::swap(dx, dy);
    }

    if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
        dx = -dx;
        dy = -dy;
    }

    float gradient = static_cast<float>(dy) / dx;
    float y = y1;

    for (int x = x1; x <= x2; ++x) {
        int yFloor = static_cast<int>(y);
        int yCeil = yFloor + 1;
        float intensity = y - yFloor;
        QColor c1 = m_color; QColor c2 = m_color;
        c1.setAlphaF(1.0f - intensity);
        c2.setAlphaF(intensity);
        if (steep) {
            painter.setPen(QPen(c1, 1));
            painter.drawPoint(yFloor, x);
            painter.setPen(QPen(c2, 1));
            painter.drawPoint(yCeil, x);
        } else {
            painter.setPen(QPen(c1, 1));
            painter.drawPoint(x, yFloor);
            painter.setPen(QPen(c2, 1));
            painter.drawPoint(x, yCeil);
        }
        y += gradient;
    }
}

QPoint Rectangle::getVertex(int index) const
{
    if (index >=0 && index < static_cast<int>(m_vertices.size()))
        return m_vertices[index];
    return QPoint();
}

bool Rectangle::isNearVertex(const QPoint& point, int& vertexIndex) const
{
    for (int i = 0; i < 4; ++i) {
        int dx = point.x() - m_vertices[i].x();
        int dy = point.y() - m_vertices[i].y();
        if (dx*dx + dy*dy <= VERTEX_SIZE*VERTEX_SIZE) {
            vertexIndex = i;
            return true;
        }
    }
    return false;
}

bool Rectangle::isNearEdge(const QPoint& point, int& edgeIndex) const
{
    // edges 0: top between v0 v1; 1: right v1 v2; 2: bottom v2 v3; 3: left v3 v0
    for (int i = 0; i < 4; ++i) {
        const QPoint& start = m_vertices[i];
        const QPoint& end = m_vertices[(i+1)%4];
        int x = point.x(); int y = point.y();
        int x1 = start.x(); int y1 = start.y();
        int x2 = end.x(); int y2 = end.y();
        float length = std::sqrt(std::pow(x2-x1,2) + std::pow(y2-y1,2));
        if (length == 0) continue;
        float dist = std::abs((y2 - y1)*x - (x2 - x1)*y + x2*y1 - y2*x1)/length;
        if (dist <= m_thickness) {
            // Additional constraint: projection within segment
            if ((x >= std::min(x1,x2) - VERTEX_SIZE && x <= std::max(x1,x2)+VERTEX_SIZE &&
                 y >= std::min(y1,y2) - VERTEX_SIZE && y <= std::max(y1,y2)+VERTEX_SIZE)) {
                edgeIndex = i;
                return true;
            }
        }
    }
    return false;
}

bool Rectangle::contains(const QPoint& point) const
{
    // Axis aligned rectangle containment
    int left = std::min(m_vertices[0].x(), m_vertices[2].x());
    int right = std::max(m_vertices[0].x(), m_vertices[2].x());
    int top = std::min(m_vertices[0].y(), m_vertices[2].y());
    int bottom = std::max(m_vertices[0].y(), m_vertices[2].y());

    if (point.x() < left || point.x() > right || point.y() < top || point.y() > bottom)
        return false;

    // Ensure not near vertex/edge is not required for containment; but we treat whole region as contained
    return true;
}

void Rectangle::move(const QPoint& offset)
{
    m_firstCorner += offset;
    m_oppositeCorner += offset;
    for (auto& v : m_vertices) {
        v += offset;
    }
}

void Rectangle::moveVertex(int vertexIndex, const QPoint& newPos)
{
    if (vertexIndex < 0 || vertexIndex >= 4) return;
    // The opposite corner is (vertexIndex + 2) % 4
    int oppIdx = (vertexIndex + 2) % 4;
    QPoint fixedCorner = m_vertices[oppIdx];
    m_firstCorner = newPos;
    m_oppositeCorner = fixedCorner;
    updateVertices();
}

void Rectangle::moveEdge(int edgeIndex, const QPoint& offset)
{
    if (edgeIndex <0 || edgeIndex >=4) return;
    // move the two vertices on the edge by offset along allowed axis (axis aligned), restrict properly
    int axis = (edgeIndex % 2 == 0) ? 1 : 0; // even edges horizontal (move y only), odd vertical (move x only)
    for (int k=0;k<4;++k) {
        if (k==edgeIndex || k==(edgeIndex+1)%4) {
            if (axis==1) {
                m_vertices[k].ry() += offset.y();
            } else {
                m_vertices[k].rx() += offset.x();
            }
        }
    }
    // After moving, rebuild first/opposite, but keep orthogonality
    int left = std::min({m_vertices[0].x(), m_vertices[2].x()});
    int right = std::max({m_vertices[0].x(), m_vertices[2].x()});
    int top = std::min({m_vertices[0].y(), m_vertices[2].y()});
    int bottom = std::max({m_vertices[0].y(), m_vertices[2].y()});
    m_firstCorner = QPoint(left, top);
    m_oppositeCorner = QPoint(right, bottom);
    updateVertices();
} 