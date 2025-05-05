#include "canvas.h"
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QColorDialog>
#include "rectangle.h"
#include "clipping.h"
#include <algorithm>

Canvas::Canvas(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setMinimumSize(400, 400); // Ensure canvas has a minimum size
    setAutoFillBackground(true); // Enable auto-fill background
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

Canvas::~Canvas() = default;

void Canvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    
    // Draw all lines
    for (const auto& line : m_lines) {
        line->draw(painter);
    }
    
    // Draw all circles
    for (const auto& circle : m_circles) {
        circle->draw(painter);
    }
    
    // Draw all polygons
    for (const auto& polygon : m_polygons) {
        polygon->draw(painter);
    }
    
    // Draw all rectangles
    for (const auto& rect : m_rectangles) {
        rect->draw(painter);
    }
    
    // Draw current line if exists
    if (m_currentLine) {
        m_currentLine->draw(painter);
    }
    
    // Draw current circle if exists
    if (m_currentCircle) {
        m_currentCircle->draw(painter);
    }
    
    // Draw current polygon if exists
    if (m_currentPolygon) {
        m_currentPolygon->draw(painter);
    }
    
    // Draw current rectangle if exists
    if (m_currentRectangle) {
        m_currentRectangle->draw(painter);
    }
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    // First handle clipping mode separately
    if (m_isClippingMode) {
        if (event->button() == Qt::LeftButton) {
            // Left-click to add a polygon to clip chain
            for (const auto& polygon : m_polygons) {
                if (polygon->contains(event->pos())) {
                    processClippingWithPolygon(polygon.get());
                    return;
                }
            }
        } else if (event->button() == Qt::RightButton) {
            // Finalize clipping
            finalizeClipping();
            return;
        }
    }

    if (event->button() == Qt::LeftButton) {
        m_lastPoint = event->pos();
        qDebug() << "Mouse press at:" << m_lastPoint;
        
        if (m_isColorMode) {
            // Check for line selection
            for (const auto& line : m_lines) {
                if (line->contains(m_lastPoint)) {
                    QColor color = QColorDialog::getColor(line->getColor(), this, "Select Color");
                    if (color.isValid()) {
                        line->setColor(color);
                        update();
                    }
                    return;
                }
            }
            
            // Check for circle selection
            for (const auto& circle : m_circles) {
                if (circle->contains(m_lastPoint)) {
                    QColor color = QColorDialog::getColor(circle->getColor(), this, "Select Color");
                    if (color.isValid()) {
                        circle->setColor(color);
                        update();
                    }
                    return;
                }
            }
            
            // Check for polygon selection
            for (const auto& polygon : m_polygons) {
                if (polygon->contains(m_lastPoint)) {
                    QColor color = QColorDialog::getColor(polygon->getColor(), this, "Select Color");
                    if (color.isValid()) {
                        polygon->setColor(color);
                        update();
                    }
                    return;
                }
            }
            
            // Check for rectangle selection
            for (const auto& rect : m_rectangles) {
                if (rect->contains(m_lastPoint)) {
                    QColor color = QColorDialog::getColor(rect->getColor(), this, "Select Color");
                    if (color.isValid()) {
                        rect->setColor(color);
                        update();
                    }
                    return;
                }
            }
        } else if (m_isDrawing) {
            // Start drawing a new line
            m_currentLine = new Line(m_lastPoint, m_lastPoint);
            qDebug() << "Started new line";
        } else if (m_isCircleMode) {
            // Start drawing a new circle
            m_currentCircle = new Circle(m_lastPoint, 0);
            qDebug() << "Started new circle";
        } else if (m_isRectangleMode) {
            // Start drawing a new rectangle on first press
            m_currentRectangle = new Rectangle(m_lastPoint, m_lastPoint);
            qDebug() << "Started new rectangle";
        } else if (m_isPolygonMode) {
            if (!m_currentPolygon) {
                // Start a new polygon
                m_currentPolygon = new Polygon();
                m_currentPolygon->addVertex(m_lastPoint);
                qDebug() << "Started new polygon";
            } else {
                // Check if we're closing the polygon
                if (m_currentPolygon->getVertexCount() >= 3) {
                    QPoint firstVertex = m_currentPolygon->getVertex(0);
                    int dx = m_lastPoint.x() - firstVertex.x();
                    int dy = m_lastPoint.y() - firstVertex.y();
                    int distanceSquared = dx * dx + dy * dy;
                    
                    if (distanceSquared <= 100) { // 10 pixel threshold
                        m_currentPolygon->close();
                        addPolygon(std::unique_ptr<Polygon>(m_currentPolygon));
                        m_currentPolygon = nullptr;
                        qDebug() << "Polygon closed";
                        return;
                    }
                }
                
                // Add new vertex
                m_currentPolygon->addVertex(m_lastPoint);
                qDebug() << "Added vertex to polygon";
            }
            update(); // Force update to show the current polygon
        } else if (m_isThicknessMode) {
            // Check if we clicked on a line or polygon or rectangle to change thickness
            for (const auto& line : m_lines) {
                if (line->contains(m_lastPoint)) {
                    handleThicknessChange(line.get(), true);
                    break;
                }
            }
            for (const auto& polygon : m_polygons) {
                if (polygon->contains(m_lastPoint)) {
                    handlePolygonThicknessChange(polygon.get(), true);
                    break;
                }
            }
            for (const auto& rect : m_rectangles) {
                if (rect->contains(m_lastPoint)) {
                    handleRectangleThicknessChange(rect.get(), true);
                    break;
                }
            }
        } else {
            // Check for polygon vertex/edge selection
            for (const auto& polygon : m_polygons) {
                int vertexIndex;
                if (polygon->isNearVertex(m_lastPoint, vertexIndex)) {
                    m_selectedPolygon = polygon.get();
                    m_selectedVertexIndex = vertexIndex;
                    m_isDraggingVertex = true;
                    qDebug() << "Selected polygon vertex";
                    return;
                }
                
                int edgeIndex;
                if (polygon->isNearEdge(m_lastPoint, edgeIndex)) {
                    m_selectedPolygon = polygon.get();
                    m_selectedEdgeIndex = edgeIndex;
                    m_isDraggingEdge = true;
                    qDebug() << "Selected polygon edge";
                    return;
                }

                // Check for polygon interior (for whole polygon dragging)
                if (polygon->contains(m_lastPoint)) {
                    m_selectedPolygon = polygon.get();
                    m_isDraggingPolygon = true;
                    qDebug() << "Selected polygon for dragging";
                    return;
                }
            }
            
            // Check for circle operations
            if (!m_selectedPolygon) {
                for (const auto& circle : m_circles) {
                    if (circle->isNearCenter(m_lastPoint)) {
                        m_selectedCircle = circle.get();
                        m_isDraggingCenter = true;
                        qDebug() << "Selected circle center";
                        break;
                    } else if (circle->isNearRadius(m_lastPoint)) {
                        m_selectedCircle = circle.get();
                        m_isDraggingRadius = true;
                        qDebug() << "Selected circle radius";
                        break;
                    }
                }
            }
            
            // Check for line operations
            if (!m_selectedPolygon && !m_selectedCircle) {
                for (const auto& line : m_lines) {
                    bool isStart;
                    if (line->isNearEndpoint(m_lastPoint, isStart)) {
                        m_selectedLine = line.get();
                        m_isDraggingEndpoint = true;
                        m_isDraggingStartPoint = isStart;
                        qDebug() << "Selected endpoint of line";
                        break;
                    }
                }
            }

            // Check for rectangle operations
            if (!m_selectedPolygon) {
                for (const auto& rect : m_rectangles) {
                    int vIdx;
                    if (rect->isNearVertex(m_lastPoint, vIdx)) {
                        m_selectedRectangle = rect.get();
                        m_selectedRectVertexIndex = vIdx;
                        m_isDraggingRectVertex = true;
                        qDebug() << "Selected rectangle vertex";
                        return;
                    }
                    int eIdx;
                    if (rect->isNearEdge(m_lastPoint, eIdx)) {
                        m_selectedRectangle = rect.get();
                        m_selectedRectEdgeIndex = eIdx;
                        m_isDraggingRectEdge = true;
                        qDebug() << "Selected rectangle edge";
                        return;
                    }
                    if (rect->contains(m_lastPoint)) {
                        m_selectedRectangle = rect.get();
                        m_isDraggingRectangle = true;
                        qDebug() << "Selected rectangle for dragging";
                        return;
                    }
                }
            }
        }
    } else if (event->button() == Qt::RightButton) {
        if (m_isDrawing) {
            // Remove line
            for (const auto& line : m_lines) {
                if (line->contains(event->pos())) {
                    removeLine(line.get());
                    qDebug() << "Line removed";
                    break;
                }
            }
        } else if (m_isCircleMode) {
            // Remove circle
            for (const auto& circle : m_circles) {
                if (circle->contains(event->pos())) {
                    removeCircle(circle.get());
                    qDebug() << "Circle removed";
                    break;
                }
            }
        } else if (m_isPolygonMode) {
            // Remove polygon
            for (const auto& polygon : m_polygons) {
                if (polygon->contains(event->pos())) {
                    removePolygon(polygon.get());
                    qDebug() << "Polygon removed";
                    break;
                }
            }
        } else if (m_isRectangleMode) {
            // Remove rectangle
            for (const auto& rect : m_rectangles) {
                if (rect->contains(event->pos())) {
                    removeRectangle(rect.get());
                    qDebug() << "Rectangle removed";
                    break;
                }
            }
        } else if (m_isThicknessMode) {
            // Decrease thickness lines/polygons/rectangles
            for (const auto& line : m_lines) {
                if (line->contains(event->pos())) {
                    handleThicknessChange(line.get(), false);
                    break;
                }
            }
            for (const auto& polygon : m_polygons) {
                if (polygon->contains(event->pos())) {
                    handlePolygonThicknessChange(polygon.get(), false);
                    break;
                }
            }
            for (const auto& rect : m_rectangles) {
                if (rect->contains(event->pos())) {
                    handleRectangleThicknessChange(rect.get(), false);
                    break;
                }
            }
        }
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDrawing && m_currentLine) {
        // Update the end point of the current line
        m_currentLine->setEndPoint(event->pos());
        qDebug() << "Updating line to:" << event->pos();
        update();
    } else if (m_isCircleMode && m_currentCircle) {
        // Update the radius of the current circle
        int dx = event->pos().x() - m_currentCircle->getCenter().x();
        int dy = event->pos().y() - m_currentCircle->getCenter().y();
        int radius = static_cast<int>(std::sqrt(dx * dx + dy * dy));
        m_currentCircle->setRadius(radius);
        update();
    } else if (m_isPolygonMode && m_currentPolygon) {
        // Update the last vertex position during polygon creation
        if (m_currentPolygon->getVertexCount() > 0) {
            m_currentPolygon->setVertex(m_currentPolygon->getVertexCount() - 1, event->pos());
            update();
        }
    } else if (m_isDraggingCenter && m_selectedCircle) {
        // Move the circle's center
        QPoint offset = event->pos() - m_lastPoint;
        m_selectedCircle->move(offset);
        m_lastPoint = event->pos();
        update();
    } else if (m_isDraggingRadius && m_selectedCircle) {
        // Change the circle's radius
        handleRadiusChange(m_selectedCircle, event->pos());
        update();
    } else if (m_isDraggingEndpoint && m_selectedLine) {
        // Move the selected endpoint
        if (m_isDraggingStartPoint) {
            m_selectedLine->setStartPoint(event->pos());
        } else {
            m_selectedLine->setEndPoint(event->pos());
        }
        update();
    } else if (m_isDraggingVertex && m_selectedPolygon) {
        // Move the selected vertex
        m_selectedPolygon->setVertex(m_selectedVertexIndex, event->pos());
        update();
    } else if (m_isDraggingEdge && m_selectedPolygon) {
        // Move the selected edge
        QPoint offset = event->pos() - m_lastPoint;
        m_selectedPolygon->moveEdge(m_selectedEdgeIndex, offset);
        m_lastPoint = event->pos();
        update();
    } else if (m_isDraggingPolygon && m_selectedPolygon) {
        // Move the entire polygon
        QPoint offset = event->pos() - m_lastPoint;
        m_selectedPolygon->move(offset);
        m_lastPoint = event->pos();
        update();
    } else if (m_isRectangleMode && m_currentRectangle) {
        // Update opposite corner while drawing
        m_currentRectangle->setOppositeCorner(event->pos());
        update();
    } else if (m_isDraggingRectangle && m_selectedRectangle) {
        // Move the entire rectangle
        QPoint offset = event->pos() - m_lastPoint;
        m_selectedRectangle->move(offset);
        m_lastPoint = event->pos();
        update();
    } else if (m_isDraggingRectVertex && m_selectedRectangle) {
        // Move a rectangle vertex
        m_selectedRectangle->moveVertex(m_selectedRectVertexIndex, event->pos());
        update();
    } else if (m_isDraggingRectEdge && m_selectedRectangle) {
        // Move rectangle edge
        QPoint offset = event->pos() - m_lastPoint;
        m_selectedRectangle->moveEdge(m_selectedRectEdgeIndex, offset);
        m_lastPoint = event->pos();
        update();
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_isDrawing && m_currentLine) {
            // Add the completed line to the lines list
            addLine(std::unique_ptr<Line>(m_currentLine));
            m_currentLine = nullptr;
            qDebug() << "Line completed and added to lines";
        } else if (m_isCircleMode && m_currentCircle) {
            // Add the completed circle to the circles list
            addCircle(std::unique_ptr<Circle>(m_currentCircle));
            m_currentCircle = nullptr;
            qDebug() << "Circle completed and added to circles";
        } else if (m_isRectangleMode && m_currentRectangle) {
            // Add completed rectangle
            addRectangle(std::unique_ptr<Rectangle>(m_currentRectangle));
            m_currentRectangle = nullptr;
            qDebug() << "Rectangle completed and added";
        }
        m_isDraggingEndpoint = false;
        m_isDraggingStartPoint = false;
        m_isDraggingCenter = false;
        m_isDraggingRadius = false;
        m_isDraggingVertex = false;
        m_isDraggingEdge = false;
        m_isDraggingPolygon = false;
        m_isDraggingRectVertex = false;
        m_isDraggingRectEdge = false;
        m_isDraggingRectangle = false;
        m_selectedLine = nullptr;
        m_selectedCircle = nullptr;
        m_selectedPolygon = nullptr;
        m_selectedVertexIndex = -1;
        m_selectedEdgeIndex = -1;
        m_selectedRectVertexIndex = -1;
        m_selectedRectEdgeIndex = -1;
    }
}

