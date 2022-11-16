#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>


#include <QThread>
#include "scanlinefillhelper.h"
#include "matrix.h"
#include "region_codes.h"

class Sleeper : public QThread
{
public:
    static void usleep(unsigned long usecs){QThread::usleep(usecs);}
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    void Mouse_Pressed();
    void Mouse_Released();
    void showMousePosition(QPoint& pos);
    void drawGrid(int k);
    void removeGrid();
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_show_axes_clicked();

    void on_Draw_clicked();

    void on_set_point1_clicked();

    void on_set_point2_clicked();

    void on_pushButton_clicked();

    void on_show_grid_clicked();

    void on_grid_size_valueChanged(int arg1);

    void on_pushButtonShowAxes_clicked();

    void on_pushButtonScanFill_clicked();

    void on_pushButtonDrawPolygon_clicked();

    void on_pushButtonReflectXAxis_clicked();

    void on_pushButtonReflectYAxis_clicked();

    void on_pushButtonReflectionXYAxis_clicked();

    void on_pushButtonAnimateClock_clicked();

    void on_pushButtonBezierCurve_clicked();

private:
    Ui::MainWindow *ui;
    QElapsedTimer timer;
    QPoint p1,p2;
    QRgb defaultAxesColor;
    QRgb defaultPenColor;
    QRgb defaultLineColor;
    QRgb defaultMidpointColor;
    QRgb defaultBresenhamColor;
    QRgb defaultEmptyColor;
    QRgb defaultFillColor;
    QRgb defaultCircleCenterColor;
    QRgb defaultBezierColor;
    bool timerActionPerformed;
    bool isGridDrawn, areAxesDrawn;
    bool isPenDown, isPenDownPrev;
    bool isClockAnimating;
    int penXPrev, penYPrev;
    int ns;
    QVector<QVector<int>> cntEdge;

    QVector<QVector<QPoint>> polygonList;  // list of current drawn and recognized polygons ...
    bool drawPolygonInputPointState;

    QVector<QPair<QPoint, QPoint>> lineList;  // list of current drawn and recognized lines ...

    void point(int,int);
    void delay(float scale = 1.0);

    ScanlineFillHelper sf;
    QVector<QPoint> ScanlinePts;
    bool scanlineInputPointState;

    QVector<QPoint> bezierControlPoints;
    QVector<QVector<int>> C;
    bool bezierInputState;

    inline int convertToUserX(int x);
    inline int convertToUserY(int y);
    void drawUserPoint(int x, int y, QRgb color);
    QRgb getUserPoint(int x, int y);
    void drawAxes();
    void removeAxes();
    void clearScreen(bool clearPolygonList = true, bool clearLineList = true);

    void drawLineSimple(int x1, int y1, int x2, int y2, bool includeToLineList = true);
    void drawLineDDA(int x1, int y1, int x2, int y2, bool includeToLineList = true);
    void drawLineBresenham(int x1, int y1, int x2, int y2, QRgb color, bool includeToLineList = true);

    void drawCircleSimple(int xc, int yc, int r);
    void drawCircleMidpoint(int xc, int yc, int r);
    void drawCircleBresenham(int xc, int yc, int r);

    void drawEllipseSimple(int xc, int yc, int rx, int ry);
    void drawEllipseMidpoint(int xc, int yc, int rx, int ry);

    void floodFill(int x, int y, QRgb initialColor, QRgb finalColor);
    void boundaryFill(int x, int y);
    void fillScan(int scan, Edge *active);
    void scanlineFill();

    void drawAllCurrentPolygons();
    void translate(int tx, int ty, QVector<QVector<QPoint>> &polygonList, bool updateAndShow = true);
    void scale(int pivotX, int pivotY, float scaleX, float scaleY, QVector<QVector<QPoint>> &polygonList, bool updateAndShow = true);
    void shear(int pivotX, int pivotY, float shearX, float shearY, QVector<QVector<QPoint>> &polygonList, bool updateAndShow = true);
    void rotate(int pivotX, int pivotY, float angle, QVector<QVector<QPoint>> &polygonList, bool updateAndShow = true);
    void reflect(int x1, int y1, int x2, int y2, QVector<QVector<QPoint>> &polygonList, bool updateAndShow = true);

    Region getRegionCode(QPoint pnt, int xLeft, int xRight, int yDown, int yUp);
    void drawAllCurrentLines();
    bool clipTestLiangBarsky(float p, float q, float *t1, float *t2);
    void clipLineCohenSutherland(int xLeft, int xRight, int yDown, int yUp);
    void clipLineLiangBarsky(int xLeft, int xRight, int yDown, int yUp);
    void clipPolygonSutherlandHodgman(int xLeft, int xRight, int yDown, int yUp);
    void clipPolygonWeilerAtherton(int xLeft, int xRight, int yDown, int yUp);

    int COMBINATION(int n, int r);
    float BEZ(int k, int n, float u);
    void drawBezierCurve();

    void animateClock();




    void drawLineSimpleDebug(int x1, int y1, int x2, int y2);
    void drawLineDDADebug(int x1, int y1, int x2, int y2);
    void drawLineBresenhamDebug(int x1, int y1, int x2, int y2, QRgb color);

    void drawCircleSimpleDebug(int xc, int yc, int r);
    void drawCircleMidpointDebug(int xc, int yc, int r);
    void drawCircleBresenhamDebug(int xc, int yc, int r);

    void drawEllipseSimpleDebug(int xc, int yc, int rx, int ry);
    void drawEllipseMidpointDebug(int xc, int yc, int rx, int ry);

    void floodFillDebug(int x, int y, QRgb initialColor, QRgb finalColor);
    void boundaryFillDebug(int x, int y);
    void scanFillDebug();
};

#endif // MAINWINDOW_H
