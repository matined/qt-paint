#include "canvas.h"
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

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
    
    // Draw current line if exists
    if (m_currentLine) {
        m_currentLine->draw(painter);
    }
    
    // Draw current circle if exists
    if (m_currentCircle) {
        m_currentCircle->draw(painter);
    }
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_lastPoint = event->pos();
        qDebug() << "Mouse press at:" << m_lastPoint;
        
        if (m_isDrawing) {
            // Start drawing a new line
            m_currentLine = new Line(m_lastPoint, m_lastPoint);
            qDebug() << "Started new line";
        } else if (m_isCircleMode) {
            // Start drawing a new circle
            m_currentCircle = new Circle(m_lastPoint, 0);
            qDebug() << "Started new circle";
        } else if (m_isThicknessMode) {
            // Check if we clicked on a line to increase its thickness
            for (const auto& line : m_lines) {
                if (line->contains(m_lastPoint)) {
                    handleThicknessChange(line.get(), true);
                    break;
                }
            }
        } else {
            // Check if we clicked on a circle's center or radius point
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
            
            // Check if we clicked on a line's endpoint
            if (!m_selectedCircle) {
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
            // Check if we right-clicked on a line to remove it
            for (const auto& line : m_lines) {
                if (line->contains(event->pos())) {
                    removeLine(line.get());
                    qDebug() << "Line removed";
                    break;
                }
            }
        } else if (m_isCircleMode) {
            // Check if we right-clicked on a circle to remove it
            for (const auto& circle : m_circles) {
                if (circle->contains(event->pos())) {
                    removeCircle(circle.get());
                    qDebug() << "Circle removed";
                    break;
                }
            }
        } else if (m_isThicknessMode) {
            // Check if we right-clicked on a line to decrease its thickness
            for (const auto& line : m_lines) {
                if (line->contains(event->pos())) {
                    handleThicknessChange(line.get(), false);
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
        m_isDraggingCenter = false;
        m_isDraggingRadius = false;
        m_selectedLine = nullptr;
        m_selectedCircle = nullptr;
    }
}

void Canvas::clearCanvas()
{
    m_lines.clear();
    m_circles.clear();
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

void Canvas::handleThicknessChange(Line* line, bool increase)
{
    if (!line) return;
    
    int currentThickness = line->getThickness();
    int newThickness = increase ? currentThickness + 1 : std::max(1, currentThickness - 1);
    
    line->setThickness(newThickness);
    update();
    qDebug() << "Line thickness changed to:" << newThickness;
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