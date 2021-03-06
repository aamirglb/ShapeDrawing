#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMouseEvent>
#include <QDebug>
#include <Shapes.h>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Draw Shapes");

    qApp->installEventFilter(this);
    drawMode = false;

    QStringList drawingColors;
    //drawingColors << QColor::colorNames();
    drawingColors << "acqua" <<  "blue" <<  "black" <<  "brown" <<  "coral" <<  "cyan"
                  <<  "gold" <<  "gray" <<  "green" <<  "lime" <<  "magenta" <<  "maroon"
                  <<  "navy" <<  "olive" <<  "orange" <<  "pink" <<  "red" <<  "white"
                  <<  "yellow";

    // Populate the colorToQColor map with different colors
    for(QString name : drawingColors)
        colorNamesMap[name] = QColor(name);

    QStringList brushStyleStr;
    brushStyleStr << "No Brush" << "Solid Pattern" << "Dense-1 Pattern" << "Dense-2"
               << "Dense-3" << "Dense-4" << "Dense-5" << "Dense-6" << "Dense-7"
               << "Horizontal" << "Vertical" << "Cross" << "BDiag" << "FDiag" << "DiagCross";

    // Populate Brush-Style to QString map
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

    // Populate the UI with different colors and brush style
    ui->colorCombo->insertItems(0, drawingColors);
    ui->brushColorCombo->insertItems(0, drawingColors);
    ui->brushCombo->insertItems(0, brushStyleStr);

    // Connect UI buttons to slots
    connect(ui->drawModeBtn, SIGNAL(clicked(bool)), this, SLOT(drawModeClicked()));
    connect(ui->clearShapes, SIGNAL(clicked(bool)), this, SLOT(clearShapes()));
    connect(ui->saveBtn, SIGNAL(clicked(bool)), this, SLOT(saveToXml()));
    connect(ui->loadBtn, SIGNAL(clicked(bool)), this, SLOT(loadFromXml()));

    // New style signal slot
    connect(ui->colorCombo, &QComboBox::currentTextChanged, [=](QString color){
        currentColor = color; });

    connect(ui->brushCombo, &QComboBox::currentTextChanged, [=](QString brush){
        currentBrush.setStyle(brushStyle[brush]);});

    connect(ui->brushColorCombo, &QComboBox::currentTextChanged, [=](QString brushColor){
        currentBrushColor = brushColor;
        currentBrush.setColor(brushColor);
    });

    ui->colorCombo->setCurrentText("blue");
    ui->brushCombo->setCurrentText("No Brush");
    ui->brushColorCombo->setCurrentText("red");
}

///
/// \brief MainWindow::drawModeClicked
///        Slot which gets fired when Draw Mode button is clicked on the GUI
///        Toggles Enable and Disable draw mode upon button click
///
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

///
/// \brief MainWindow::eventFilter
/// \param obj
/// \param event
/// \return
///
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

///
/// \brief MainWindow::mousePressEvent
/// \param event
///
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

///
/// \brief MainWindow::clearShapes
///        Clear the internal data structure and refresh the window
///        This has an effect of clearing all the drawings from the window
///
void MainWindow::clearShapes()
{
    m_shapes.clear();
    m_clickPositions.clear();
    update();
}

///
/// \brief MainWindow::saveToXml
///        Save the current drawing to an xml file.
///        The format of the xml file is as follows
///  <shapes>
///     <shape TotalPoints="3" DrawColor="gray" BrushStyle="Dense-7" BrushColor="green">
///         <point X="136" Y="139"/>
///         <point X="26" Y="349"/>
///         <point X="267" Y="356"/>
///     </shape>
///  </shapes>
///

