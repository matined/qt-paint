#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPainter>
#include <vector>
#include <memory>
#include "line.h"
#include "circle.h"
#include "polygon.h"

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr);
    ~Canvas();

    void setDrawingMode(bool enabled) { m_isDrawing = enabled; }
    void setThicknessMode(bool enabled) { m_isThicknessMode = enabled; }
    void setCircleMode(bool enabled) { m_isCircleMode = enabled; }
    void setPolygonMode(bool enabled) { m_isPolygonMode = enabled; }
    void clearCanvas();
    void addLine(std::unique_ptr<Line> line);
    void removeLine(Line* line);
    void addCircle(std::unique_ptr<Circle> circle);
    void removeCircle(Circle* circle);
    void addPolygon(std::unique_ptr<Polygon> polygon);
    void removePolygon(Polygon* polygon);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_isDrawing = false;
    bool m_isThicknessMode = false;
    bool m_isCircleMode = false;
    bool m_isPolygonMode = false;
    Line* m_currentLine = nullptr;
    Circle* m_currentCircle = nullptr;
    Polygon* m_currentPolygon = nullptr;
    std::vector<std::unique_ptr<Line>> m_lines;
    std::vector<std::unique_ptr<Circle>> m_circles;
    std::vector<std::unique_ptr<Polygon>> m_polygons;
    QPoint m_lastPoint;
    bool m_isDraggingEndpoint = false;
    bool m_isDraggingStartPoint = false;
    bool m_isDraggingCenter = false;
    bool m_isDraggingRadius = false;
    bool m_isDraggingVertex = false;
    bool m_isDraggingPolygon = false;
    Line* m_selectedLine = nullptr;
    Circle* m_selectedCircle = nullptr;
    Polygon* m_selectedPolygon = nullptr;
    int m_selectedVertexIndex = -1;
    
    void handleThicknessChange(Line* line, bool increase);
    void handleRadiusChange(Circle* circle, const QPoint& newPoint);
    void handlePolygonThicknessChange(Polygon* polygon, bool increase);
};

#endif // CANVAS_H