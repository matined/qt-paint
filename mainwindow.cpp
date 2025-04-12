#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColorDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // Create and set up the canvas
    canvas = new Canvas(this);
    ui->verticalLayout->insertWidget(1, canvas);
    canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // Add left margin to status bar
    QWidget *spacer = new QWidget(this);
    spacer->setFixedWidth(10); // 10 pixel margin
    ui->statusBar->addWidget(spacer);
    
    // Create status label
    statusLabel = new QLabel("Mode: None", this);
    ui->statusBar->addWidget(statusLabel);
    
    // Get the drawing tool buttons
    btnDrawLine = ui->btnDrawLine;
    btnDrawCircle = ui->btnDrawCircle;
    btnDrawPolygon = ui->btnDrawPolygon;
    btnResetMode = ui->btnResetMode;
    
    // Get the style buttons
    btnChangeColor = ui->btnChangeColor;
    btnThicken = ui->btnThicken;
    btnToggleAntiAliasing = ui->btnToggleAntiAliasing;
    
    // Get the file operation buttons
    btnSave = ui->btnSave;
    btnLoad = ui->btnLoad;
    btnRemoveAll = ui->btnRemoveAll;
    
    // Connect drawing tool signals to slots
    connect(btnDrawLine, &QPushButton::clicked, this, &MainWindow::onDrawLine);
    connect(btnDrawCircle, &QPushButton::clicked, this, &MainWindow::onDrawCircle);
    connect(btnDrawPolygon, &QPushButton::clicked, this, &MainWindow::onDrawPolygon);
    connect(btnResetMode, &QPushButton::clicked, this, &MainWindow::onResetMode);
    
    // Connect style signals to slots
    connect(btnChangeColor, &QPushButton::clicked, this, &MainWindow::onChangeColor);
    connect(btnThicken, &QPushButton::clicked, this, &MainWindow::onThicken);
    connect(btnToggleAntiAliasing, &QPushButton::clicked, this, &MainWindow::onToggleAntiAliasing);
    
    // Connect file operation signals to slots
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSave);
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoad);
    connect(btnRemoveAll, &QPushButton::clicked, this, &MainWindow::onRemoveAll);
    
    qDebug() << "MainWindow initialized";
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Drawing tool slots
void MainWindow::onDrawLine()
{
    qDebug() << "Line drawing mode activated";
    canvas->setDrawingMode(true);
    canvas->setCircleMode(false);
    canvas->setThicknessMode(false);
    statusLabel->setText("Mode: Line Drawing (Right-click to remove lines)");
}

void MainWindow::onDrawCircle()
{
    qDebug() << "Circle drawing mode activated";
    canvas->setDrawingMode(false);
    canvas->setCircleMode(true);
    canvas->setThicknessMode(false);
    statusLabel->setText("Mode: Circle Drawing (Right-click to remove circles)");
}

void MainWindow::onDrawPolygon()
{
    qDebug() << "Polygon drawing mode activated";
    canvas->setDrawingMode(false);
    canvas->setCircleMode(false);
    canvas->setThicknessMode(false);
    statusLabel->setText("Mode: None");
}

void MainWindow::onResetMode()
{
    qDebug() << "Mode reset";
    canvas->setDrawingMode(false);
    canvas->setCircleMode(false);
    canvas->setThicknessMode(false);
    statusLabel->setText("Mode: None");
}

// Style slots
void MainWindow::onChangeColor()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "Select Color");
    if (color.isValid()) {
        // To be implemented - set color for new shapes
    }
}

void MainWindow::onThicken()
{
    qDebug() << "Thickness mode activated";
    canvas->setDrawingMode(false);
    canvas->setCircleMode(false);
    canvas->setThicknessMode(true);
    statusLabel->setText("Mode: Thickness (Left-click to increase, Right-click to decrease)");
}

void MainWindow::onToggleAntiAliasing()
{
    // To be implemented
}

// File operation slots
void MainWindow::onSave()
{
    // To be implemented
}

void MainWindow::onLoad()
{
    // To be implemented
}

void MainWindow::onRemoveAll()
{
    canvas->clearCanvas();
}