/// TODO: Instead of hardcoding the filename, ask user for a filename
void MainWindow::saveToXml()
{
    QFile file("shapes.xml");
    if( !file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "File to open file";
        return;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement("shapes");

    // For each shape write TotalPoints, DrawColor, BrushStyle and BrushColor
    for(int shapeCount = 0; shapeCount < m_shapes.length(); ++shapeCount)
    {
        Shape shape = m_shapes[shapeCount];
        QString brushSty = brushStyle.key(shape.m_brush.style());
        QString brushColor = colorNamesMap.key(shape.m_brushColor);
        QString drawColor = colorNamesMap.key(shape.m_drawColor);
        //drawShapes(shape.m_points, shape.m_drawColor, shape.m_brush, &painter);

        writer.writeStartElement("shape");
        writer.writeAttribute("TotalPoints", QString::number(shape.m_points.length()));
        writer.writeAttribute("DrawColor", drawColor);
        writer.writeAttribute("BrushStyle", brushSty);
        writer.writeAttribute("BrushColor", brushColor);

        for(int i = 0; i < shape.m_points.length(); ++i)
        {
            writer.writeStartElement("point");
            writer.writeAttribute("X", QString::number(shape.m_points[i].x()));
            writer.writeAttribute("Y", QString::number(shape.m_points[i].y()));
            writer.writeEndElement();
        }
        writer.writeEndElement(); // shape
    }
    writer.writeEndDocument();
}

///
/// \brief MainWindow::loadFromXml
///        Load the shapes from the xml file
void MainWindow::loadFromXml()
{
    QFile file("shapes.xml");
    if( !file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "File to open file";
        return;
    }

    QXmlStreamReader reader(&file);

    while(!reader.atEnd() &&  !reader.hasError()) {
        /// Read next element
        QXmlStreamReader::TokenType token = reader.readNext();

        if(token == QXmlStreamReader::StartDocument) {
            continue;
        }

        /// If token is StartElement, we'll see if we can read it.
        if(token == QXmlStreamReader::StartElement)
        {
            /// If it is a shape
            if(reader.name() == "shape")
            {
                QString brushSty;
                QString brushColor;
                QString drawColor;
                int totalPoints = 0;
                QList<QPointF> points;

                /// Let's get the attributes for person */
                QXmlStreamAttributes attributes = reader.attributes();

                totalPoints = attributes.value("TotalPoints").toInt();
                drawColor = attributes.value("DrawColor").toString();
                brushSty = attributes.value("BrushStyle").toString();
                brushColor = attributes.value("BrushColor").toString();

                qDebug() << "Draw Color: " << drawColor;
                qDebug() << "Brush Color: " << brushColor;

                for(int i = 0; i < totalPoints; ++i)
                {
                    while(reader.readNextStartElement())
                    {
                        if(reader.name() == "point"){
                            QXmlStreamAttributes attributes = reader.attributes();
                            float x = attributes.value("X").toFloat();
                            float y = attributes.value("Y").toFloat();
                            points.append(QPointF(x, y));
                        }
                        else
                        {
                            reader.skipCurrentElement();
                            qDebug() << "Skipping element";
                        }
                    }
                } // end of for

                // insert the shape in the list
                Shape shape(points, colorNamesMap[drawColor], brushStyle[brushSty], colorNamesMap[brushColor]);
                m_shapes.append(shape);
            } // end of if(shape)
        }
    }

    // Update the screen
    update();
}

///
/// \brief MainWindow::drawShapes
///        Using QPainter draw shapes on the window
///
/// \param pointList
/// \param color
/// \param brush
/// \param painter
///
void MainWindow::drawShapes(const QList<QPointF> &pointList, QColor color,
                            QBrush brush, QColor brushColor, QPainter *painter)
{
    painter->setPen(color);
    brush.setColor(brushColor);
    painter->setBrush(brush);

    /// Depending on number of points in the shape either use drawPolygon
    /// or drawLine or drawPoint call
    if( pointList.length() > 2 )
    {
#ifdef __GNUC__
        // Under MSVC 2015, below code causes the error
        //  array type is not assignable
        QPointF points[pointList.length()];
#else
        QPointF *points = new QPointF[pointList.length()];
#endif
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

///
/// \brief MainWindow::paintEvent
///        Reimplement the paintEvent to draw the shapes
/// \param event
///
void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.save();

    // Draw previous shapes
    for(int shapeCount = 0; shapeCount < m_shapes.length(); ++shapeCount)
    {
        Shape shape = m_shapes[shapeCount];
        drawShapes(shape.m_points, shape.m_drawColor, shape.m_brush, shape.m_brushColor, &painter);
    }

    // Draw current shape
    QPen pen(currentColor);
    painter.setPen(pen);
    painter.setBrush(currentBrush);

    if( drawMode )
    {
        if(m_clickPositions.length() > 2 )
        {

#ifdef __GNUC__
            // Under MSVC 2015, below code causes the error
            //  array type is not assignable
            QPointF points[m_clickPositions.length()];
#else
            QPointF *points = new QPointF[m_clickPositions.length()];
#endif
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
