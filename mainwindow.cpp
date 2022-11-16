#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QImage>
#include <iostream>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintDevice>
#include <QPoint>
#include <QThread>


using std::swap;
using std::cin;
using std::cout;
using std::endl;


QImage img=QImage(700,700,QImage::Format_RGB888);
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->x_axis->hide();
    ui->y_axis->hide();
    connect(ui->frame,SIGNAL(Mouse_Pos()),this,SLOT(Mouse_Pressed()));
    connect(ui->frame, SIGNAL(Mouse_Rel()), this, SLOT(Mouse_Released()));
    connect(ui->frame,SIGNAL(sendMousePosition(QPoint&)),this,SLOT(showMousePosition(QPoint&)));

    timerActionPerformed = true;
    isGridDrawn = false;
    areAxesDrawn = false;
    isPenDown = false;
    isPenDownPrev = false;
    isClockAnimating = false;
    ns = 0;
    defaultAxesColor = qRgb(255, 182, 193);  // Pink
    defaultPenColor = qRgb(255, 0, 0);
    defaultLineColor = qRgb(255, 255, 0);
    defaultMidpointColor = qRgb(255, 0, 0);
    defaultBresenhamColor = qRgb(0, 255, 0);
    defaultEmptyColor = qRgb(0, 0, 0);
    defaultFillColor = qRgb(0, 255, 255);
    defaultBezierColor = qRgb(255, 0, 0);

    defaultCircleCenterColor = qRgb(200, 200, 200);

    cntEdge.resize(ui->frame->width());
    for(QVector<int> &v: cntEdge) v.resize(ui->frame->height());

    ui->comboBoxFillNeighbour->addItem("4 Neighbour");
    ui->comboBoxFillNeighbour->addItem("8 Neighbour");

    for(QVector<int> &v: cntEdge) for(int &e: v) e = 0;
    bezierInputState = drawPolygonInputPointState = scanlineInputPointState = false;

    C = QVector<QVector<int>>(1001, QVector<int>(1001, -1));  // dp
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::point(int x,int y)
{
    img.setPixel(x,y,qRgb(255,255,0));
    ui->frame->setPixmap(QPixmap::fromImage(img));
}


void MainWindow::showMousePosition(QPoint &pos)
{
    int userX = convertToUserX(pos.x()), userY = convertToUserY(pos.y());
    int k = ui->grid_size->value(), midPnt = (k != 0)? ((img.height() / k) / 2): 1;
    ui->mouse_movement->setText(" X : " + QString::number(pos.x()) + ", Y : " + QString::number(pos.y()) + "  |  " +
                                " userX : " + QString::number(userX - midPnt) + ", userY : " + QString::number(userY - midPnt));

    // if(isPenDown && ui->radioButtonUserPen->isChecked() && getUserPoint(userX, userY) != defaultPenColor) drawUserPoint(userX, userY, defaultPenColor);

    // Optimisation for fast mouse movement ...
    if(isPenDown && ui->radioButtonUserPen->isChecked())
    {
        if(isPenDownPrev) drawLineBresenham(penXPrev, penYPrev, userX, userY, defaultPenColor);
        else drawUserPoint(userX, userY, defaultPenColor);

        penXPrev = userX, penYPrev = userY;

        ui->frame->setPixmap(QPixmap::fromImage(img));
    }

    isPenDownPrev = isPenDown;
}

void MainWindow::Mouse_Pressed()
{
    if(isClockAnimating) return;

    if(bezierInputState)
    {
        int x, y;
        x = convertToUserX(ui->frame->x);
        y = convertToUserY(ui->frame->y);

        if(!bezierControlPoints.empty())
            drawLineDDA(bezierControlPoints.back().x(), bezierControlPoints.back().y(), x, y, false),
            drawUserPoint(bezierControlPoints.back().x(), bezierControlPoints.back().y(), defaultBresenhamColor);

        drawUserPoint(x, y, defaultBresenhamColor);

        ui->frame->setPixmap(QPixmap::fromImage(img));

        bezierControlPoints.push_back( { x, y } );

        return;
    }

    if(scanlineInputPointState)
    {
        ScanlinePts.push_back(QPoint(convertToUserX(ui->frame->x), convertToUserY(ui->frame->y)));
        drawUserPoint(convertToUserX(ui->frame->x), convertToUserY(ui->frame->y), defaultLineColor);
        if(ScanlinePts.count() > 1)
        {
            int x1 = ScanlinePts.back().x(),
                y1 = ScanlinePts.back().y(),
                x2 = ScanlinePts[ScanlinePts.size() - 2].x(),
                y2 = ScanlinePts[ScanlinePts.size() - 2].y();
            drawLineDDA(x1, y1, x2, y2);
        }
        ui->frame->setPixmap(QPixmap::fromImage(img));

        return;
    }
    else if(drawPolygonInputPointState)
    {
        polygonList.back().push_back(QPoint(convertToUserX(ui->frame->x), convertToUserY(ui->frame->y)));
        if(polygonList.back().size() > 1)
        {
            int x1 = polygonList.back().back().x(),
                y1 = polygonList.back().back().y(),
                x2 = polygonList.back()[polygonList.back().size() - 2].x(),
                y2 = polygonList.back()[polygonList.back().size() - 2].y();
            drawLineDDA(x1, y1, x2, y2);
        }

        ui->frame->setPixmap(QPixmap::fromImage(img));

        return;
    }

    isPenDown = true;

    int k = ui->grid_size->value(), midPnt = (k != 0)? ((img.height() / k) / 2): 1;
    ui->mouse_pressed->setText(" X : " + QString::number(ui->frame->x) + ", Y : " + QString::number(ui->frame->y) + "  |  " +
                               " userX : " + QString::number(convertToUserX(ui->frame->x) - midPnt) + ", userY : " + QString::number(convertToUserY(ui->frame->y) - midPnt));
    // ui->x_axis->move(0,ui->frame->y);
    // ui->y_axis->move(ui->frame->x,0);

    if(!isGridDrawn) return;

    if(ui->checkBoxDrawBoxPoint->isChecked())
    {
        // for testing ...
        drawUserPoint(convertToUserX(ui->frame->x), convertToUserY(ui->frame->y), defaultLineColor);
        return;
    }

    int x, y;
    x = convertToUserX(ui->frame->x);
    y = convertToUserY(ui->frame->y);

    timerActionPerformed = true;
    timer.start();

    if(ui->radioButtonFloodFill->isChecked()) floodFill(x, y, getUserPoint(x, y), defaultFillColor);
    else if(ui->radioButtonBoundaryFill->isChecked()) boundaryFill(x, y);
    else timerActionPerformed = false;

    if(timerActionPerformed)
    {
        ns = timer.nsecsElapsed();
        ui->time_elapsed->setText(QString::number(ns) + " ns");
    }




    ui->frame->setPixmap(QPixmap::fromImage(img));
    // point(ui->frame->x,ui->frame->y);
}

void MainWindow::Mouse_Released()
{
    if(scanlineInputPointState || drawPolygonInputPointState || isClockAnimating || bezierInputState) return;

    isPenDown = false;

    int k = ui->grid_size->value(), midPnt = (k != 0)? ((img.height() / k) / 2): 1;
    ui->mouse_released->setText(" X : " + QString::number(ui->frame->x) + ", Y : " + QString::number(ui->frame->y) + "  |  " +
                               " userX : " + QString::number(convertToUserX(ui->frame->relX) - midPnt) + ", userY : " + QString::number(convertToUserY(ui->frame->relY) - midPnt));

    if(!isGridDrawn || ui->grid_size->value() <= 1) return;

    int x1, y1, x2, y2, r, rx, ry;
    x1 = convertToUserX(ui->frame->x);
    y1 = convertToUserY(ui->frame->y);
    x2 = convertToUserX(ui->frame->relX);
    y2 = convertToUserY(ui->frame->relY);
    // r = (int)(sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) + 0.5);
    r = ui->spinBoxRadius->value();
    if(r == 0) r = (int)(sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) + 0.5);
    rx = ui->spinBoxRx->value(), ry = ui->spinBoxRy->value();
    if(!rx || !ry) rx = abs(x2 - x1) / 2, ry = abs(y2 - y1) / 2;


    timerActionPerformed = true;
    // timer.start();

    if(ui->radioButtonLineSimple->isChecked()) drawLineSimple(x1, y1, x2, y2);
    else if(ui->radioButtonLineDDA->isChecked()) drawLineDDA(x1, y1, x2, y2);
    else if(ui->radioButtonLineBresenham->isChecked()) drawLineBresenham(x1, y1, x2, y2, defaultLineColor);
    else if(ui->radioButtonCircleSimple->isChecked()) drawCircleSimple(x1, y1, r);
    else if(ui->radioButtonCircleMidpoint->isChecked()) drawCircleMidpoint(x1, y1, r);
    else if(ui->radioButtonCircleBresenham->isChecked()) drawCircleBresenham(x1, y1, r);
    else if(ui->radioButtonEllipseSimple->isChecked()) drawEllipseSimple((x1 + x2) / 2, (y1 + y2) / 2, rx/*abs(x2 - x1) / 2*/, ry/*abs(y2 - y1) / 2*/);
    else if(ui->radioButtonEllipseMidpoint->isChecked()) drawEllipseMidpoint((x1 + x2) / 2, (y1 + y2) / 2, rx/*abs(x2 - x1) / 2*/, ry/*abs(y2 - y1) / 2*/);
    else if(ui->radioButtonTranslate->isChecked()) translate(x2 - x1, y2 - y1, polygonList);
    else if(ui->radioButtonScale->isChecked()) scale(x1, y1, (float)ui->doubleSpinBoxScaleX->value(),
                                                             (float)ui->doubleSpinBoxScaleY->value(), polygonList);
    else if(ui->radioButtonShear->isChecked()) shear(x1, y1, (float)ui->doubleSpinBoxShearX->value(),
                                                             (float)ui->doubleSpinBoxShearY->value(), polygonList);
    else if(ui->radioButtonRotate->isChecked()) rotate(x1, y1, (float)ui->doubleSpinBoxRotationAngle->value(), polygonList);
    else if(ui->radioButtonReflect->isChecked()) reflect(x1, y1, x2, y2, polygonList);
    else if(ui->radioButtonClipLine->isChecked()) clipLineCohenSutherland(x1, x2, y1, y2);
    else if(ui->radioButtonLiangBarsky->isChecked()) clipLineLiangBarsky(x1, x2, y1, y2);
    else if(ui->radioButtonClipPolygon->isChecked()) clipPolygonSutherlandHodgman(x1, x2, y1, y2);
    else timerActionPerformed = false;

    if(timerActionPerformed)
    {
        // int ns = timer.nsecsElapsed();
        ui->time_elapsed->setText(QString::number(ns) + " ns");
    }

    ui->frame->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_show_axes_clicked()
{
    if(ui->show_axes->isChecked())
    {
        ui->x_axis->show();
        ui->y_axis->show();
    }
    else{
        ui->x_axis->hide();
        ui->y_axis->hide();
    }
}

