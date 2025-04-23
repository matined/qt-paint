#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

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
    btnDrawPacman = ui->btnDrawPacman;
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
    connect(btnDrawPacman, &QPushButton::clicked, this, &MainWindow::onDrawPacman);
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
    canvas->setPolygonMode(false);
    canvas->setPacmanMode(false);
    canvas->setThicknessMode(false);
    canvas->setColorMode(false);
    statusLabel->setText("Mode: Line Drawing (Right-click to remove lines)");
}

void MainWindow::onDrawCircle()
{
    qDebug() << "Circle drawing mode activated";
    canvas->setDrawingMode(false);
    canvas->setCircleMode(true);
    canvas->setPolygonMode(false);
    canvas->setPacmanMode(false);
    canvas->setThicknessMode(false);
    canvas->setColorMode(false);
    statusLabel->setText("Mode: Circle Drawing (Right-click to remove circles)");
}

void MainWindow::onDrawPolygon()
{
    qDebug() << "Polygon drawing mode activated";
    canvas->setDrawingMode(false);
    canvas->setCircleMode(false);
    canvas->setPolygonMode(true);
    canvas->setPacmanMode(false);
    canvas->setThicknessMode(false);
    canvas->setColorMode(false);
    statusLabel->setText("Mode: Polygon Drawing (Click to add vertices, click near first vertex to close, Right-click to remove)");
}

void MainWindow::onDrawPacman()
{
    qDebug() << "Pacman drawing mode activated";
    canvas->setDrawingMode(false);
    canvas->setCircleMode(false);
    canvas->setPolygonMode(false);
    canvas->setPacmanMode(true);
    canvas->setThicknessMode(false);
    canvas->setColorMode(false);
    statusLabel->setText("Mode: Pacman Drawing (3 clicks: center, radius point, mouth end point. Right-click to remove)");
}

void MainWindow::onResetMode()
{
    qDebug() << "Mode reset";
    canvas->setDrawingMode(false);
    canvas->setCircleMode(false);
    canvas->setPolygonMode(false);
    canvas->setPacmanMode(false);
    canvas->setThicknessMode(false);
    canvas->setColorMode(false);
    statusLabel->setText("Mode: None");
}

// Style slots
void MainWindow::onChangeColor()
{
    canvas->setColorMode(true);
    statusLabel->setText("Mode: Color Change (Click on an object to change its color)");
}

void MainWindow::onThicken()
{
    qDebug() << "Thickness mode activated";
    canvas->setDrawingMode(false);
    canvas->setCircleMode(false);
    canvas->setPolygonMode(false);
    canvas->setPacmanMode(false);
    canvas->setThicknessMode(true);
    canvas->setColorMode(false);
    statusLabel->setText("Mode: Thickness (Left-click to increase, Right-click to decrease)");
}

void MainWindow::onToggleAntiAliasing()
{
    static bool antiAliasingEnabled = false;
    antiAliasingEnabled = !antiAliasingEnabled;
    canvas->setAntiAliasing(antiAliasingEnabled);
    statusLabel->setText(QString("Anti-aliasing: %1").arg(antiAliasingEnabled ? "Enabled" : "Disabled"));
}

