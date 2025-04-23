# Qt Paint 🎨

A feature-rich painting application built with Qt that implements fundamental computer graphics algorithms for drawing and manipulating geometric shapes. ✨

![Qt Paint Banner](https://img.shields.io/badge/Qt-Paint-green?style=for-the-badge&logo=qt)
![C++](https://img.shields.io/badge/C++-17-blue?style=for-the-badge&logo=c%2B%2B)
![License](https://img.shields.io/badge/license-MIT-orange?style=for-the-badge)

## Screenshot 📸

![image](https://github.com/user-attachments/assets/7ece84b9-6b53-4775-a46b-617279bfe12b)

## ✨ Features

### 🎨 Drawing Tools
- **Line Drawing** 📏: Implementation of Digital Differential Analyzer (DDA) algorithm for line rasterization
- **Circle Drawing** ⭕: Implementation of Midpoint Circle algorithm
- **Polygon Drawing** 📐: Support for creating multi-vertex polygons with dynamic vertex addition
- **Anti-aliasing** 🔲: Wu's line algorithm implementation for smooth edges
- **Thickness Control** 📊: Adjustable line and polygon edge thickness using a circular brush pattern
- **Color Selection** 🌈: Dynamic color changing for all shapes

### 🔄 Shape Manipulation
- **Line Manipulation** ↔️: Drag endpoints to modify line position and orientation
- **Circle Manipulation** 🎯: 
  - Drag center point to move the circle
  - Drag radius control point to resize
- **Polygon Manipulation** 🔺:
  - Vertex dragging for shape modification
  - Edge dragging for parallel edge movement
  - Complete polygon translation
  - Support for both open and closed polygons

### 💾 File Operations
- **Save/Load** 📂: Persistent storage of drawings in a custom `.qtpaint` format
- **Clear Canvas** 🗑️: Option to reset the drawing area

## 🔧 Technical Implementation

### 🧮 Core Algorithms

#### Line Drawing (DDA Algorithm) 📏
- Uses floating-point arithmetic for precise pixel placement
- Implements slope-based pixel selection for smooth lines
- Supports arbitrary line angles and positions

#### Circle Drawing (Midpoint Algorithm) ⭕
- Efficient implementation using only integer arithmetic
- Symmetry optimization for reduced computation
- Support for arbitrary radius and center position

#### Anti-aliasing (Wu's Algorithm) 🔲
- Sub-pixel precision for smooth edge rendering
- Intensity-based color blending for reduced aliasing artifacts
- Applied to all geometric primitives

#### Brush Pattern Generation 🖌️
- Circular brush pattern for variable thickness
- Dynamic size adjustment
- Support for both aliased and anti-aliased rendering

### 🏗️ Architecture

#### Class Structure 📚
- `MainWindow`: Main application window and UI management
- `Canvas`: Core drawing surface and event handling
- `Line`, `Circle`, `Polygon`: Shape classes with specific drawing algorithms
- `Brush`: Implements thickness and pattern generation

#### Event Handling 🎮
- Mouse event tracking for interactive drawing
- Mode-based operation switching
- Drag-and-drop support for shape manipulation

## 🚀 Building and Running

### Prerequisites 📋
- Qt 6.0 or higher
- C++17 compatible compiler
- CMake 3.16 or higher

### Build Instructions 🛠️
1. Clone the repository
2. Open the project in Qt Creator
3. Configure the project for your Qt version
4. Build and run

### Platform Support 💻
- Tested on macOS
- Should work on Windows and Linux (Qt is cross-platform)

## 📖 Usage Guide

### Drawing Shapes ✏️
1. Select the desired drawing tool (Line, Circle, or Polygon)
2. Click and drag to draw lines and circles
3. For polygons:
   - Click to add vertices
   - Click near the first vertex to close the polygon
   - Right-click to cancel

### Manipulating Shapes 🎯
1. Click and drag:
   - Line endpoints
   - Circle center or radius point
   - Polygon vertices or edges
2. Use the thickness control to adjust line width
3. Use the color picker to change shape colors

### File Operations 📁
1. Save your work using the Save button (creates a .qtpaint file)
2. Load existing drawings using the Load button
3. Clear the canvas using Remove All

## 🔍 Implementation Details

### Anti-aliasing Implementation 🔲
The project implements Wu's line algorithm for anti-aliased rendering:
- Calculates sub-pixel positions
- Uses alpha blending for smooth edges
- Supports all geometric primitives

### Brush Pattern Generation 🖌️
- Creates circular patterns for variable thickness
- Supports sizes from 1 to any reasonable value
- Implements efficient pattern caching

### Event System 🎮
- Robust mouse event handling
- Mode-based drawing system
- Support for multiple simultaneous operations

## 🚀 Future Improvements
1. Add support for curved lines (Bézier curves) ➰
2. Implement shape filling algorithms 🎨
3. Add layer support for complex drawings 📑
4. Implement undo/redo functionality ↩️
5. Add text drawing capabilities ✍️
6. Implement more advanced anti-aliasing techniques 🔲

## 👥 Contributing
Contributions are welcome! Please feel free to submit pull requests. Together we can make Qt Paint even better! 🌟

## 📄 License
This project is open source and available under the MIT License. 

---
<div align="center">
Made with ❤️ using Qt and C++

[Report Bug](../../issues) · [Request Feature](../../issues)
</div> 