void Canvas::clearCanvas()
{
    m_lines.clear();
    m_circles.clear();
    m_polygons.clear();
    m_rectangles.clear();
    update();
}

void Canvas::addLine(std::unique_ptr<Line> line)
{
    m_lines.push_back(std::move(line));
    update();
}

void Canvas::removeLine(Line* line)
{
    auto it = std::find_if(m_lines.begin(), m_lines.end(),
        [line](const std::unique_ptr<Line>& l) { return l.get() == line; });
    
    if (it != m_lines.end()) {
        m_lines.erase(it);
        update();
    }
}

void Canvas::addCircle(std::unique_ptr<Circle> circle)
{
    m_circles.push_back(std::move(circle));
    update();
}

void Canvas::removeCircle(Circle* circle)
{
    auto it = std::find_if(m_circles.begin(), m_circles.end(),
        [circle](const std::unique_ptr<Circle>& c) { return c.get() == circle; });
    
    if (it != m_circles.end()) {
        m_circles.erase(it);
        update();
    }
}

void Canvas::addPolygon(std::unique_ptr<Polygon> polygon)
{
    m_polygons.push_back(std::move(polygon));
    update();
}

void Canvas::removePolygon(Polygon* polygon)
{
    auto it = std::find_if(m_polygons.begin(), m_polygons.end(),
        [polygon](const std::unique_ptr<Polygon>& p) { return p.get() == polygon; });
    
    if (it != m_polygons.end()) {
        m_polygons.erase(it);
        update();
    }
}

