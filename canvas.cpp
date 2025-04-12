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
    
    // Draw current line if exists
    if (m_currentLine) {
        m_currentLine->draw(painter);
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
        } else if (m_isThicknessMode) {
            // Check if we clicked on a line to increase its thickness
            for (const auto& line : m_lines) {
                if (line->contains(m_lastPoint)) {
                    handleThicknessChange(line.get(), true);
                    break;
                }
            }
        } else {
            // Check if we clicked on an endpoint
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

void Canvas::handleThicknessChange(Line* line, bool increase)
{
    if (!line) return;
    
    int currentThickness = line->getThickness();
    int newThickness = increase ? currentThickness + 1 : std::max(1, currentThickness - 1);
    
    line->setThickness(newThickness);
    update();
    qDebug() << "Line thickness changed to:" << newThickness;
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDrawing && m_currentLine) {
        // Update the end point of the current line
        m_currentLine->setEndPoint(event->pos());
        qDebug() << "Updating line to:" << event->pos();
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
        }
        m_isDraggingEndpoint = false;
        m_selectedLine = nullptr;
    }
}

void Canvas::clearCanvas()
{
    m_lines.clear();
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