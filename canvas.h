#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPainter>
#include <vector>
#include <memory>
#include "line.h"

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr);
    ~Canvas();

    void setDrawingMode(bool enabled) { m_isDrawing = enabled; }
    void setThicknessMode(bool enabled) { m_isThicknessMode = enabled; }
    void clearCanvas();
    void addLine(std::unique_ptr<Line> line);
    void removeLine(Line* line);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_isDrawing = false;
    bool m_isThicknessMode = false;
    Line* m_currentLine = nullptr;
    std::vector<std::unique_ptr<Line>> m_lines;
    QPoint m_lastPoint;
    bool m_isDraggingEndpoint = false;
    bool m_isDraggingStartPoint = false;
    Line* m_selectedLine = nullptr;
    
    void handleThicknessChange(Line* line, bool increase);
};

#endif // CANVAS_H