void Canvas::addRectangle(std::unique_ptr<Rectangle> rect)
{
    m_rectangles.push_back(std::move(rect));
    update();
}

void Canvas::removeRectangle(Rectangle* rect)
{
    auto it = std::find_if(m_rectangles.begin(), m_rectangles.end(),
        [rect](const std::unique_ptr<Rectangle>& r){ return r.get() == rect; });
    if (it != m_rectangles.end()) {
        m_rectangles.erase(it);
        update();
    }
}

void Canvas::handleThicknessChange(Line* line, bool increase)
{
    if (!line) return;
    
    int currentThickness = line->getThickness();
    int newThickness = increase ? currentThickness + 1 : std::max(1, currentThickness - 1);
    
    line->setThickness(newThickness);
    update();
    qDebug() << "Line thickness changed to:" << newThickness;
}

void Canvas::handlePolygonThicknessChange(Polygon* polygon, bool increase)
{
    if (!polygon) return;
    
    int currentThickness = polygon->getThickness();
    int newThickness = increase ? currentThickness + 1 : std::max(1, currentThickness - 1);
    
    polygon->setThickness(newThickness);
    update();
    qDebug() << "Polygon thickness changed to:" << newThickness;
}

void Canvas::handleRadiusChange(Circle* circle, const QPoint& newPoint)
{
    if (!circle) return;
    
    int dx = newPoint.x() - circle->getCenter().x();
    int dy = newPoint.y() - circle->getCenter().y();
    int newRadius = static_cast<int>(std::sqrt(dx * dx + dy * dy));
    
    circle->setRadius(newRadius);
    qDebug() << "Circle radius changed to:" << newRadius;
}

