#include "polygon.h"
#include <QPainter>
#include <cmath>
#include <algorithm>
#include <QDebug>
#include <limits>
#include <QPainterPath>
#include <QImage>
#include <QString>

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
    // First fill interior if needed
    if (m_isImageFilled && !m_fillImage.isNull()) {
        fillWithImage(painter);
    } else if (m_isFilled) {
        fillScanline(painter);
    }
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

    // Calculate differences
    int dx = x2 - x1;
    int dy = y2 - y1;

    // Handle vertical lines
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

    // Handle horizontal lines
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

    // Determine if the line is steep
    bool steep = std::abs(dy) > std::abs(dx);
    
    // If steep, swap x and y coordinates
    if (steep) {
        std::swap(x1, y1);
        std::swap(x2, y2);
        std::swap(dx, dy);
    }

    // Ensure we're always drawing from left to right
    if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
        dx = -dx;
        dy = -dy;
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

        if (steep) {
            // For steep lines, swap back x and y coordinates when drawing
            painter.setPen(QPen(color1, 1));
            painter.drawPoint(yFloor, x);
            painter.setPen(QPen(color2, 1));
            painter.drawPoint(yCeil, x);
        } else {
            painter.setPen(QPen(color1, 1));
            painter.drawPoint(x, yFloor);
            painter.setPen(QPen(color2, 1));
            painter.drawPoint(x, yCeil);
        }

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

std::pair<QPoint, QPoint> Polygon::getEdgePoints(int edgeIndex) const
{
    if (edgeIndex < 0 || edgeIndex >= static_cast<int>(m_vertices.size())) {
        return std::make_pair(QPoint(), QPoint());
    }

    const QPoint& start = m_vertices[edgeIndex];
    const QPoint& end = m_vertices[(edgeIndex + 1) % m_vertices.size()];
    return std::make_pair(start, end);
}

void Polygon::moveEdge(int edgeIndex, const QPoint& offset)
{
    if (edgeIndex < 0 || edgeIndex >= static_cast<int>(m_vertices.size())) {
        return;
    }

    // Move both vertices of the edge
    m_vertices[edgeIndex] += offset;
    int nextIndex = (edgeIndex + 1) % m_vertices.size();
    
    // Only move the second vertex if we're not at the last edge of an unclosed polygon
    if (m_isClosed || edgeIndex < static_cast<int>(m_vertices.size()) - 1) {
        m_vertices[nextIndex] += offset;
    }
}

// ==== Scan-line fill implementation ====
void Polygon::fillScanline(QPainter& painter) const
{
    if (!m_isClosed || m_vertices.size() < 3)
        return;

    struct EdgeEntry {
        int yMax;          // maximum y of the edge
        double x;          // x intersection with current scanline
        double invSlope;   // 1/m = dx/dy
    };

    // Build Edge Table (ET) as a vector indexed by y
    int minY = std::numeric_limits<int>::max();
    int maxY = std::numeric_limits<int>::min();
    for (const auto& v : m_vertices) {
        minY = std::min(minY, v.y());
        maxY = std::max(maxY, v.y());
    }
    if (minY == maxY) return; // degenerate

    int tableHeight = maxY - minY + 1;
    std::vector<std::vector<EdgeEntry>> edgeTable(tableHeight);

    auto addEdge = [&](const QPoint& p1, const QPoint& p2) {
        // Ignore horizontal edges
        if (p1.y() == p2.y())
            return;
        int yMin = p1.y() < p2.y() ? p1.y() : p2.y();
        int yMaxEdge = p1.y() > p2.y() ? p1.y() : p2.y();
        double xOfYMin = (p1.y() < p2.y()) ? p1.x() : p2.x();
        double invSlope = static_cast<double>(p2.x() - p1.x()) / static_cast<double>(p2.y() - p1.y());
        edgeTable[yMin - minY].push_back({yMaxEdge, xOfYMin, invSlope});
    };

    for (size_t i = 0; i < m_vertices.size(); ++i) {
        const QPoint& v1 = m_vertices[i];
        const QPoint& v2 = m_vertices[(i + 1) % m_vertices.size()];
        addEdge(v1, v2);
    }

    // Active Edge Table (AET)
    std::vector<EdgeEntry> AET;

    // Iterate scanlines from minY to maxY
    for (int y = minY; y <= maxY; ++y) {
        // 1. Move edges starting at y into AET
        const auto& bucket = edgeTable[y - minY];
        AET.insert(AET.end(), bucket.begin(), bucket.end());

        // 2. Remove from AET edges where y == yMax
        AET.erase(std::remove_if(AET.begin(), AET.end(), [y](const EdgeEntry& e) {
            return e.yMax == y;
        }), AET.end());

        // 3. Sort AET by current x
        std::sort(AET.begin(), AET.end(), [](const EdgeEntry& a, const EdgeEntry& b) {
            return a.x < b.x;
        });

        // 4. Fill pixels between pairs of intersections
        painter.setPen(QPen(m_fillColor, 1));
        for (size_t i = 0; i + 1 < AET.size(); i += 2) {
            int xStart = static_cast<int>(std::ceil(AET[i].x));
            int xEnd   = static_cast<int>(std::floor(AET[i + 1].x));
            if (xEnd >= xStart) {
                painter.drawLine(xStart, y, xEnd, y);
            }
        }

        // 5. Increment y; handled by loop; 6. For each edge in AET, update x += invSlope
        for (auto& edge : AET) {
            edge.x += edge.invSlope;
        }
    }
}

// ==== Image fill implementation ====
void Polygon::fillWithImage(QPainter& painter) const
{
    if (!m_isClosed || m_vertices.size() < 3 || m_fillImage.isNull())
        return;

    // Build path
    QPainterPath path;
    path.moveTo(m_vertices[0]);
    for (size_t i = 1; i < m_vertices.size(); ++i) {
        path.lineTo(m_vertices[i]);
    }
    path.closeSubpath();

    painter.save();
    painter.setClipPath(path);

    // Choose bounding rect to draw image (scaled to fit)
    QRectF bbox = path.boundingRect();
    painter.drawImage(bbox, m_fillImage);

    painter.restore();
}

bool Polygon::isConvex() const
{
    if (m_vertices.size() < 3 || !m_isClosed) {
        return false;
    }
    bool hasPos = false;
    bool hasNeg = false;
    int n = static_cast<int>(m_vertices.size());
    for (int i = 0; i < n; ++i) {
        const QPoint& a = m_vertices[i];
        const QPoint& b = m_vertices[(i + 1) % n];
        const QPoint& c = m_vertices[(i + 2) % n];
        int cross = (b.x() - a.x()) * (c.y() - b.y()) - (b.y() - a.y()) * (c.x() - b.x());
        if (cross > 0) hasPos = true;
        else if (cross < 0) hasNeg = true;
        if (hasPos && hasNeg) {
            return false; // both directions -> concave
        }
    }
    return true;
} 