void MainWindow::on_set_point1_clicked()
{
    if(ui->draw_line->isChecked()){
        p1.setX(ui->frame->x);
        p1.setY(ui->frame->y);
    }
}

void MainWindow::on_set_point2_clicked()
{
    if(ui->draw_line->isChecked()){
        p2.setX(ui->frame->x);
        p2.setY(ui->frame->y);
    }
}

void MainWindow::on_Draw_clicked()
{
    int r0=ui->circle_radius->value();
    QPainter painter(&img);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::red);
    if(ui->draw_circle->isChecked()){
        p1.setX(ui->frame->x);
        p1.setY(ui->frame->y);
        painter.setPen(pen);
        painter.drawEllipse(p1,r0,r0);
    }
    if(ui->draw_line->isChecked()){
        painter.setPen(Qt::red);
        painter.drawLine(p1,p2);
    }
    ui->frame->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::clearScreen(bool clearPolygonList, bool clearLineList)
{
    for(int j=0;j<img.height();j++)
    {
        for(int i=0;i<img.width();i++)
        {
            img.setPixel(i,j,qRgb(0,0,0));
        }
    }

    if(isGridDrawn) drawGrid(ui->grid_size->value());
    if(areAxesDrawn) drawAxes();
    if(clearPolygonList) polygonList.clear();
    if(clearLineList) lineList.clear();

    for(QVector<int> &v: cntEdge) for(int &e: v) e = 0;
    ui->frame->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_pushButton_clicked()
{
    clearScreen();
}

void MainWindow::on_show_grid_clicked()
{
    int k=ui->grid_size->value();
        // if(ui->show_grid->isChecked()){
        if(!isGridDrawn) drawGrid(k);
        else removeGrid(), areAxesDrawn = false, removeAxes();

        isGridDrawn = !isGridDrawn;
}

void MainWindow::drawGrid(int k)
{
    if(k == 0) return;

    int i, j;
    for(i=0;i<img.height();i++)
        for(j=0;j<img.width();j+=k)
            // if(img.pixel(i, j) == qRgb(0, 0, 0))
                img.setPixel(i,j,qRgb(255,255,255));

    for(i=0;i<img.height();i+=k)
        for(j=0;j<img.width();j++)
            // if(img.pixel(i, j) == qRgb(0, 0, 0))
                img.setPixel(i,j,qRgb(255,255,255));

    for(QVector<int> &v: cntEdge) for(int &e: v) e = 0;

    ui->frame->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::removeGrid()
{
    int i, j;

    for(i=0;i<img.width();i++)
        for(j=0;j<img.height();j++)
            img.setPixel(j,i,qRgb(0,0,0));

//    for(i=0;i<img.height();i++)
//        for(j=0;j<img.width();j++)
//            if(img.pixel(i, j) == qRgb(255, 255, 255))
//                img.setPixel(i,j,qRgb(0,0,0));

    ui->frame->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::delay(float scale)
{
    ui->frame->setPixmap(QPixmap::fromImage(img));
    ui->frame->repaint();
    Sleeper::msleep((int)(scale * 50));
}

void MainWindow::on_grid_size_valueChanged(int arg1)
{
    if(isGridDrawn)
    {
        removeGrid();
        drawGrid(ui->grid_size->value());

        if(areAxesDrawn) drawAxes();
    }
}

// if the mouse pointer is on a grid line, it selects the right/down box automatically

inline int MainWindow::convertToUserX(int x)
{
    int k = ui->grid_size->value();
    if(k) return x / k;
    else return -1;
}

inline int MainWindow::convertToUserY(int y)
{
    int k = ui->grid_size->value();
    if(k) return y / k;
    else return -1;
}

void MainWindow::drawUserPoint(int x, int y, QRgb color)
{
    int k = ui->grid_size->value();
    if(k <= 1 || x < 0 || y < 0) return;

    int userX = x, userY = y;
    int xStart = userX * k + 1, yStart = userY * k + 1;
    int xEnd = std::min((userX + 1) * k - 1, img.height() - 1), yEnd = std::min((userY + 1) * k - 1, img.width() - 1);
    for(int i = xStart; i <= xEnd; i++)
        for(int j = yStart; j <= yEnd; j++)
            img.setPixel(i, j, color);

    if(color == defaultLineColor) cntEdge[x][y]++;

    // ui->frame->setPixmap(QPixmap::fromImage(img));
}

QRgb MainWindow::getUserPoint(int x, int y)
{
    if(x < 0 || y < 0 || !isGridDrawn || ui->grid_size->value() <= 1) return defaultEmptyColor;

    int k = ui->grid_size->value(), xOrg = k * x + 1, yOrg = k * y + 1;
    if(xOrg > img.width() || yOrg > img.height()) return defaultEmptyColor;
    else return img.pixel(xOrg, yOrg);
}

void MainWindow::drawAxes()
{
    int k = ui->grid_size->value();
    int midPnt = (img.height() / k) / 2;

    for(int i=0;i<=img.height()/k;i++)
        drawUserPoint(i, midPnt, defaultAxesColor), drawUserPoint(midPnt, i, defaultAxesColor);

    ui->frame->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::removeAxes()
{
    int k = ui->grid_size->value();
    int midPnt = (img.height() / k) / 2;

    for(int i=0;i<img.height()/k;i++)
        drawUserPoint(i, midPnt, defaultEmptyColor), drawUserPoint(midPnt, i, defaultEmptyColor);

    ui->frame->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_pushButtonShowAxes_clicked()
{
    if(!isGridDrawn) return;

    if(areAxesDrawn)
    {
        // undraw axes
        removeAxes();
    }
    else
    {
        // draw axes
        drawAxes();
    }

    areAxesDrawn = !areAxesDrawn;
}


//  All drawing functions ......................................................................................................................


void MainWindow::drawLineSimple(int x1, int y1, int x2, int y2, bool includeToLineList)
{
    if(includeToLineList) lineList.push_back( { {x1, y1} , {x2, y2} } );

    timer.start();
    drawLineSimpleDebug(x1, y1, x2, y2);
    ns = timer.nsecsElapsed();

    if(x1 == x2)
    {
        // vertical line
        int start = std::min(y1, y2), end = std::max(y1, y2);
        for(int j = start; j <= end; j++)
            drawUserPoint(x1, j, defaultLineColor);
    }
    else
    {
        float m = ((float)(y1 - y2)) / (x1 - x2);
        int start = std::min(x1, x2), end = std::max(x1, x2), yStart = (start == x1)? y1: y2, j;
        for(int i = start; i <= end; i++)
        {
            j = (int)(yStart + m * (i - start) + 0.5);
            drawUserPoint(i, j, defaultLineColor);
        }
    }
}

void MainWindow::drawLineDDA(int x1, int y1, int x2, int y2, bool includeToLineList)
{
    if(includeToLineList) lineList.push_back( { {x1, y1} , {x2, y2} } );

    timer.start();
    drawLineDDADebug(x1, y1, x2, y2);
    ns = timer.nsecsElapsed();

    float dx, dy, y, x, Dx, Dy;

    // Base Case
    if(x1 == x2 && y1 == y2){ drawUserPoint(x1, y1, defaultLineColor); return; } // else it will cause division by 0

    dx = x2 - x1;  // total span in x
    dy = y2 - y1;  // total span in y
    x = x1;
    y = y1;

    // incremental steps in x and y
    if(abs(dx) > abs(dy))
    {
        Dx = (dx > 0)? 1: -1;
        Dy = dy / abs(dx);
    }
    else
    {
        Dx = dx / abs(dy);
        Dy = (dy > 0)? 1: -1;
    }

    // to get the number of iterations correctly
    if(dx < 0) dx = -dx;
    if(dy < 0) dy = -dy;

    int ix, iy, end = (dx > dy)? dx: dy;  // integer x and y co-ordinates
    for(int k = 0; k <= end; k++)
    {
        ix = (int)(x + 0.5), iy = (int)(y + 0.5);
        drawUserPoint(ix, iy, defaultLineColor);
        x += Dx, y += Dy;
    }
}

void MainWindow::drawLineBresenham(int x1, int y1, int x2, int y2, QRgb color, bool includeToLineList)
{
    if(includeToLineList) lineList.push_back( { {x1, y1} , {x2, y2} } );

    timer.start();
    drawLineBresenhamDebug(x1, y1, x2, y2, color);
    ns = timer.nsecsElapsed();

    // Case for horizontal line
    if(y1 == y2)
    {
        int start = std::min(x1, x2), end = std::max(x1, x2);
        for(int i = start; i <= end; i++) drawUserPoint(i, y1, color);
        return;
    }

    // Case for vertical line
    if(x1 == x2)
    {
        int start = std::min(y1, y2), end = std::max(y1, y2);
        for(int j = start; j <= end; j++) drawUserPoint(x1, j, color);
        return;
    }

    int dx = x2 - x1, dy = y2 - y1, p;

    // Case for |m| <= 1
    if(abs(dy) <= abs(dx))
    {
        if(x1 > x2) std::swap(x1, x2), std::swap(y1, y2), dx = -dx, dy = -dy;

        int j = (dy > 0)? y1: (y1 + 1);
        p = 2 * dy - dx;
        drawUserPoint(x1, y1, color);  // Plotting initial point ...
        for(int i = x1 + 1; i <= x2; i++)
        {
            if(dy > 0)
            {
                if(p < 0) p += 2 * dy;
                else p += 2 * dy - 2 * dx, j++;
            }
            else
            {
                if(p > 0) p += 2 * dy;
                else p += 2 * dy + 2 * dx, j--;
            }

            drawUserPoint(i, j, color);
        }
    }

    // Case for |m| > 1
    else
    {
        if(y1 > y2) std::swap(x1, x2), std::swap(y1, y2), dx = -dx, dy = -dy;

        int i = (dx > 0)? x1: (x1 + 1);
        p = 2 * dx - dy;
        drawUserPoint(x1, y1, color);  // Plotting initial point ...
        for(int j = y1 + 1; j <= y2; j++)
        {
            if(dx > 0)
            {
                if(p < 0) p += 2 * dx;
                else p += 2 * dx - 2 * dy, i++;
            }
            else
            {
                if(p > 0) p += 2 * dx;
                else p += 2 * dx + 2 * dy, i--;
            }

            drawUserPoint(i, j, color);
        }
    }
}




void MainWindow::drawCircleSimple(int xc, int yc, int r)
{
    timer.start();
    drawCircleSimpleDebug(xc, yc, r);
    ns = timer.nsecsElapsed();

    drawUserPoint(xc, yc, defaultCircleCenterColor);
    delay();

    // Polar
    if ( r <= 0 ) return;

    // pi / 4 = 0.79
    for (int i = 0; ((float)i)/r <= 0.82; i++)
    {
        float theta = ((float)i) / r;
        float x = xc + r * std::cos(theta);
        float y = yc + r * std::sin(theta);
        int ix = (x == 0)? 0: (int)(x + 0.5 * abs(x) / x);
        int iy = (y == 0)? 0: (int)(y + 0.5 * abs(y) / y);

        int xDiff = ix - xc, yDiff = iy - yc;
        // drawUserPoint(ix, iy, defaultLineColor);
        drawUserPoint(xc + xDiff, yc + yDiff, defaultLineColor);
        drawUserPoint(xc - xDiff, yc + yDiff, defaultLineColor);
        drawUserPoint(xc + xDiff, yc - yDiff, defaultLineColor);
        drawUserPoint(xc - xDiff, yc - yDiff, defaultLineColor);
        drawUserPoint(xc + yDiff, yc + xDiff, defaultLineColor);
        drawUserPoint(xc - yDiff, yc + xDiff, defaultLineColor);
        drawUserPoint(xc + yDiff, yc - xDiff, defaultLineColor);
        drawUserPoint(xc - yDiff, yc - xDiff, defaultLineColor);

        delay();
    }

    // Simple
    /* int j;
    for(int i = -r; i <= r; i++)
    {
        j = (int)(sqrt(r * r - i * i) + 0.5);
        drawUserPoint(xc + i, yc + j, defaultLineColor);
        drawUserPoint(xc + i, yc - j, defaultLineColor);

        delay();
    } */
}

void MainWindow::drawCircleMidpoint(int xc, int yc, int r)
{
    timer.start();
    drawCircleMidpointDebug(xc, yc, r);
    ns = timer.nsecsElapsed();

    drawUserPoint(xc, yc, defaultCircleCenterColor);
    if(ui->checkBoxDelay->isChecked()) delay();

    // 8 way symmetry
    int x, y, p;
    x = 0, y = r, p = 1 - r;

    while(x <= y)
    {
        drawUserPoint(xc + x, yc + y, defaultMidpointColor);
        drawUserPoint(xc + x, yc - y, defaultMidpointColor);
        drawUserPoint(xc - x, yc + y, defaultMidpointColor);
        drawUserPoint(xc - x, yc - y, defaultMidpointColor);

        drawUserPoint(xc + y, yc + x, defaultMidpointColor);
        drawUserPoint(xc + y, yc - x, defaultMidpointColor);
        drawUserPoint(xc - y, yc + x, defaultMidpointColor);
        drawUserPoint(xc - y, yc - x, defaultMidpointColor);

        x++;
        if(p < 0) p += 2 * x + 1;
        else y--, p += 2 * x + 1 - 2 * y;

        if(ui->checkBoxDelay->isChecked()) delay();
    }
}

void MainWindow::drawCircleBresenham(int xc, int yc, int r)
{
    timer.start();
    drawCircleBresenhamDebug(xc, yc, r);
    ns = timer.nsecsElapsed();

    drawUserPoint(xc, yc, defaultCircleCenterColor);
    delay();

    // 8 way symmetry (with updated parameters)
    int x, y, p;
    x = 0, y = r, p = 3 - 2 * r;

    while(x <= y)
    {
        drawUserPoint(xc + x, yc + y, defaultBresenhamColor);
        drawUserPoint(xc + x, yc - y, defaultBresenhamColor);
        drawUserPoint(xc - x, yc + y, defaultBresenhamColor);
        drawUserPoint(xc - x, yc - y, defaultBresenhamColor);

        drawUserPoint(xc + y, yc + x, defaultBresenhamColor);
        drawUserPoint(xc + y, yc - x, defaultBresenhamColor);
        drawUserPoint(xc - y, yc + x, defaultBresenhamColor);
        drawUserPoint(xc - y, yc - x, defaultBresenhamColor);

        x++;
        if(p < 0) p += 4 * x + 6;
        else y--, p += 4 * x + 10 - 4 * y;

        delay();
    }
}




void MainWindow::drawEllipseSimple(int xc, int yc, int rx, int ry)
{
    timer.start();
    drawEllipseSimpleDebug(xc, yc, rx, ry);
    ns = timer.nsecsElapsed();

    drawUserPoint(xc, yc, defaultCircleCenterColor);
    delay();

    // Polar
    int r_l = std::max(rx, ry);

    // pi / 2 = 1.57
    for (int i = 0; ((float)i)/r_l <= 1.62; i++)
    {
        float theta = ((float)i) / r_l;
        float x = xc + rx * std::cos(theta);
        float y = yc + ry * std::sin(theta);
        int ix = (x == 0)? 0: (int)(x + 0.5 * abs(x) / x);
        int iy = (y == 0)? 0: (int)(y + 0.5 * abs(y) / y);

        int xDiff = ix - xc, yDiff = iy - yc;
        // drawUserPoint(ix, iy, defaultLineColor);

        drawUserPoint(xc + xDiff, yc + yDiff, defaultLineColor);
        drawUserPoint(xc - xDiff, yc + yDiff, defaultLineColor);
        drawUserPoint(xc + xDiff, yc - yDiff, defaultLineColor);
        drawUserPoint(xc - xDiff, yc - yDiff, defaultLineColor);

        delay();
    }

    // Simple
    /* int j;
    for(int i = -rx; i <= rx; i++)
    {
        j = (int)(ry * sqrt(1 - ((double)i * i) / (rx * rx)) + 0.5);
        drawUserPoint(xc + i, yc + j, defaultLineColor);
        drawUserPoint(xc + i, yc - j, defaultLineColor);

        delay();
    } */
}

void MainWindow::drawEllipseMidpoint(int xc, int yc, int rx, int ry)
{
    timer.start();
    drawEllipseMidpointDebug(xc, yc, rx, ry);
    ns = timer.nsecsElapsed();

    drawUserPoint(xc, yc, defaultCircleCenterColor);
    delay();

    drawUserPoint(xc + rx, yc, defaultMidpointColor);
    drawUserPoint(xc, yc + ry, defaultMidpointColor);
    drawUserPoint(xc - rx, yc, defaultMidpointColor);
    drawUserPoint(xc, yc - ry, defaultMidpointColor);
    int x = 0;
    int y = ry;
    int p = 4 * ry * ry - 4 * rx * rx * ry + rx * rx;
    while (ry * ry * x < rx * rx * y) {
        if (p < 0) {
            p += 4 * ry * ry * (2 * x + 3);
            x += 1;
        } else {
            p += 4 * ry * ry * (2 * x + 3) + 8 * rx * rx * (1 - y);
            x += 1;
            y -= 1;
        }
        drawUserPoint(x + xc, y + yc, defaultMidpointColor);
        drawUserPoint(x + xc, -y + yc, defaultMidpointColor);
        drawUserPoint(-x + xc, y + yc, defaultMidpointColor);
        drawUserPoint(-x + xc, -y + yc, defaultMidpointColor);

        delay();
    }

    x = rx;
    y = 0;
    p = 4 * rx * rx - 4 * ry * ry * rx + ry * ry;
    while (ry * ry * x > rx * rx * y) {
        if (p < 0) {
            p += 4 * rx * rx * (2 * y + 3);
            y += 1;
        } else {
            p += 4 * rx * rx * (2 * y + 3) + 8 * ry * ry * (1 - x);
            y += 1;
            x -= 1;
        }
        drawUserPoint(x + xc, y + yc,  defaultMidpointColor);
        drawUserPoint(x + xc, -y + yc, defaultMidpointColor);
        drawUserPoint(-x + xc, y + yc, defaultMidpointColor);
        drawUserPoint(-x + xc, -y + yc, defaultMidpointColor);

        delay();
    }
}




void MainWindow::floodFill(int x, int y, QRgb initialColor, QRgb finalColor)
{
    // 4 neighbour method ...
    int k = ui->grid_size->value();
    if(x < 0 || y < 0 || x >= img.width() / k || y >= img.height() / k || getUserPoint(x, y) != initialColor) return;

    drawUserPoint(x, y, finalColor);
    if(ui->checkBoxDelay->isChecked()){ delay(0.1); ui->frame->setPixmap(QPixmap::fromImage(img)); }
    floodFill(x + 1, y, initialColor, finalColor);
    floodFill(x - 1, y, initialColor, finalColor);
    floodFill(x, y + 1, initialColor, finalColor);
    floodFill(x, y - 1, initialColor, finalColor);

    if(ui->comboBoxFillNeighbour->currentText() == "8 Neighbour")
    {
        floodFill(x + 1, y + 1, initialColor, finalColor);
        floodFill(x - 1, y + 1, initialColor, finalColor);
        floodFill(x + 1, y - 1, initialColor, finalColor);
        floodFill(x - 1, y - 1, initialColor, finalColor);
    }
}

void MainWindow::boundaryFill(int x, int y)
{
    // 4 neighbour method ...
    // std::cout<<x<<" "<<y<<std::endl;
    int k = ui->grid_size->value();
    if(x < 0 || y < 0 || x >= img.width() / k || y >= img.height() / k || getUserPoint(x, y) == defaultLineColor || getUserPoint(x, y) == defaultFillColor) return;

    drawUserPoint(x, y, defaultFillColor);
    // drawUserPoint(x, y, defaultLineColor);
    if(ui->checkBoxDelay->isChecked()){ delay(0.1); ui->frame->setPixmap(QPixmap::fromImage(img)); }

    boundaryFill(x + 1, y);
    boundaryFill(x - 1, y);
    boundaryFill(x, y + 1);
    boundaryFill(x, y - 1);

    if(ui->comboBoxFillNeighbour->currentText() == "8 Neighbour")
    {
        boundaryFill(x + 1, y + 1);
        boundaryFill(x - 1, y + 1);
        boundaryFill(x + 1, y - 1);
        boundaryFill(x - 1, y - 1);
    }
}

void MainWindow::fillScan(int scan, Edge *active)
{
    Edge *p1 = active->next, *p2;
    int i;
    while(p1)
    {
        p2 = p1->next;
        for(i = p1->xIntersect; i < p2->xIntersect; i++)
        {
            if(getUserPoint(i, scan) != defaultLineColor)
                drawUserPoint(i, scan, defaultFillColor);
            if(ui->checkBoxDelay->isChecked()) delay(0.1);
            //, ui->frame->setPixmap(QPixmap::fromImage(img));
        }
        p1 = p2->next;
    }
}

void MainWindow::scanlineFill()
{
    int cnt = ScanlinePts.count(), k = ui->grid_size->value(), yMax = (img.height() - 1) / k;
    Edge *edges[yMax + 1];

    for(int i = 0; i <= yMax; i++)
    {
        edges[i] = new Edge();
        edges[i]->next = nullptr;
    }

    sf.buildEdgeList(ScanlinePts, edges);
    Edge *active = new Edge();
    active->next = nullptr;

    for(int scan = 0; scan <= yMax; scan++)
    {
        sf.buildActiveList(scan, active, edges);
        if(active->next)
        {
            fillScan(scan, active);
            sf.updateActiveList(scan, active);
            sf.resortActiveList(active);
        }
    }

    // My Testings ...
    /* int k = ui->grid_size->value(), xMax = (ui->frame->width() - 1) / k, yMax = (ui->frame->height() - 1) / k;

    bool doFill;
    for(int y = 0; y <= yMax; y++)
    {
        // Every horizontal scanline is y-indexed
        doFill = false;

        for(int x = 0; x <= xMax; x++)
        {
            if(getUserPoint(x, y) == defaultLineColor)
            {
                int xEnd = x;
                while((xEnd + 1) <= xMax && getUserPoint(xEnd + 1, y) == defaultLineColor) xEnd++;

                if(x == xEnd)
                {
                    if(cntEdge[x][y] % 2) doFill = !doFill;
                }
                else
                {
//                     if(y != 0 && y != yMax)
//                    {
//                        // if x are on extremes -> only possibility is same side lines -> no change
//                        bool leftPntLineUp, rightPntLineUp;
//                        leftPntLineUp = (x < xMax && getUserPoint(x + 1, y + 1) == defaultLineColor) ||
//                                        getUserPoint(x, y + 1) == defaultLineColor ||
//                                        (x > 0 && getUserPoint(x - 1, y + 1) == defaultLineColor);
//                        rightPntLineUp = (xEnd < xMax && getUserPoint(xEnd + 1, y + 1) == defaultLineColor) ||
//                                getUserPoint(xEnd, y + 1) == defaultLineColor ||
//                                (xEnd > 0 && getUserPoint(xEnd - 1, y + 1) == defaultLineColor);

//                        if(leftPntLineUp ^ rightPntLineUp) doFill = !doFill;
//                    }
                    if(cntEdge[x][y] < 2 && cntEdge[xEnd][y] < 2) doFill = !doFill;

                    x = xEnd;
                }
            }
            else
            {
                if(doFill) drawUserPoint(x, y, defaultFillColor);
            }
        }
    } */
    /* for(int y = 0; y <= yMax; y++)
    {
        // Every horizontal scanline is y-indexed
        doFill = false;

        for(int x = 0; x <= xMax; x++)
        {
            if(getUserPoint(x, y) == defaultLineColor)
            {
                int xEnd = x;
                while((xEnd + 1) <= xMax && getUserPoint(xEnd + 1, y) == defaultLineColor) xEnd++;

                if(x == xEnd)
                {
                    if(y != 0 && y != yMax)
                    {
                        // if x are on extremes -> only possibility is same side lines -> no change
                        int numUp = 0, numDown = 0;
                        // Remember, operator ? has lower precedance than +, + will be evaluated first -> so ()
                        numUp = ((x < xMax && getUserPoint(x + 1, y + 1) == defaultLineColor)? 1: 0) +
                                ((getUserPoint(x, y + 1) == defaultLineColor)? 1: 0) +
                                ((x > 0 && getUserPoint(x - 1, y + 1) == defaultLineColor)? 1: 0);
                        numDown = ((x < xMax && getUserPoint(x + 1, y - 1) == defaultLineColor)? 1: 0) +
                                ((getUserPoint(x, y - 1) == defaultLineColor)? 1: 0) +
                                ((x > 0 && getUserPoint(x - 1, y - 1) == defaultLineColor)? 1: 0);

                        if(numUp < 2 && numDown < 2 && numUp && numDown) doFill = !doFill;
                    }
                }
                else
                {
                    if(y != 0 && y != yMax)
                    {
                        // if x are on extremes -> only possibility is same side lines -> no change
                        bool leftPntLineUp, rightPntLineUp;
                        leftPntLineUp = (x < xMax && getUserPoint(x + 1, y + 1) == defaultLineColor) ||
                                        getUserPoint(x, y + 1) == defaultLineColor ||
                                        (x > 0 && getUserPoint(x - 1, y + 1) == defaultLineColor);
                        rightPntLineUp = (xEnd < xMax && getUserPoint(xEnd + 1, y + 1) == defaultLineColor) ||
                                getUserPoint(xEnd, y + 1) == defaultLineColor ||
                                (xEnd > 0 && getUserPoint(xEnd - 1, y + 1) == defaultLineColor);

                        if(leftPntLineUp ^ rightPntLineUp) doFill = !doFill;
                    }

                    x = xEnd;
                }
            }
            else
            {
                if(doFill) drawUserPoint(x, y, defaultFillColor);
            }
        }
    } */
}




void MainWindow::drawAllCurrentPolygons()
{
    for(QVector<QPoint> &polygon: polygonList)
    {
        int n = polygon.size();
        for(int i = 0; i < n; i++) drawLineDDA(polygon[i].x(), polygon[i].y(),
                                               polygon[(i + 1) % n].x(), polygon[(i + 1) % n].y());
    }

    ui->frame->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::translate(int tx, int ty, QVector<QVector<QPoint>> &polygonList, bool updateAndShow)
{
    Matrix mTr;
    mTr.setTranslationMatrix(tx, ty);

//    for(QVector<QPoint> &polygon: polygonList){ for(QPoint &vertex: polygon) std::cout<<vertex.x()<<","<<vertex.y()<<"\t"; std::cout<<std::endl; } std::cout<<std::endl;

    for(QVector<QPoint> &polygon: polygonList)
        for(QPoint &vertex: polygon)
            vertex = Matrix::multiply(mTr, vertex);

//    for(QVector<QPoint> &polygon: polygonList){ for(QPoint &vertex: polygon) std::cout<<vertex.x()<<","<<vertex.y()<<"\t"; std::cout<<std::endl; } std::cout<<std::endl;

    if(updateAndShow)
    {
        clearScreen(false);
        drawAllCurrentPolygons();
    }
}

void MainWindow::scale(int pivotX, int pivotY, float scaleX, float scaleY, QVector<QVector<QPoint>> &polygonList, bool updateAndShow)
{
    Matrix mTr, mTrRev, mSc;
    mTr.setTranslationMatrix(-pivotX, -pivotY);
    mTrRev.setTranslationMatrix(pivotX, pivotY);
    mSc.setScaleMatrix(scaleX, scaleY);

    for(QVector<QPoint> &polygon: polygonList)
        for(QPoint &vertex: polygon)
//            std::cout<<vertex.x()<<","<<vertex.y()<<"\t",
            vertex = Matrix::multiply(mTr, vertex),
            vertex = Matrix::multiply(mSc, vertex),
            vertex = Matrix::multiply(mTrRev, vertex)
//          , std::cout<<" --> "<<vertex.x()<<","<<vertex.y()<<"\n"
            ;

    if(updateAndShow)
    {
        clearScreen(false);
        drawAllCurrentPolygons();
    }
}

void MainWindow::shear(int pivotX, int pivotY, float shearX, float shearY, QVector<QVector<QPoint>> &polygonList, bool updateAndShow)
{
    Matrix mTr, mTrRev, mSh;
    mTr.setTranslationMatrix(-pivotX, -pivotY);
    mTrRev.setTranslationMatrix(pivotX, pivotY);
    mSh.setShearMatrix(shearX, shearY);

    for(QVector<QPoint> &polygon: polygonList)
        for(QPoint &vertex: polygon)
//            std::cout<<vertex.x()<<","<<vertex.y()<<"\t",
            vertex = Matrix::multiply(mTr, vertex),
            vertex = Matrix::multiply(mSh, vertex),
            vertex = Matrix::multiply(mTrRev, vertex)
//          , std::cout<<" --> "<<vertex.x()<<","<<vertex.y()<<"\n"
            ;

    if(updateAndShow)
    {
        clearScreen(false);
        drawAllCurrentPolygons();
    }
}

void MainWindow::rotate(int pivotX, int pivotY, float angle, QVector<QVector<QPoint>> &polygonList, bool updateAndShow)
{
    // here angle is degrees -> convert it to radians ...
    angle = angle * M_PI / 180;

    Matrix mTr, mTrRev, mRot;
    mTr.setTranslationMatrix(-pivotX, -pivotY);
    mTrRev.setTranslationMatrix(pivotX, pivotY);
    mRot.setRotationMatrix(angle);

    for(QVector<QPoint> &polygon: polygonList)
        for(QPoint &vertex: polygon)
//            std::cout<<vertex.x()<<","<<vertex.y()<<"\t",
            vertex = Matrix::multiply(mTr, vertex),
            vertex = Matrix::multiply(mRot, vertex),
            vertex = Matrix::multiply(mTrRev, vertex)
//          , std::cout<<" --> "<<vertex.x()<<","<<vertex.y()<<"\n"
            ;

    if(updateAndShow)
    {
        clearScreen(false);
        drawAllCurrentPolygons();
    }
}

void MainWindow::reflect(int x1, int y1, int x2, int y2, QVector<QVector<QPoint>> &polygonList, bool updateAndShow)
{
    // reflect about (unbounded) line (x1, y1) -> (x2, y2)
    if(x1 == x2)
    {
        // Case for vertical line ...
        Matrix mTr;
        for(QVector<QPoint> &polygon: polygonList)
            for(QPoint &vertex: polygon)
                mTr.setTranslationMatrix(- 2 * (vertex.x() - x1), 0),
                vertex = Matrix::multiply(mTr, vertex);

        if(updateAndShow)
        {
            clearScreen(false);
            drawLineBresenham(x1, y1, x2, y2, defaultFillColor, false);
            drawAllCurrentPolygons();
        }

        return;
    }

    // Reflecting Line Constants
    float m, b, angleRad;
    m = ((float)(y2 - y1)) / (x2 - x1);
    b = y2 - m * x2;
    angleRad = (float)atan(m);

    Matrix mTr, mRot, mReflX, mRotRev, mTrRev, vMat;
    mTr.setTranslationMatrix(0, -b);
    mRot.setRotationMatrix(-angleRad);
    mReflX.setReflectionMatrixXAxis();
    mRotRev.setRotationMatrix(angleRad);
    mTrRev.setTranslationMatrix(0, b);



    for(QVector<QPoint> &polygon: polygonList)
        for(QPoint &vertex: polygon)
//            std::cout<<vertex.x()<<","<<vertex.y()<<"\t",
            vMat = vertex,
            vMat = Matrix::multiply(mTr, vMat),
            vMat = Matrix::multiply(mRot, vMat),
            vMat = Matrix::multiply(mReflX, vMat),
            vMat = Matrix::multiply(mRotRev, vMat),
            vMat = Matrix::multiply(mTrRev, vMat),
            vertex = vMat
//          , std::cout<<" --> "<<vertex.x()<<","<<vertex.y()<<"\n"
            ;

    if(updateAndShow)
    {
        clearScreen(false);
        drawLineBresenham(x1, y1, x2, y2, defaultFillColor, false);
        drawAllCurrentPolygons();
    }
}




// Function to get region code
Region MainWindow::getRegionCode(QPoint pnt, int xLeft, int xRight, int yDown, int yUp)
{
    Region r = INSIDE;

    // Even on the clipping boundary means outside
    if(pnt.x() <= xLeft) r |= LEFT;
    else if(pnt.x() >= xRight) r |= RIGHT;

    if(pnt.y() <= yDown) r |= DOWN;
    else if(pnt.y() >= yUp) r |= UP;

    return r;
}

void MainWindow::drawAllCurrentLines()
{
    for(QPair<QPoint, QPoint> line: lineList)
        drawLineDDA(line.first.x(), line.first.y(), line.second.x(), line.second.y(), false);
}

bool MainWindow::clipTestLiangBarsky(float p, float q, float *t1, float *t2)
{
    float r;
    bool retVal = true;

    if(p < 0.0)
    {
        // line entry point ...
        r = q / p ;

        // line portion is outside the clipping edge
        if (r > *t2) retVal = false;
        else if (r > *t1) *t1 = r;
    }
    else if(p > 0.0)
    {
        // line leaving point ...
        r = q / p ;

        // line portion is outside
        if (r < *t1) retVal = false;
        else if (r < *t2) *t2 = r;
    }
    else
    {
        if(q < 0.0) retVal = false;
    }

    return retVal;
}

void MainWindow::clipLineCohenSutherland(int xLeft, int xRight, int yDown, int yUp)
{
    timer.start();

    if(xLeft > xRight) swap(xLeft, xRight);
    if(yDown > yUp) swap(yDown, yUp);

    if(xRight - xLeft < 2 || yUp - yDown < 2)
    {
        // Base Case of nothing clipped ...
        clearScreen(true, true);  // Clear lineList too ...
        return;
    }

    int x1 = xLeft + 1,
        x2 = xRight - 1,
        y1 = yDown + 1,
        y2 = yUp - 1;

//    cout << x1 << " " << y1 << " " <<  x2 << " " << y2 << endl;

    QVector<QPair<QPoint, QPoint>> newLineList;
    QPoint pnt1, pnt2;
    Region r1, r2;
    for(QPair<QPoint, QPoint> line: lineList)
    {
        pnt1 = line.first, pnt2 = line.second;
        r1 = getRegionCode(pnt1, xLeft, xRight, yDown, yUp), r2 = getRegionCode(pnt2, xLeft, xRight, yDown, yUp);
        if(r1 != INSIDE && r2 == INSIDE) swap(pnt1, pnt2), swap(r1, r2);

//        cout << endl << "(" << pnt1.x() << "," << pnt1.y() << ") , (" << pnt2.x() << "," << pnt2.y() << ")  -->  ";

        if(r1 == INSIDE && r2 == INSIDE)
        {
            // Both points inside -> no need of clipping ...
            newLineList.push_back({pnt1, pnt2});
//            cout << "(" << newLineList.back().first.x() << "," << newLineList.back().first.y() << ") , ("
//                        << newLineList.back().second.x() << "," << newLineList.back().second.y() << ") => CASE 1";
        }
        else if(r1 == INSIDE && r2 != INSIDE)
        {
            // pnt1 is inside and pnt2 is outside leading to a guaranteed clipping ...

            // Base Case of vertical line ...
            if(pnt1.x() == pnt2.x())
            {
                if(r2 & UP) newLineList.push_back({pnt1, {pnt1.x(), y2}});
                else newLineList.push_back({pnt1, {pnt1.x(), y1}});

//                cout << "(" << newLineList.back().first.x() << "," << newLineList.back().first.y() << ") , ("
//                            << newLineList.back().second.x() << "," << newLineList.back().second.y() << ") => CASE 2 (VERT)";

                continue;
            }

            int x, y;
            float m = ((float)(pnt2.y() - pnt1.y())) / (pnt2.x() - pnt1.x()), b = pnt1.y() - m * pnt1.x();

            // Bitwise AND helps to locate region in which point is located to reduce intersection calculations ...
            if(r2 & UP)
            {
                y = y2;
                x = (int)((y - b) / m + 0.5);

                if(xLeft < x && x < xRight)
                {
                    newLineList.push_back({pnt1, {x, y}});
//                    cout << "(" << newLineList.back().first.x() << "," << newLineList.back().first.y() << ") , ("
//                                << newLineList.back().second.x() << "," << newLineList.back().second.y() << ") => CASE 2 (UP)";
                    continue;
                }
            }
            if(r2 & DOWN)
            {
                y = y1;
                x = (int)((y - b) / m + 0.5);

                if(xLeft < x && x < xRight)
                {
                    newLineList.push_back({pnt1, {x, y}});
//                    cout << "(" << newLineList.back().first.x() << "," << newLineList.back().first.y() << ") , ("
//                                << newLineList.back().second.x() << "," << newLineList.back().second.y() << ") => CASE 2 (DOWN)";
                    continue;
                }
            }
            if(r2 & RIGHT)
            {
                x = x2;
                y = (int)(m * x + b + 0.5);

                if(yDown < y && y < yUp)
                {
                    newLineList.push_back({pnt1, {x, y}});
//                    cout << "(" << newLineList.back().first.x() << "," << newLineList.back().first.y() << ") , ("
//                                << newLineList.back().second.x() << "," << newLineList.back().second.y() << ") => CASE 2 (RIGHT)";
                    continue;
                }
            }
            if(r2 & LEFT)
            {
                x = x1;
                y = (int)(m * x + b + 0.5);

                if(yDown < y && y < yUp)
                {
                    newLineList.push_back({pnt1, {x, y}});
//                    cout << "(" << newLineList.back().first.x() << "," << newLineList.back().first.y() << ") , ("
//                                << newLineList.back().second.x() << "," << newLineList.back().second.y() << ") => CASE 2 (LEFT)";
                    continue;
                }
            }
        }
        else
        {
            // Both points outside , can be clipped or totally removed ...

            // A common direction is found in bitwise AND -> line can be totally removed ...
            if(r1 & r2) continue;

            if(pnt1.x() == pnt2.x())
            {
                // Base case of vertical line
                // Since no same region case is checked , this case is bound to be clipped ...
                newLineList.push_back({{pnt1.x(), y1}, {pnt1.x(), y2}});
//                cout << "(" << newLineList.back().first.x() << "," << newLineList.back().first.y() << ") , ("
//                            << newLineList.back().second.x() << "," << newLineList.back().second.y() << ") => CASE 3 (VERT)";

                continue;
            }

            int x, y;
            QPoint p1, p2;
            float m = ((float)(pnt2.y() - pnt1.y())) / (pnt2.x() - pnt1.x()), b = pnt1.y() - m * pnt1.x();
//            cout << "m: " << m << " , b: " << b << endl;


            bool p1Found = false, p2Found = false;

            // PNT1 -> Bitwise AND helps to locate region in which point is located to reduce intersection calculations ...
            if((r1 & UP) && !p1Found)
            {
                cout << "[lol1-UP]";
                y = y2;
                x = (int)((y - b) / m + 0.5);

                if(xLeft < x && x < xRight)
                {
                    p1 = {x, y};
                    p1Found = true;
                }
            }
            if((r1 & DOWN) && !p1Found)
            {
                cout << "[lol1-DOWN]";
                y = y1;
                x = (int)((y - b) / m + 0.5);

                if(xLeft < x && x < xRight)
                {
                    p1 = {x, y};
                    p1Found = true;
                }
            }
            if((r1 & RIGHT) && !p1Found)
            {
                cout << "[lol1-RIGHT]";
                x = x2;
                y = (int)(m * x + b + 0.5);

                if(yDown < y && y < yUp)
                {
                    p1 = {x, y};
                    p1Found = true;
                }
            }
            if((r1 & LEFT) && !p1Found)
            {
                cout << "[lol1-LEFT]";
                x = x1;
                y = (int)(m * x + b + 0.5);

                if(yDown < y && y < yUp)
                {
                    p1 = {x, y};
                    p1Found = true;
                }
            }

            if(!p1Found) continue;  // No intersection found -> Line not added ...

            // PNT2 -> Bitwise AND helps to locate region in which point is located to reduce intersection calculations ...
            if((r2 & UP) && !p2Found)
            {
                cout << "[lol2-UP]";
                y = y2;
                x = (int)((y - b) / m + 0.5);

                if(xLeft < x && x < xRight)
                {
                    p2 = {x, y};
                    p2Found = true;
                }
            }
            if((r2 & DOWN) && !p2Found)
            {
                cout << "[lol2-DOWN]";
                y = y1;
                x = (int)((y - b) / m + 0.5);

                if(xLeft < x && x < xRight)
                {
                    p2 = {x, y};
                    p2Found = true;
                }
            }
            if((r2 & RIGHT) && !p2Found)
            {
                cout << "[lol2-RIGHT]";
                x = x2;
                y = (int)(m * x + b + 0.5);

                if(yDown < y && y < yUp)
                {
                    p2 = {x, y};
                    p2Found = true;
                }
            }
            if((r2 & LEFT) && !p2Found)
            {
                cout << "[lol2-LEFT]";
                x = x1;
                y = (int)(m * x + b + 0.5);

                if(yDown < y && y < yUp)
                {
                    p2 = {x, y};
                    p2Found = true;
                }
            }

            if(!p2Found) continue;  // No intersection found -> Line not added ...




            // Inserting into list ...
            newLineList.push_back({p1, p2});
//            cout << "(" << newLineList.back().first.x() << "," << newLineList.back().first.y() << ") , ("
//                        << newLineList.back().second.x() << "," << newLineList.back().second.y() << ") => CASE 3";
        }
    }

    ns = timer.nsecsElapsed();

    lineList = newLineList;
    clearScreen(true, false);
    drawAllCurrentLines();

//    cout << endl;
//    for(QPair<QPoint, QPoint> line: lineList)
//        cout << "(" << line.first.x() << "," << line.first.y() << ") , (" << line.second.x() << "," << line.second.y() << ")  |  ";
//    cout << "Size: " << lineList.size() << endl << endl;


    // Drawing lines for Clipping Boundary
    drawLineBresenham(xLeft, yUp, xRight, yUp, defaultPenColor, false);
    drawLineBresenham(xLeft, yDown, xRight, yDown, defaultPenColor, false);
    drawLineBresenham(xLeft, yDown, xLeft, yUp, defaultPenColor, false);
    drawLineBresenham(xRight, yDown, xRight, yUp, defaultPenColor, false);


//    for(int x: {xLeft, xRight})
//        for(int y: {yDown, yUp})

//    for(QPair<int, int> xPair: QVector<QPair<int, int>>({{xLeft, xRight}, {xRight, xLeft}}))
//        for(QPair<int, int> yPair: QVector<QPair<int, int>>{{yDown, yUp}, {yUp, yDown}})
//            drawLineBresenham(xPair.first, yPair.first, xPair.second, yPair.second, defaultPenColor);
}

void MainWindow::clipLineLiangBarsky(int xLeft, int xRight, int yDown, int yUp)
{
    timer.start();

    if(xLeft > xRight) swap(xLeft, xRight);
    if(yDown > yUp) swap(yDown, yUp);

    if(xRight - xLeft < 2 || yUp - yDown < 2)
    {
        // Base Case of nothing clipped ...
        clearScreen(true, true);  // Clear lineList too ...
        return;
    }

    int x1 = xLeft + 1,
        x2 = xRight - 1,
        y1 = yDown + 1,
        y2 = yUp - 1;

    QVector<QPair<QPoint, QPoint>> newLineList;
    QPoint pnt1, pnt2;
    int dx, dy;
    float t1, t2;

    for(QPair<QPoint, QPoint> line: lineList)
    {
        pnt1 = line.first, pnt2 = line.second;
        dx = pnt2.x() - pnt1.x(), dy = pnt2.y() - pnt1.y();
        t1 = 0.0, t2 = 1.0;

        // inside test wrt left edge
        if(clipTestLiangBarsky(-dx, pnt1.x() - x1, &t1, &t2))
        {
            // inside test wrt right edge
            if(clipTestLiangBarsky(dx, x2 - pnt1.x(), &t1, &t2))
            {
                // inside test wrt bottom edge
                if(clipTestLiangBarsky(-dy, pnt1.y() - y1, &t1, &t2))
                {
                    // inside test wrt top edge
                    if(clipTestLiangBarsky(dy, y2 - pnt1.y(), &t1, &t2))
                    {
                        float p2x = pnt2.x(), p2y = pnt2.y(), p1x = pnt1.x(), p1y = pnt1.y();
                        if(t2 < 1.0)
                        {
                            p2x = pnt1.x() + t2 * dx;
                            p2y = pnt1.y() + t2 * dy;
                        }
                        if(t1 > 0.0)
                        {
                            p1x = pnt1.x() + t1 * dx;
                            p1y = pnt1.y() + t1 * dy;
                        }

                        newLineList.push_back( { { (int)(p1x + 0.5) , (int)(p1y + 0.5) } ,
                                                 { (int)(p2x + 0.5) , (int)(p2y + 0.5) } } );
                    }
                }
            }
        }
    }

    ns = timer.nsecsElapsed();

    lineList = newLineList;
    clearScreen(true, false);
    drawAllCurrentLines();

    // Drawing lines for Clipping Boundary
    drawLineBresenham(xLeft, yUp, xRight, yUp, defaultPenColor, false);
    drawLineBresenham(xLeft, yDown, xRight, yDown, defaultPenColor, false);
    drawLineBresenham(xLeft, yDown, xLeft, yUp, defaultPenColor, false);
    drawLineBresenham(xRight, yDown, xRight, yUp, defaultPenColor, false);
}

void MainWindow::clipPolygonSutherlandHodgman(int xLeft, int xRight, int yDown, int yUp)
{
    if(xLeft > xRight) swap(xLeft, xRight);
    if(yDown > yUp) swap(yDown, yUp);

    if(xRight - xLeft < 2 || yUp - yDown < 2)
    {
        // Base Case of nothing clipped ...
        clearScreen(true, true);  // Clear polygonList too ...
        return;
    }

    int x1 = xLeft + 1,
        x2 = xRight - 1,
        y1 = yDown + 1,
        y2 = yUp - 1;

    QVector<QVector<QPoint>> newPolygonList;
    QPoint pnt1, pnt2, pntI;
    bool isPnt1Inside, isPnt2Inside;

    for(QVector<QPoint> &polygon: polygonList)
    {
        newPolygonList.push_back(QVector<QPoint>());

        for(Region direction: {UP, DOWN, LEFT, RIGHT})
        {
//            cout << ((int)direction) << endl;
            if(direction != UP) polygon = newPolygonList.back();  // Take previous result , if not 1st iteration ...
            newPolygonList.back().clear();

            // Base Case ...
            if(polygon.empty()) break;

            for(int index = 0; index < polygon.size(); index++)
            {
                pnt1 = polygon[index], pnt2 = polygon[(index + 1) % polygon.size()];

                switch(direction)
                {
                    case UP:    isPnt1Inside = (pnt1.y() < yUp); isPnt2Inside = (pnt2.y() < yUp); break;
                    case DOWN:  isPnt1Inside = (pnt1.y() > yDown); isPnt2Inside = (pnt2.y() > yDown); break;
                    case RIGHT: isPnt1Inside = (pnt1.x() < xRight); isPnt2Inside = (pnt2.x() < xRight); break;
                    case LEFT:  isPnt1Inside = (pnt1.x() > xLeft); isPnt2Inside = (pnt2.x() > xLeft); break;
                }

                if(isPnt1Inside) newPolygonList.back().push_back(pnt1);

                if(isPnt1Inside != isPnt2Inside)
                {
                    // Case when one inside one outside (!= is logical XOR , ^ is binary XOR) ...
                    // We calculate intersection point and add it to polygon ...

                    // Vertical Line Base Case ...
                    if(pnt1.x() == pnt2.x())
                    {
                        switch(direction)
                        {
                            case UP:    pntI = {pnt1.x(), y2}; break;
                            case DOWN:  pntI = {pnt1.x(), y1}; break;
                            default: /* Not Possible */ break;
                        }

                        newPolygonList.back().push_back(pntI);
                        continue;
                    }

                    float m = ((float)(pnt2.y() - pnt1.y())) / (pnt2.x() - pnt1.x()), b = pnt1.y() - m * pnt1.x();

                    switch(direction)
                    {
                        case UP:    pntI = {(int)((y2 - b) / m + 0.5), y2}; break;
                        case DOWN:  pntI = {(int)((y1 - b) / m + 0.5), y1}; break;
                        case RIGHT: pntI = {x2, (int)(m * x2 + b + 0.5)}; break;
                        case LEFT:  pntI = {x1, (int)(m * x1 + b + 0.5)}; break;
                    }

                    newPolygonList.back().push_back(pntI);
                }
            }
        }

        if(newPolygonList.back().empty()) newPolygonList.pop_back();
    }

    polygonList = newPolygonList;
    clearScreen(false);
    drawAllCurrentPolygons();

    // Drawing lines for Clipping Boundary
    drawLineBresenham(xLeft, yUp, xRight, yUp, defaultPenColor, false);
    drawLineBresenham(xLeft, yDown, xRight, yDown, defaultPenColor, false);
    drawLineBresenham(xLeft, yDown, xLeft, yUp, defaultPenColor, false);
    drawLineBresenham(xRight, yDown, xRight, yUp, defaultPenColor, false);
}

void MainWindow::clipPolygonWeilerAtherton(int xLeft, int xRight, int yDown, int yUp)
{
//    if(xLeft > xRight) swap(xLeft, xRight);
//    if(yDown > yUp) swap(yDown, yUp);

//    if(xRight - xLeft < 2 || yUp - yDown < 2)
//    {
//        // Base Case of nothing clipped ...
//        clearScreen(true, true);  // Clear polygonList too ...
//        return;
//    }

//    int x1 = xLeft + 1,
//        x2 = xRight - 1,
//        y1 = yDown + 1,
//        y2 = yUp - 1;

//    QVector<QVector<QPoint>> newPolygonList;

//    for(QVector<QPoint> &polygon: polygonList)
//    {
//        QVector<QPoint> clippingPolygon, clippedPolygon;  // clipped polygon is real polygon and clipping is clipping boundary ...
//        QPoint pnt1, pnt2, pntI1, pntI2;
//        Region r1, r2;
//        QMap<QPoint, bool> isEntering;

//        // Constructing point vectors for clipping and clipped polygons
//        for(int i = 0; i < polygon.size(); i++)
//        {
//            pnt1 = polygon[i], pnt2 = polygon[(i + 1) % polygon.size()];
//            r1 = getRegionCode(pnt1, xLeft, xRight, yDown, yUp), r2 = getRegionCode(pnt2, xLeft, xRight, yDown, yUp);

//            if(r1 == INSIDE) clippedPolygon.push_back(pnt1);

//            float m, b;

//            if(!(r1 == INSIDE && r2 == INSIDE))
//            {
//                if(pnt1.x() == pnt2.x())
//                {
//                    // Base case of vertical line ...

//                    continue;
//                }

//                m = ((float)(pnt2.y() - pnt1.y())) / (pnt2.x() - pnt1.x()), b = pnt1.y() - m * pnt1.x();
//            }

//            if(r1 == INSIDE && r2 != INSIDE)
//            {
//                pntI1 = {, }

//                if(r2 & UP)
//                {

//                }
//            }
//        }
//    }

//    polygonList = newPolygonList;
//    clearScreen(false);
//    drawAllCurrentPolygons();

//    // Drawing lines for Clipping Boundary
//    drawLineBresenham(xLeft, yUp, xRight, yUp, defaultPenColor, false);
//    drawLineBresenham(xLeft, yDown, xRight, yDown, defaultPenColor, false);
//    drawLineBresenham(xLeft, yDown, xLeft, yUp, defaultPenColor, false);
//    drawLineBresenham(xRight, yDown, xRight, yUp, defaultPenColor, false);
}




int MainWindow::COMBINATION(int n, int r)
{
    // Computes nCr
    // Allows only till 1000 C 1000

    // Faster ...
    if(r > n / 2) return COMBINATION(n, n - r);

    // Base Case
    if(r > n) return 0;
    else if(n == r || r == 0) return 1;

    // DP
    if(C[n][r] != -1) return C[n][r];

    // Recurrence Relation ...
    return C[n][r] = COMBINATION(n - 1, r) + COMBINATION(n - 1, r - 1);
}

float MainWindow::BEZ(int k, int n, float u)
{
    return (float)(COMBINATION(n, k) * pow(u, k) * pow(1 - u, n - k));
}

void MainWindow::drawBezierCurve()
{
    // Cannot be drawn with less than 2 points ...
    if(bezierControlPoints.size() < 2) return;

    // Drawing Bezier Curve First (k, n, u) ...
    int n = bezierControlPoints.size() - 1;
    float x, y, du, bezierBlendingCoefficient;

    /* du = 0.0001;
    for(float u = 0.0; u <= 1.0; u += du)
    {
        x = y = 0.0;

        for(int k = 0; k <= n; k++)
        {
            bezierBlendingCoefficient = BEZ(k, n, u);
            x += bezierControlPoints[k].x() * bezierBlendingCoefficient;
            y += bezierControlPoints[k].y() * bezierBlendingCoefficient;
        }

        drawUserPoint((int)(x + 0.5), (int)(y + 0.5), defaultBezierColor);
        delay(0.2);

    } */

    // Line Implementation ...
    int xPrev, yPrev;
    bool firstPointDone = false;
    float val = ui->doubleSpinBoxDu->value();
    du = (val >= 0.01 && val <= 0.99)? val: 0.1;
    for(float u = 0.0; u <= 1.0 + du / 2; u += du)
    {
        x = y = 0.0;

        for(int k = 0; k <= n; k++)
        {
            bezierBlendingCoefficient = BEZ(k, n, u);
            x += bezierControlPoints[k].x() * bezierBlendingCoefficient;
            y += bezierControlPoints[k].y() * bezierBlendingCoefficient;
        }

        if(firstPointDone) drawLineBresenham((int)(x + 0.5), (int)(y + 0.5), xPrev, yPrev, defaultBezierColor, false);

        xPrev = (int)(x + 0.5), yPrev = (int)(y + 0.5);
        firstPointDone = true;

        if(ui->checkBoxDelay->isChecked()) delay(0.7);
    }

    // Overwriting the points then ...
    for(QPoint p: bezierControlPoints)
        drawUserPoint(p.x(), p.y(), defaultBresenhamColor);

    ui->frame->setPixmap(QPixmap::fromImage(img));
}




void MainWindow::animateClock()
{
    int R = 20, lH = R - 7, lM = R - 5, lS = R - 3;
    int k = ui->grid_size->value(), xc = (img.height() / k) / 2, yc = xc;


    QTime curTime = QDateTime::currentDateTime().time();
    int HH, MM, SS;
    HH = curTime.hour() % 12, MM = curTime.minute(), SS = curTime.second();


//    QVector<QVector<QPoint>> handPolygonList({
//        { {xc, yc} , {(int)(xc + sin((HH / 12.0) * 2 * M_PI) * lH + 0.5), (int)(yc + cos((HH / 12.0) * 2 * M_PI) * lH + 0.5)} },
//        { {xc, yc} , {(int)(xc + sin((MM / 60.0) * 2 * M_PI) * lM + 0.5), (int)(yc + cos((MM / 60.0) * 2 * M_PI) * lM + 0.5)} },
//        { {xc, yc} , {(int)(xc + sin((SS / 60.0) * 2 * M_PI) * lS + 0.5), (int)(yc + cos((SS / 60.0) * 2 * M_PI) * lS + 0.5)} },
//    });

    QPoint HHand((int)(xc + sin((HH / 12.0) * 2 * M_PI) * lH + 0.5), (int)(yc + cos((HH / 12.0) * 2 * M_PI) * lH + 0.5)),
           MHand((int)(xc + sin((MM / 60.0) * 2 * M_PI) * lM + 0.5), (int)(yc + cos((MM / 60.0) * 2 * M_PI) * lM + 0.5)),
           SHand((int)(xc + sin((SS / 60.0) * 2 * M_PI) * lS + 0.5), (int)(yc + cos((SS / 60.0) * 2 * M_PI) * lS + 0.5));

    Matrix mRot6, mRot30, mTr, mTrRev;
    mRot6.setRotationMatrix(6 * M_PI / 180.0);
    mRot30.setRotationMatrix(30 * M_PI / 180.0);
    mTr.setTranslationMatrix(-xc, -yc);
    mTrRev.setTranslationMatrix(xc, yc);

    while(isClockAnimating)
    {
        // Clearing Screen ...
        clearScreen();

        // DRAWING CLOCK ......
        // Outer Circle
        drawCircleMidpoint(xc, yc, R);

        // Hands
        drawLineBresenham(xc, yc, HHand.x(), HHand.y(), defaultLineColor);
        drawLineBresenham(xc, yc, MHand.x(), MHand.y(), defaultBresenhamColor);
        drawLineBresenham(xc, yc, SHand.x(), SHand.y(), defaultPenColor);

        cout << HH << ":" << MM << ":" << SS << "  -->  ";
        cout << "(" << HHand.x() << "," << HHand.y() << ") : (" << MHand.x() << "," << MHand.y() << ") : (" << SHand.x() << "," << SHand.y() << ")" << endl;

        // Advance time
        SS = (SS + 1) % 60;
        if(!SS) MM = (MM + 1) % 60;
        if(!MM) HH = (HH + 1) % 12;

        HHand = {(int)(xc + sin((HH / 12.0) * 2 * M_PI) * lH + 0.5), (int)(yc - cos((HH / 12.0) * 2 * M_PI) * lH + 0.5)},
        MHand = {(int)(xc + sin((MM / 60.0) * 2 * M_PI) * lM + 0.5), (int)(yc - cos((MM / 60.0) * 2 * M_PI) * lM + 0.5)},
        SHand = {(int)(xc + sin((SS / 60.0) * 2 * M_PI) * lS + 0.5), (int)(yc - cos((SS / 60.0) * 2 * M_PI) * lS + 0.5)};


//        // Rotation not accurate unless we use float points ...
//        SS = (SS + 1) % 60, SHand = Matrix::multiply(mTr, SHand),
//                            SHand = Matrix::multiply(mRot6, SHand),
//                            SHand = Matrix::multiply(mTrRev, SHand);
//        if(!SS) MM = (MM + 1) % 60, MHand = Matrix::multiply(mTr, MHand),
//                                    MHand = Matrix::multiply(mRot6, MHand),
//                                    MHand = Matrix::multiply(mTrRev, MHand);
//        if(!MM) HH = (HH + 1) % 12, HHand = Matrix::multiply(mTr, HHand),
//                                    HHand = Matrix::multiply(mRot30, HHand),
//                                    HHand = Matrix::multiply(mTrRev, HHand);


        // Delay for 1s ...
        ui->frame->setPixmap(QPixmap::fromImage(img));
        ui->frame->repaint();
        Sleeper::msleep((int)(1000));
    }

    // Clear Screen at end ...
    clearScreen();
}




//  All debug drawing functions ......................................................................................................................


void MainWindow::drawLineSimpleDebug(int x1, int y1, int x2, int y2)
{
    if(x1 == x2)
    {
        // vertical line
        int start = std::min(y1, y2), end = std::max(y1, y2);
        for(int j = start; j <= end; j++);
    }
    else
    {
        float m = ((float)(y1 - y2)) / (x1 - x2);
        int start = std::min(x1, x2), end = std::max(x1, x2), yStart = (start == x1)? y1: y2, j;
        for(int i = start; i <= end; i++)
        {
            j = (int)(yStart + m * (i - start) + 0.5);
            // drawUserPoint(i, j, defaultLineColor);
        }
    }
}

void MainWindow::drawLineDDADebug(int x1, int y1, int x2, int y2)
{
    float dx, dy, y, x, Dx, Dy;

    // Base Case
    if(x1 == x2 && y1 == y2){ return; } // else it will cause division by 0

    dx = x2 - x1;  // total span in x
    dy = y2 - y1;  // total span in y
    x = x1;
    y = y1;

    // incremental steps in x and y
    if(abs(dx) > abs(dy))
    {
        Dx = (dx > 0)? 1: -1;
        Dy = dy / abs(dx);
    }
    else
    {
        Dx = dx / abs(dy);
        Dy = (dy > 0)? 1: -1;
    }

    // to get the number of iterations correctly
    if(dx < 0) dx = -dx;
    if(dy < 0) dy = -dy;

    int ix, iy, end = (dx > dy)? dx: dy;  // integer x and y co-ordinates
    for(int k = 0; k <= end; k++)
    {
        ix = (int)(x + 0.5), iy = (int)(y + 0.5);
        // drawUserPoint(ix, iy, defaultLineColor);
        x += Dx, y += Dy;
    }
}

void MainWindow::drawLineBresenhamDebug(int x1, int y1, int x2, int y2, QRgb color)
{
    // Case for horizontal line
    if(y1 == y2)
    {
        int start = std::min(x1, x2), end = std::max(x1, x2);
        for(int i = start; i <= end; i++);
        return;
    }

    // Case for vertical line
    if(x1 == x2)
    {
        int start = std::min(y1, y2), end = std::max(y1, y2);
        for(int j = start; j <= end; j++);
        return;
    }

    int dx = x2 - x1, dy = y2 - y1, p;

    // Case for |m| <= 1
    if(abs(dy) <= abs(dx))
    {
        if(x1 > x2) std::swap(x1, x2), std::swap(y1, y2), dx = -dx, dy = -dy;

        int j = (dy > 0)? y1: (y1 + 1);
        p = 2 * dy - dx;
        // drawUserPoint(x1, y1, color);  // Plotting initial point ...
        for(int i = x1 + 1; i <= x2; i++)
        {
            if(dy > 0)
            {
                if(p < 0) p += 2 * dy;
                else p += 2 * dy - 2 * dx, j++;
            }
            else
            {
                if(p > 0) p += 2 * dy;
                else p += 2 * dy + 2 * dx, j--;
            }

            // drawUserPoint(i, j, color);
        }
    }

    // Case for |m| > 1
    else
    {
        if(y1 > y2) std::swap(x1, x2), std::swap(y1, y2), dx = -dx, dy = -dy;

        int i = (dx > 0)? x1: (x1 + 1);
        p = 2 * dx - dy;
        // drawUserPoint(x1, y1, color);  // Plotting initial point ...
        for(int j = y1 + 1; j <= y2; j++)
        {
            if(dx > 0)
            {
                if(p < 0) p += 2 * dx;
                else p += 2 * dx - 2 * dy, i++;
            }
            else
            {
                if(p > 0) p += 2 * dx;
                else p += 2 * dx + 2 * dy, i--;
            }

            // drawUserPoint(i, j, color);
        }
    }
}




void MainWindow::drawCircleSimpleDebug(int xc, int yc, int r)
{
    // Polar
    if ( r <= 0 ) return;

    // pi / 4 = 0.79
    for (int i = 0; ((float)i)/r <= 0.79; i++)
    {
        float theta = ((float)i) / r;
        float x = xc + r * std::cos(theta);
        float y = yc + r * std::sin(theta);
        int ix = (x == 0)? 0: (int)(x + 0.5 * abs(x) / x);
        int iy = (y == 0)? 0: (int)(y + 0.5 * abs(y) / y);

        int xDiff = ix - xc, yDiff = iy - yc;
        // drawUserPoint(ix, iy, defaultLineColor);
//        drawUserPoint(xc + xDiff, yc + yDiff, defaultLineColor);
//        drawUserPoint(xc - xDiff, yc + yDiff, defaultLineColor);
//        drawUserPoint(xc + xDiff, yc - yDiff, defaultLineColor);
//        drawUserPoint(xc - xDiff, yc - yDiff, defaultLineColor);
//        drawUserPoint(xc + yDiff, yc + xDiff, defaultLineColor);
//        drawUserPoint(xc - yDiff, yc + xDiff, defaultLineColor);
//        drawUserPoint(xc + yDiff, yc - xDiff, defaultLineColor);
//        drawUserPoint(xc - yDiff, yc - xDiff, defaultLineColor);

        // delay();
    }

    // Simple
    /* int j;
    for(int i = -r; i <= r; i++)
    {
        j = (int)(sqrt(r * r - i * i) + 0.5);
        drawUserPoint(xc + i, yc + j, defaultLineColor);
        drawUserPoint(xc + i, yc - j, defaultLineColor);

        delay();
    } */
}

void MainWindow::drawCircleMidpointDebug(int xc, int yc, int r)
{
    // defaultLineColor = qRgb(255, 0, 0);

    // 8 way symmetry
    int x, y, p;
    x = 0, y = r, p = 1 - r;

    while(x <= y)
    {
//        drawUserPoint(xc + x, yc + y, defaultLineColor);
//        drawUserPoint(xc + x, yc - y, defaultLineColor);
//        drawUserPoint(xc - x, yc + y, defaultLineColor);
//        drawUserPoint(xc - x, yc - y, defaultLineColor);

//        drawUserPoint(xc + y, yc + x, defaultLineColor);
//        drawUserPoint(xc + y, yc - x, defaultLineColor);
//        drawUserPoint(xc - y, yc + x, defaultLineColor);
//        drawUserPoint(xc - y, yc - x, defaultLineColor);

        x++;
        if(p < 0) p += 2 * x + 1;
        else y--, p += 2 * x + 1 - 2 * y;

        // delay();
    }
}

void MainWindow::drawCircleBresenhamDebug(int xc, int yc, int r)
{
    // 8 way symmetry (with updated parameters)
    int x, y, p;
    x = 0, y = r, p = 3 - 2 * r;

    while(x <= y)
    {
//        drawUserPoint(xc + x, yc + y, defaultLineColor);
//        drawUserPoint(xc + x, yc - y, defaultLineColor);
//        drawUserPoint(xc - x, yc + y, defaultLineColor);
//        drawUserPoint(xc - x, yc - y, defaultLineColor);

//        drawUserPoint(xc + y, yc + x, defaultLineColor);
//        drawUserPoint(xc + y, yc - x, defaultLineColor);
//        drawUserPoint(xc - y, yc + x, defaultLineColor);
//        drawUserPoint(xc - y, yc - x, defaultLineColor);

        x++;
        if(p < 0) p += 4 * x + 6;
        else y--, p += 4 * x + 10 - 4 * y;

        // delay();
    }
}




void MainWindow::drawEllipseSimpleDebug(int xc, int yc, int rx, int ry)
{
    // defaultLineColor = qRgb(255, 255, 0);

    // Polar
    int r_l = std::max(rx, ry);

    // pi / 2 = 1.57
    for (int i = 0; ((float)i)/r_l <= 1.58; i++)
    {
        float theta = ((float)i) / r_l;
        float x = xc + rx * std::cos(theta);
        float y = yc + ry * std::sin(theta);
        int ix = (x == 0)? 0: (int)(x + 0.5 * abs(x) / x);
        int iy = (y == 0)? 0: (int)(y + 0.5 * abs(y) / y);

        int xDiff = ix - xc, yDiff = iy - yc;
        // drawUserPoint(ix, iy, defaultLineColor);

//        drawUserPoint(xc + xDiff, yc + yDiff, defaultLineColor);
//        drawUserPoint(xc - xDiff, yc + yDiff, defaultLineColor);
//        drawUserPoint(xc + xDiff, yc - yDiff, defaultLineColor);
//        drawUserPoint(xc - xDiff, yc - yDiff, defaultLineColor);

//        delay();
    }

    // Simple
    /* int j;
    for(int i = -rx; i <= rx; i++)
    {
        j = (int)(ry * sqrt(1 - ((double)i * i) / (rx * rx)) + 0.5);
        drawUserPoint(xc + i, yc + j, defaultLineColor);
        drawUserPoint(xc + i, yc - j, defaultLineColor);

        delay();
    } */
}

void MainWindow::drawEllipseMidpointDebug(int xc, int yc, int rx, int ry)
{
    // defaultLineColor = qRgb(255, 0, 0);

//    drawUserPoint(xc + rx, yc, defaultLineColor);
//    drawUserPoint(xc, yc + ry, defaultLineColor);
//    drawUserPoint(xc - rx, yc, defaultLineColor);
//    drawUserPoint(xc, yc - ry, defaultLineColor);
    int x = 0;
    int y = ry;
    int p = 4 * ry * ry - 4 * rx * rx * ry + rx * rx;
    while (ry * ry * x < rx * rx * y) {
        if (p < 0) {
            p += 4 * ry * ry * (2 * x + 3);
            x += 1;
        } else {
            p += 4 * ry * ry * (2 * x + 3) + 8 * rx * rx * (1 - y);
            x += 1;
            y -= 1;
        }
//        drawUserPoint(x + xc, y + yc, defaultLineColor);
//        drawUserPoint(x + xc, -y + yc, defaultLineColor);
//        drawUserPoint(-x + xc, y + yc, defaultLineColor);
//        drawUserPoint(-x + xc, -y + yc, defaultLineColor);

//        delay();
    }

    x = rx;
    y = 0;
    p = 4 * rx * rx - 4 * ry * ry * rx + ry * ry;
    while (ry * ry * x > rx * rx * y) {
        if (p < 0) {
            p += 4 * rx * rx * (2 * y + 3);
            y += 1;
        } else {
            p += 4 * rx * rx * (2 * y + 3) + 8 * ry * ry * (1 - x);
            y += 1;
            x -= 1;
        }
//        drawUserPoint(x + xc, y + yc,  defaultLineColor);
//        drawUserPoint(x + xc, -y + yc, defaultLineColor);
//        drawUserPoint(-x + xc, y + yc, defaultLineColor);
//        drawUserPoint(-x + xc, -y + yc, defaultLineColor);

//        delay();
    }
}




void MainWindow::floodFillDebug(int x, int y, QRgb initialColor, QRgb finalColor)
{
    // 4 neighbour method ...
    int k = ui->grid_size->value();
    if(x < 0 || y < 0 || x >= img.width() / k || y >= img.height() / k || getUserPoint(x, y) != initialColor) return;

    drawUserPoint(x, y, finalColor);
    floodFill(x + 1, y, initialColor, finalColor);
    floodFill(x - 1, y, initialColor, finalColor);
    floodFill(x, y + 1, initialColor, finalColor);
    floodFill(x, y - 1, initialColor, finalColor);
}

void MainWindow::boundaryFillDebug(int x, int y)
{
    // 4 neighbour method ...
    // std::cout<<x<<" "<<y<<std::endl;
    int k = ui->grid_size->value();
    if(x < 0 || y < 0 || x >= img.width() / k || y >= img.height() / k || getUserPoint(x, y) == defaultLineColor || getUserPoint(x, y) == defaultFillColor) return;

    drawUserPoint(x, y, defaultFillColor);
    // drawUserPoint(x, y, defaultLineColor);
    boundaryFill(x + 1, y);
    boundaryFill(x - 1, y);
    boundaryFill(x, y + 1);
    boundaryFill(x, y - 1);
}


void MainWindow::on_pushButtonScanFill_clicked()
{
    if(drawPolygonInputPointState) return;

    scanlineInputPointState = !scanlineInputPointState;

    if(!scanlineInputPointState)
    {
        // Drawing final line ...
        drawLineDDA(ScanlinePts.front().x(),
                    ScanlinePts.front().y(),
                    ScanlinePts.back().x(),
                    ScanlinePts.back().y());

        timer.start();
        scanlineFill();
        ns = timer.nsecsElapsed();

        // Drawing boundary lines
//        int cnt = ScanlinePts.count();
//        for(int i = 0; i < cnt; i++)
//            drawLineDDA(ScanlinePts[i].x(), ScanlinePts[i].y(), ScanlinePts[(i + 1) % cnt].x(), ScanlinePts[(i + 1) % cnt].y());

        ui->time_elapsed->setText(QString::number(ns) + " ns");

        ui->frame->setPixmap(QPixmap::fromImage(img));

        ScanlinePts.clear();
    }
}


void MainWindow::on_pushButtonDrawPolygon_clicked()
{
    if(scanlineInputPointState) return;

    drawPolygonInputPointState = !drawPolygonInputPointState;

    if(drawPolygonInputPointState)
    {
        // Points will start to be drawn now ...
        polygonList.push_back(QVector<QPoint>());
    }
    else
    {
        if(polygonList.back().empty()){ polygonList.pop_back(); return; }

        // Drawing final line ...
        drawLineDDA(polygonList.back().front().x(),
                    polygonList.back().front().y(),
                    polygonList.back().back().x(),
                    polygonList.back().back().y());

        ui->frame->setPixmap(QPixmap::fromImage(img));
    }
}


void MainWindow::on_pushButtonReflectXAxis_clicked()
{
    int k = ui->grid_size->value(), midPnt = (img.height() / k) / 2;
    Matrix mTr, mTrRev, mReflX;
    mReflX.setReflectionMatrixXAxis();
    mTr.setTranslationMatrix(-midPnt, -midPnt);
    mTrRev.setTranslationMatrix(midPnt, midPnt);

    for(QVector<QPoint> &polygon: polygonList)
        for(QPoint &vertex: polygon)
//            std::cout<<vertex.x()<<","<<vertex.y()<<"\t",
            vertex = Matrix::multiply(mTr, vertex),
//            std::cout<<" --> "<<vertex.x()<<","<<vertex.y(),
            vertex = Matrix::multiply(mReflX, vertex),
//            std::cout<<" --> "<<vertex.x()<<","<<vertex.y(),
            vertex = Matrix::multiply(mTrRev, vertex)
//          , std::cout<<" --> "<<vertex.x()<<","<<vertex.y()<<std::endl
            ;

    clearScreen(false);
    drawAllCurrentPolygons();

//    std::cout << "Done ..." << std::endl;
}


void MainWindow::on_pushButtonReflectYAxis_clicked()
{
    int k = ui->grid_size->value(), midPnt = (img.height() / k) / 2;
    Matrix mTr, mTrRev, mReflY;
    mReflY.setReflectionMatrixYAxis();
    mTr.setTranslationMatrix(-midPnt, -midPnt);
    mTrRev.setTranslationMatrix(midPnt, midPnt);

    for(QVector<QPoint> &polygon: polygonList)
        for(QPoint &vertex: polygon)
//            std::cout<<vertex.x()<<","<<vertex.y()<<"\t",
            vertex = Matrix::multiply(mTr, vertex),
            vertex = Matrix::multiply(mReflY, vertex),
            vertex = Matrix::multiply(mTrRev, vertex)
//          , std::cout<<" --> "<<vertex.x()<<","<<vertex.y()<<"\n"
            ;

    clearScreen(false);
    drawAllCurrentPolygons();
}


void MainWindow::on_pushButtonReflectionXYAxis_clicked()
{
    int k = ui->grid_size->value(), midPnt = (img.height() / k) / 2;
    Matrix mTr, mTrRev, mReflXY;
    mReflXY.setReflectionMatrixXYAxis();
    mTr.setTranslationMatrix(-midPnt, -midPnt);
    mTrRev.setTranslationMatrix(midPnt, midPnt);

    for(QVector<QPoint> &polygon: polygonList)
        for(QPoint &vertex: polygon)
//            std::cout<<vertex.x()<<","<<vertex.y()<<"\t",
            vertex = Matrix::multiply(mTr, vertex),
            vertex = Matrix::multiply(mReflXY, vertex),
            vertex = Matrix::multiply(mTrRev, vertex)
//          , std::cout<<" --> "<<vertex.x()<<","<<vertex.y()<<"\n"
            ;

    clearScreen(false);
    drawAllCurrentPolygons();
}


void MainWindow::on_pushButtonAnimateClock_clicked()
{
    isClockAnimating = !isClockAnimating;

    if(isClockAnimating)
    {
         animateClock();
//        QThread *th = QThread::create(animateClock);
//        th->start();
    }
}


void MainWindow::on_pushButtonBezierCurve_clicked()
{
    bezierInputState = !bezierInputState;

    if(!bezierInputState) drawBezierCurve(), bezierControlPoints.clear();
}