void Canvas::handleRectangleThicknessChange(Rectangle* rect, bool increase)
{
    if (!rect) return;
    int current = rect->getThickness();
    int newT = increase ? current+1 : std::max(1, current-1);
    rect->setThickness(newT);
    update();
    qDebug() << "Rectangle thickness changed to:" << newT;
}

void Canvas::setAntiAliasing(bool enabled)
{
    m_antiAliasing = enabled;
    updateAllObjectsAntiAliasing();
    update();
}

void Canvas::updateAllObjectsAntiAliasing()
{
    // Update lines
    for (const auto& line : m_lines) {
        line->setAntiAliasing(m_antiAliasing);
    }
    if (m_currentLine) {
        m_currentLine->setAntiAliasing(m_antiAliasing);
    }

    // Update circles
    for (const auto& circle : m_circles) {
        circle->setAntiAliasing(m_antiAliasing);
    }
    if (m_currentCircle) {
        m_currentCircle->setAntiAliasing(m_antiAliasing);
    }

    // Update polygons
    for (const auto& polygon : m_polygons) {
        polygon->setAntiAliasing(m_antiAliasing);
    }
    if (m_currentPolygon) {
        m_currentPolygon->setAntiAliasing(m_antiAliasing);
    }

    // Update rectangles
    for (const auto& rect : m_rectangles) {
        rect->setAntiAliasing(m_antiAliasing);
    }
    if (m_currentRectangle) {
        m_currentRectangle->setAntiAliasing(m_antiAliasing);
    }
}

