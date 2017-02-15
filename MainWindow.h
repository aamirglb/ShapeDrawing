#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QList>
#include <QVector>
#include <QMap>

#include "Shapes.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    bool eventFilter(QObject *obj, QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent* event);
    void drawShapes(const QList<QPointF> &pointList, QColor color,
                    QBrush brush, QPainter *painter);
    ~MainWindow();

public slots:
    void drawModeClicked();
    void clearShapes();

private:
    Ui::MainWindow *ui;
    bool drawMode; // set to true to enable drawing
    QList<QPointF> m_clickPositions;
    //QVector<QList<QPointF>> m_shaps;
    QColor currentColor;
    QBrush currentBrush;
    QColor currentBrushColor;
    QVector<Shape> m_shapes;

    QMap<QString,  Qt::BrushStyle> brushStyle;
};

#endif // MAINWINDOW_H
