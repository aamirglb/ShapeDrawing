#ifndef SHAPES_H
#define SHAPES_H

#include <QList>
#include <QPointF>
#include <QColor>
#include <QBrush>

// Basic shapes class, holds the points and draw color
///
/// \brief The Shape class
///  Basic shape class to holds the shape points and draw color, brush style
///  and brush color.
///
class Shape {

public:
    QList<QPointF> m_points;
    QColor m_drawColor; // Draw shape with this color
    QBrush m_brush;
    QColor m_brushColor;

    // Default constructor
    Shape() {}

    ///
    /// \brief Shape
    ///  Main constructor of the shape class.
    /// \param points
    /// \param color
    /// \param brush
    /// \param brushColor
    ///
    Shape(QList<QPointF> points, QColor color, QBrush brush, QColor brushColor) {
        m_points = points;
        m_drawColor = color;
        m_brush = brush;
        m_brushColor = brushColor;
    }
};

#endif // SHAPES_H