// ==== Clipping helper functions ====
void Canvas::processClippingWithPolygon(Polygon* selectedPolygon)
{
    if (!selectedPolygon) return;

    // Avoid adding same polygon twice
    if (std::find(m_clipSelections.begin(), m_clipSelections.end(), selectedPolygon) != m_clipSelections.end()) {
        qDebug() << "Polygon already selected for clipping";
        return;
    }

    m_clipSelections.push_back(selectedPolygon);
    qDebug() << "Polygon added to clipping selections. Total:" << m_clipSelections.size();

    // If this is the first polygon, just store its vertices as current result
    if (m_clipSelections.size() == 1) {
        m_clipResultVertices = selectedPolygon->getVertices();
    } else {
        // Perform clipping of current result with new polygon
        std::vector<QPoint> clipVertices = selectedPolygon->getVertices();
        if (!selectedPolygon->isClosed()) {
            qDebug() << "Clip polygon is not closed. Aborting.";
            return;
        }

        m_clipResultVertices = sutherlandHodgman(m_clipResultVertices, clipVertices);
    }

    // Highlight selected polygon by changing its color temporarily
    if (m_clippingOldColors.find(selectedPolygon) == m_clippingOldColors.end()) {
        m_clippingOldColors[selectedPolygon] = selectedPolygon->getColor();
        selectedPolygon->setColor(Qt::blue);
    }

    update();
}

void Canvas::finalizeClipping()
{
    if (!m_isClippingMode) return;

    if (m_clipResultVertices.size() >= 3) {
        // Create new polygon from result vertices
        auto newPoly = std::make_unique<Polygon>();
        for (const QPoint& pt : m_clipResultVertices) {
            newPoly->addVertex(pt);
        }
        newPoly->close();
        newPoly->setColor(Qt::magenta); // highlight new polygon
        newPoly->setAntiAliasing(m_antiAliasing);
        addPolygon(std::move(newPoly));
        qDebug() << "Clipping finalized, new polygon added";
    } else {
        qDebug() << "Clipping result has insufficient vertices";
    }

    // Reset clipping state
    m_clipSelections.clear();
    m_clipResultVertices.clear();
    for (auto& pair : m_clippingOldColors) {
        pair.first->setColor(pair.second);
    }
    m_clippingOldColors.clear();
    m_isClippingMode = false;
    update();
}