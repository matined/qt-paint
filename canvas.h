#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPainter>
#include <vector>
#include <memory>
#include "line.h"
#include "circle.h"

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr);
    ~Canvas();

    void setDrawingMode(bool enabled) { m_isDrawing = enabled; }
    void setThicknessMode(bool enabled) { m_isThicknessMode = enabled; }
    void setCircleMode(bool enabled) { m_isCircleMode = enabled; }
    void clearCanvas();
    void addLine(std::unique_ptr<Line> line);
    void removeLine(Line* line);
    void addCircle(std::unique_ptr<Circle> circle);
    void removeCircle(Circle* circle);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_isDrawing = false;
    bool m_isThicknessMode = false;
    bool m_isCircleMode = false;
    Line* m_currentLine = nullptr;
    Circle* m_currentCircle = nullptr;
    std::vector<std::unique_ptr<Line>> m_lines;
    std::vector<std::unique_ptr<Circle>> m_circles;
    QPoint m_lastPoint;
    bool m_isDraggingEndpoint = false;
    bool m_isDraggingStartPoint = false;
    bool m_isDraggingCenter = false;
    bool m_isDraggingRadius = false;
    Line* m_selectedLine = nullptr;
    Circle* m_selectedCircle = nullptr;
    
    void handleThicknessChange(Line* line, bool increase);
    void handleRadiusChange(Circle* circle, const QPoint& newPoint);
};

#endif // CANVAS_H