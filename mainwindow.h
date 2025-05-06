#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "canvas.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Canvas *canvas;
    QLabel *statusLabel;
    
    // Drawing tool buttons
    QPushButton *btnDrawLine;
    QPushButton *btnDrawCircle;
    QPushButton *btnDrawPolygon;
    QPushButton *btnDrawRectangle;
    QPushButton *btnResetMode;
    QPushButton *btnClip;
    
    // Style buttons
    QPushButton *btnChangeColor;
    QPushButton *btnThicken;
    QPushButton *btnToggleAntiAliasing;
    QPushButton *btnFill;
    
    // File operation buttons
    QPushButton *btnSave;
    QPushButton *btnLoad;
    QPushButton *btnRemoveAll;

private slots:
    // Drawing tool slots
    void onDrawLine();
    void onDrawCircle();
    void onDrawPolygon();
    void onDrawRectangle();
    void onResetMode();
    void onClip();
    void onFill();
    
    // Style slots
    void onChangeColor();
    void onThicken();
    void onToggleAntiAliasing();
    
    // File operation slots
    void onSave();
    void onLoad();
    void onRemoveAll();
};
#endif // MAINWINDOW_H
