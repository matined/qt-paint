#include "canvas.h"
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QColorDialog>

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
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
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
        } else if (m_isDrawing) {
            // Start drawing a new line
            m_currentLine = new Line(m_lastPoint, m_lastPoint);
            qDebug() << "Started new line";
        } else if (m_isCircleMode) {
            // Start drawing a new circle
            m_currentCircle = new Circle(m_lastPoint, 0);
            qDebug() << "Started new circle";
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
            // Check if we clicked on a line or polygon to change thickness
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
        } else {
            // Check for polygon vertex/edge selection
            for (const auto& polygon : m_polygons) {
                int vertexIndex;
                if (polygon->isNearVertex(m_lastPoint, vertexIndex)) {
                    m_selectedPolygon = polygon.get();
                    m_selectedVertexIndex = vertexIndex;
                    m_isDraggingVertex = true;
                    qDebug() << "Selected polygon vertex";
                    break;
                }
                
                int edgeIndex;
                if (polygon->isNearEdge(m_lastPoint, edgeIndex)) {
                    m_selectedPolygon = polygon.get();
                    m_isDraggingPolygon = true;
                    qDebug() << "Selected polygon edge";
                    break;
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
        } else if (m_isThicknessMode) {
            // Decrease thickness
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
    } else if (m_isDraggingPolygon && m_selectedPolygon) {
        // Move the entire polygon
        QPoint offset = event->pos() - m_lastPoint;
        m_selectedPolygon->move(offset);
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
        }
        m_isDraggingEndpoint = false;
        m_isDraggingStartPoint = false;
        m_isDraggingCenter = false;
        m_isDraggingRadius = false;
        m_isDraggingVertex = false;
        m_isDraggingPolygon = false;
        m_selectedLine = nullptr;
        m_selectedCircle = nullptr;
        m_selectedPolygon = nullptr;
        m_selectedVertexIndex = -1;
    }
}

void Canvas::clearCanvas()
{
    m_lines.clear();
    m_circles.clear();
    m_polygons.clear();
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