// File operation slots
void MainWindow::onSave()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Drawing", "", "QtPaint Files (*.qtpaint)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file for writing");
        return;
    }

    QTextStream out(&file);
    
    // Save lines
    for (const auto& line : canvas->getLines()) {
        out << "LINE " 
            << line->getStartPoint().x() << " " 
            << line->getStartPoint().y() << " "
            << line->getEndPoint().x() << " "
            << line->getEndPoint().y() << " "
            << line->getColor().name() << " "
            << line->getThickness() << "\n";
    }
    
    // Save circles
    for (const auto& circle : canvas->getCircles()) {
        out << "CIRCLE "
            << circle->getCenter().x() << " "
            << circle->getCenter().y() << " "
            << circle->getRadius() << " "
            << circle->getColor().name() << "\n";
    }
    
    // Save polygons
    for (const auto& polygon : canvas->getPolygons()) {
        out << "POLYGON ";
        for (int i = 0; i < polygon->getVertexCount(); ++i) {
            QPoint vertex = polygon->getVertex(i);
            out << vertex.x() << " " << vertex.y() << " ";
        }
        out << polygon->getColor().name() << " "
            << polygon->getThickness() << " "
            << (polygon->isClosed() ? "1" : "0") << "\n";
    }
    
    // Save pacmans
    for (const auto& pacman : canvas->getPacmans()) {
        out << "PACMAN "
            << pacman->getCenter().x() << " "
            << pacman->getCenter().y() << " "
            << pacman->getRadius() << " "
            << pacman->getStartAngle() << " "
            << pacman->getEndAngle() << "\n";
    }

    file.close();
    statusLabel->setText("Drawing saved successfully");
}

void MainWindow::onLoad()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Load Drawing", "", "QtPaint Files (*.qtpaint)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file for reading");
        return;
    }

    // Clear existing canvas
    canvas->clearCanvas();

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(" ");
        
        if (parts.isEmpty()) continue;

        if (parts[0] == "LINE" && parts.size() >= 7) {
            QPoint start(parts[1].toInt(), parts[2].toInt());
            QPoint end(parts[3].toInt(), parts[4].toInt());
            QColor color(parts[5]);
            int thickness = parts[6].toInt();
            
            auto newLine = std::make_unique<Line>(start, end);
            newLine->setColor(color);
            newLine->setThickness(thickness);
            canvas->addLine(std::move(newLine));
        }
        else if (parts[0] == "CIRCLE" && parts.size() >= 5) {
            QPoint center(parts[1].toInt(), parts[2].toInt());
            int radius = parts[3].toInt();
            QColor color(parts[4]);
            
            auto newCircle = std::make_unique<Circle>(center, radius);
            newCircle->setColor(color);
            canvas->addCircle(std::move(newCircle));
        }
        else if (parts[0] == "POLYGON" && parts.size() >= 4) {
            auto newPolygon = std::make_unique<Polygon>();
            int i = 1;
            
            // Read vertices (pairs of x,y coordinates)
            while (i + 1 < parts.size() - 3) {
                QPoint vertex(parts[i].toInt(), parts[i+1].toInt());
                newPolygon->addVertex(vertex);
                i += 2;
            }
            
            // Read color, thickness, and closed status
            QColor color(parts[i]);
            int thickness = parts[i+1].toInt();
            bool isClosed = parts[i+2].toInt() == 1;
            
            newPolygon->setColor(color);
            newPolygon->setThickness(thickness);
            if (isClosed) {
                newPolygon->close();
            }
            
            canvas->addPolygon(std::move(newPolygon));
        }
        else if (parts[0] == "PACMAN" && parts.size() >= 6) {
            QPoint center(parts[1].toInt(), parts[2].toInt());
            int radius = parts[3].toInt();
            float startAngle = parts[4].toFloat();
            float endAngle = parts[5].toFloat();
            
            auto newPacman = std::make_unique<Pacman>(center);
            // Create a radius point based on the radius and startAngle
            QPoint radiusPoint(
                center.x() + static_cast<int>(radius * std::cos(startAngle)),
                center.y() + static_cast<int>(radius * std::sin(startAngle))
            );
            newPacman->setRadiusPoint(radiusPoint);
            
            // Create a mouth point based on the endAngle
            QPoint mouthPoint(
                center.x() + static_cast<int>(radius * std::cos(endAngle)),
                center.y() + static_cast<int>(radius * std::sin(endAngle))
            );
            newPacman->setMouthPoint(mouthPoint);
            
            canvas->addPacman(std::move(newPacman));
        }
    }

    file.close();
    statusLabel->setText("Drawing loaded successfully");
}

void MainWindow::onRemoveAll()
{
    canvas->clearCanvas();
}
