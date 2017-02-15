#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMouseEvent>
#include <QDebug>
#include <Shapes.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qApp->installEventFilter(this);
    drawMode = false;

    QStringList drawingColors;
    //drawingColors << QColor::colorNames();
    drawingColors << "acqua" <<  "blue" <<  "black" <<  "brown" <<  "coral" <<  "cyan"
                  <<  "gold" <<  "gray" <<  "green" <<  "lime" <<  "magenta" <<  "maroon"
                  <<  "navy" <<  "olive" <<  "orange" <<  "pink" <<  "red" <<  "white"
                  <<  "yellow";
    ui->colorCombo->insertItems(0, drawingColors);
    ui->brushColorCombo->insertItems(0, drawingColors);

    QStringList brushStyleStr;
    brushStyleStr << "No Brush" << "Solid Pattern" << "Dense-1 Pattern" << "Dense-2"
               << "Dense-3" << "Dense-4" << "Dense-5" << "Dense-6" << "Dense-7"
               << "Horizontal" << "Vertical" << "Cross" << "BDiag" << "FDiag" << "DiagCross";
    ui->brushCombo->insertItems(0, brushStyleStr);

    brushStyle["No Brush"] = Qt::NoBrush;
    brushStyle["Solid Pattern"] = Qt::SolidPattern;
    brushStyle["Dense-1 Pattern"] = Qt::Dense1Pattern;
    brushStyle["Dense-2"] = Qt::Dense2Pattern;
    brushStyle["Dense-3"] = Qt::Dense3Pattern;
    brushStyle["Dense-4"] = Qt::Dense4Pattern;
    brushStyle["Dense-5"] = Qt::Dense5Pattern;
    brushStyle["Dense-6"] = Qt::Dense6Pattern;
    brushStyle["Dense-7"] = Qt::Dense7Pattern;
    brushStyle["Horizontal"] = Qt::HorPattern;
    brushStyle["Vertical"] = Qt::VerPattern;
    brushStyle["Cross"] = Qt::CrossPattern;
    brushStyle["BDiag"] = Qt::BDiagPattern;
    brushStyle["FDiag"] = Qt::FDiagPattern;
    brushStyle["DiagCross"] = Qt::DiagCrossPattern;

    currentColor = QColor("blue");
    currentBrush.setColor(Qt::red);

    connect(ui->drawModeBtn, SIGNAL(clicked(bool)), this, SLOT(drawModeClicked()));
    connect(ui->clearShapes, SIGNAL(clicked(bool)), this, SLOT(clearShapes()));

    // New style signal slot
    connect(ui->colorCombo, &QComboBox::currentTextChanged, [=](QString color){
        currentColor = color; });

    connect(ui->brushCombo, &QComboBox::currentTextChanged, [=](QString brush){
        //currentBrush = brush;
        currentBrush.setStyle(brushStyle[brush]);});

    connect(ui->brushColorCombo, &QComboBox::currentTextChanged, [=](QString brushColor){
        currentBrushColor = brushColor;
        currentBrush.setColor(brushColor);
    });

    ui->colorCombo->setCurrentText("blue");
    ui->brushCombo->setCurrentText("No Brush");
    ui->brushColorCombo->setCurrentText("red");
}

void MainWindow::drawModeClicked()
{
    if( !drawMode ) {
        ui->drawModeBtn->setText("Disable Draw");
        drawMode = true;
    }
    else
    {
        ui->drawModeBtn->setText("Enable Draw");
        drawMode = false;

        // If there is shape avaialbe insert in the shape list
        if( m_clickPositions.length() > 0 ) {
            Shape shape(m_clickPositions, currentColor, currentBrush, currentBrushColor);

            // Beforing clearing, add the shape to vector
            m_shapes.append(shape);
            m_clickPositions.clear();
        }
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        statusBar()->showMessage(QString("Mouse move (%1,%2)").arg(mouseEvent->pos().x()).arg(mouseEvent->pos().y()));
    }
    return false;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

    if (event->button() == Qt::RightButton)
    {
        if( drawMode )
        {
           drawModeClicked();
        }
        statusBar()->showMessage(QString("Right click (%1,%2)").arg(mouseEvent->pos().x()).arg(mouseEvent->pos().y()));
    }
    else if(event->button() == Qt::LeftButton )
    {
        statusBar()->showMessage(QString("Left click (%1,%2)").arg(mouseEvent->pos().x()).arg(mouseEvent->pos().y()));

        // Add mouse click position to current shape
        if( drawMode )
        {
            m_clickPositions.append(QPointF(mouseEvent->pos().x(), mouseEvent->pos().y()));

            // call paintEvent immediately to draw the shapes
            update();
        }
    }
}

void MainWindow::clearShapes()
{
    m_shapes.clear();
    m_clickPositions.clear();
    update();
}

void MainWindow::drawShapes(const QList<QPointF> &pointList, QColor color,
                            QBrush brush, QPainter *painter)
{
    painter->setPen(color);
    painter->setBrush(brush);

    /// Depending on number of points in the shape either use drawPolygon
    /// or drawLine ordrawPoint call
    if( pointList.length() > 2 )
    {
        QPointF points[pointList.length()];
        for(int i = 0; i < pointList.length(); ++i)
            points[i] = pointList[i];

        painter->drawPolygon(points, pointList.length());
    }
    else
    {
        if( pointList.length() == 2)
            painter->drawLine(pointList[0], pointList[1]);
        else
            painter->drawPoint(pointList[0]);
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.save();

    // Draw previous shapes
    for(int shapeCount = 0; shapeCount < m_shapes.length(); ++shapeCount)
    {
        Shape shape = m_shapes[shapeCount];
        drawShapes(shape.m_points, shape.m_drawColor, shape.m_brush, &painter);
    }

    // Draw current shape
    QPen pen(currentColor);
    painter.setPen(pen);
    painter.setBrush(currentBrush);

    if( drawMode )
    {
        if(m_clickPositions.length() > 2 )
        {
            QPointF points[m_clickPositions.length()];
            for(int i = 0; i < m_clickPositions.length(); ++i)
                points[i] = m_clickPositions[i];

            painter.drawPolygon(points, m_clickPositions.length());
        }
        else
        {
            if(m_clickPositions.length() == 2)
                painter.drawLine(m_clickPositions[0], m_clickPositions[1]);
            else if(m_clickPositions.length() == 1)
                painter.drawPoint(m_clickPositions[0]);
        }
    }
    painter.restore();
}

MainWindow::~MainWindow()
{
    delete ui;
}
