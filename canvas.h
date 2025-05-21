#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPainter>
#include <vector>
#include <memory>
#include "line.h"
#include "circle.h"
#include "polygon.h"
#include "rectangle.h"
#include <unordered_map>

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
    void setRectangleMode(bool enabled) { m_isRectangleMode = enabled; }
    void setClippingMode(bool enabled) { m_isClippingMode = enabled; }
    void setColorMode(bool enabled) { m_isColorMode = enabled; }
    void setFillMode(bool enabled) { m_isFillMode = enabled; }
    void setImageFillMode(bool enabled) { m_isImageFillMode = enabled; }
    void setFloodFillMode(bool enabled) { m_isFloodFillMode = enabled; }
    void setAntiAliasing(bool enabled);
    void clearCanvas();
    void addLine(std::unique_ptr<Line> line);
    void removeLine(Line* line);
    void addCircle(std::unique_ptr<Circle> circle);
    void removeCircle(Circle* circle);
    void addPolygon(std::unique_ptr<Polygon> polygon);
    void removePolygon(Polygon* polygon);
    void addRectangle(std::unique_ptr<Rectangle> rect);
    void removeRectangle(Rectangle* rect);

    // Getter methods for saving
    const std::vector<std::unique_ptr<Line>>& getLines() const { return m_lines; }
    const std::vector<std::unique_ptr<Circle>>& getCircles() const { return m_circles; }
    const std::vector<std::unique_ptr<Polygon>>& getPolygons() const { return m_polygons; }
    const std::vector<std::unique_ptr<Rectangle>>& getRectangles() const { return m_rectangles; }

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
    bool m_isRectangleMode = false;
    bool m_isClippingMode = false;
    bool m_isColorMode = false;
    bool m_isFillMode = false;
    bool m_isImageFillMode = false;
    bool m_isFloodFillMode = false;
    bool m_antiAliasing = false;
    Line* m_currentLine = nullptr;
    Circle* m_currentCircle = nullptr;
    Polygon* m_currentPolygon = nullptr;
    Rectangle* m_currentRectangle = nullptr;
    std::vector<std::unique_ptr<Line>> m_lines;
    std::vector<std::unique_ptr<Circle>> m_circles;
    std::vector<std::unique_ptr<Polygon>> m_polygons;
    std::vector<std::unique_ptr<Rectangle>> m_rectangles;
    QPoint m_lastPoint;
    bool m_isDraggingEndpoint = false;
    bool m_isDraggingStartPoint = false;
    bool m_isDraggingCenter = false;
    bool m_isDraggingRadius = false;
    bool m_isDraggingVertex = false;
    bool m_isDraggingEdge = false;
    bool m_isDraggingPolygon = false;
    bool m_isDraggingRectVertex = false;
    bool m_isDraggingRectEdge = false;
    bool m_isDraggingRectangle = false;
    Line* m_selectedLine = nullptr;
    Circle* m_selectedCircle = nullptr;
    Polygon* m_selectedPolygon = nullptr;
    Rectangle* m_selectedRectangle = nullptr;
    int m_selectedVertexIndex = -1;
    int m_selectedEdgeIndex = -1;
    int m_selectedRectVertexIndex = -1;
    int m_selectedRectEdgeIndex = -1;
    std::vector<Polygon*> m_clipSelections;
    std::vector<QPoint> m_clipResultVertices;
    std::unordered_map<Polygon*, QColor> m_clippingOldColors;
    
    void handleThicknessChange(Line* line, bool increase);
    void handleRadiusChange(Circle* circle, const QPoint& newPoint);
    void handlePolygonThicknessChange(Polygon* polygon, bool increase);
    void handleRectangleThicknessChange(Rectangle* rect, bool increase);
    void updateAllObjectsAntiAliasing();
    void processClippingWithPolygon(Polygon* selectedPolygon);
    void finalizeClipping();
};

#endif // CANVAS_H