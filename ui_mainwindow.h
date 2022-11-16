/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include <my_label.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    my_label *frame;
    QLabel *mouse_movement;
    QLabel *label_3;
    QLabel *mouse_pressed;
    QLabel *label_5;
    QFrame *x_axis;
    QFrame *y_axis;
    QCheckBox *show_axes;
    QPushButton *Draw;
    QSpinBox *circle_radius;
    QRadioButton *draw_circle;
    QRadioButton *draw_line;
    QPushButton *set_point1;
    QPushButton *set_point2;
    QPushButton *pushButton;
    QPushButton *show_grid;
    QSpinBox *grid_size;
    QCheckBox *checkBoxDrawBoxPoint;
    QLabel *label;
    QLabel *mouse_released;
    QLabel *label_2;
    QRadioButton *radioButtonLineSimple;
    QRadioButton *radioButtonLineDDA;
    QRadioButton *radioButtonLineBresenham;
    QLabel *label_4;
    QRadioButton *radioButtonCircleSimple;
    QRadioButton *radioButtonCircleMidpoint;
    QLabel *label_6;
    QRadioButton *radioButtonEllipseSimple;
    QRadioButton *radioButtonEllipseMidpoint;
    QRadioButton *radioButtonCircleBresenham;
    QLabel *label_7;
    QRadioButton *radioButtonFloodFill;
    QRadioButton *radioButtonUserPen;
    QRadioButton *radioButton;
    QRadioButton *radioButtonBoundaryFill;
    QLabel *label_8;
    QLabel *time_elapsed;
    QPushButton *pushButtonShowAxes;
    QSpinBox *spinBoxRadius;
    QSpinBox *spinBoxRx;
    QSpinBox *spinBoxRy;
    QPushButton *pushButtonScanFill;
    QComboBox *comboBoxFillNeighbour;
    QCheckBox *checkBoxDelay;
    QLabel *label_9;
    QRadioButton *radioButtonTranslate;
    QRadioButton *radioButtonScale;
    QRadioButton *radioButtonShear;
    QPushButton *pushButtonDrawPolygon;
    QDoubleSpinBox *doubleSpinBoxScaleX;
    QDoubleSpinBox *doubleSpinBoxScaleY;
    QDoubleSpinBox *doubleSpinBoxShearX;
    QDoubleSpinBox *doubleSpinBoxShearY;
    QRadioButton *radioButtonRotate;
    QDoubleSpinBox *doubleSpinBoxRotationAngle;
    QRadioButton *radioButtonReflect;
    QPushButton *pushButtonReflectXAxis;
    QPushButton *pushButtonReflectYAxis;
    QPushButton *pushButtonReflectionXYAxis;
    QLabel *label_10;
    QRadioButton *radioButtonClipLine;
    QRadioButton *radioButtonClipPolygon;
    QPushButton *pushButtonAnimateClock;
    QPushButton *pushButtonBezierCurve;
    QDoubleSpinBox *doubleSpinBoxDu;
    QLabel *label_11;
    QRadioButton *radioButtonLiangBarsky;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1295, 736);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        frame = new my_label(centralWidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 700, 700));
        frame->setCursor(QCursor(Qt::CrossCursor));
        frame->setMouseTracking(true);
        frame->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));
        frame->setFrameShape(QFrame::Box);
        frame->setLineWidth(1);
        mouse_movement = new QLabel(centralWidget);
        mouse_movement->setObjectName(QString::fromUtf8("mouse_movement"));
        mouse_movement->setGeometry(QRect(720, 60, 261, 31));
        mouse_movement->setFrameShape(QFrame::Panel);
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(710, 40, 141, 20));
        mouse_pressed = new QLabel(centralWidget);
        mouse_pressed->setObjectName(QString::fromUtf8("mouse_pressed"));
        mouse_pressed->setGeometry(QRect(720, 130, 261, 31));
        mouse_pressed->setFrameShape(QFrame::Panel);
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(720, 110, 111, 20));
        x_axis = new QFrame(centralWidget);
        x_axis->setObjectName(QString::fromUtf8("x_axis"));
        x_axis->setGeometry(QRect(0, 225, 700, 1));
        x_axis->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 85, 255);"));
        x_axis->setFrameShape(QFrame::HLine);
        x_axis->setFrameShadow(QFrame::Sunken);
        y_axis = new QFrame(centralWidget);
        y_axis->setObjectName(QString::fromUtf8("y_axis"));
        y_axis->setGeometry(QRect(225, 0, 1, 700));
        y_axis->setStyleSheet(QString::fromUtf8("\n"
"background-color: rgb(85, 255, 255);"));
        y_axis->setFrameShape(QFrame::VLine);
        y_axis->setFrameShadow(QFrame::Sunken);
        show_axes = new QCheckBox(centralWidget);
        show_axes->setObjectName(QString::fromUtf8("show_axes"));
        show_axes->setGeometry(QRect(720, 10, 101, 21));
        Draw = new QPushButton(centralWidget);
        Draw->setObjectName(QString::fromUtf8("Draw"));
        Draw->setGeometry(QRect(710, 290, 101, 41));
        circle_radius = new QSpinBox(centralWidget);
        circle_radius->setObjectName(QString::fromUtf8("circle_radius"));
        circle_radius->setGeometry(QRect(790, 190, 46, 20));
        draw_circle = new QRadioButton(centralWidget);
        draw_circle->setObjectName(QString::fromUtf8("draw_circle"));
        draw_circle->setGeometry(QRect(710, 190, 81, 17));
        draw_line = new QRadioButton(centralWidget);
        draw_line->setObjectName(QString::fromUtf8("draw_line"));
        draw_line->setGeometry(QRect(710, 240, 77, 17));
        set_point1 = new QPushButton(centralWidget);
        set_point1->setObjectName(QString::fromUtf8("set_point1"));
        set_point1->setGeometry(QRect(790, 240, 81, 23));
        set_point2 = new QPushButton(centralWidget);
        set_point2->setObjectName(QString::fromUtf8("set_point2"));
        set_point2->setGeometry(QRect(880, 240, 81, 23));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(850, 180, 101, 41));
        show_grid = new QPushButton(centralWidget);
        show_grid->setObjectName(QString::fromUtf8("show_grid"));
        show_grid->setGeometry(QRect(830, 290, 93, 41));
        grid_size = new QSpinBox(centralWidget);
        grid_size->setObjectName(QString::fromUtf8("grid_size"));
        grid_size->setGeometry(QRect(940, 290, 42, 41));
        checkBoxDrawBoxPoint = new QCheckBox(centralWidget);
        checkBoxDrawBoxPoint->setObjectName(QString::fromUtf8("checkBoxDrawBoxPoint"));
        checkBoxDrawBoxPoint->setGeometry(QRect(830, 10, 131, 24));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(1000, 10, 111, 20));
        mouse_released = new QLabel(centralWidget);
        mouse_released->setObjectName(QString::fromUtf8("mouse_released"));
        mouse_released->setGeometry(QRect(1000, 30, 261, 31));
        mouse_released->setFrameShape(QFrame::Panel);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(1000, 70, 261, 20));
        radioButtonLineSimple = new QRadioButton(centralWidget);
        radioButtonLineSimple->setObjectName(QString::fromUtf8("radioButtonLineSimple"));
        radioButtonLineSimple->setGeometry(QRect(1000, 90, 71, 24));
        radioButtonLineDDA = new QRadioButton(centralWidget);
        radioButtonLineDDA->setObjectName(QString::fromUtf8("radioButtonLineDDA"));
        radioButtonLineDDA->setGeometry(QRect(1080, 90, 61, 24));
        radioButtonLineBresenham = new QRadioButton(centralWidget);
        radioButtonLineBresenham->setObjectName(QString::fromUtf8("radioButtonLineBresenham"));
        radioButtonLineBresenham->setGeometry(QRect(1150, 90, 110, 24));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(1000, 130, 221, 20));
        radioButtonCircleSimple = new QRadioButton(centralWidget);
        radioButtonCircleSimple->setObjectName(QString::fromUtf8("radioButtonCircleSimple"));
        radioButtonCircleSimple->setGeometry(QRect(1000, 150, 110, 24));
        radioButtonCircleMidpoint = new QRadioButton(centralWidget);
        radioButtonCircleMidpoint->setObjectName(QString::fromUtf8("radioButtonCircleMidpoint"));
        radioButtonCircleMidpoint->setGeometry(QRect(1080, 150, 110, 24));
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(1000, 190, 221, 20));
        radioButtonEllipseSimple = new QRadioButton(centralWidget);
        radioButtonEllipseSimple->setObjectName(QString::fromUtf8("radioButtonEllipseSimple"));
        radioButtonEllipseSimple->setGeometry(QRect(1000, 210, 110, 24));
        radioButtonEllipseMidpoint = new QRadioButton(centralWidget);
        radioButtonEllipseMidpoint->setObjectName(QString::fromUtf8("radioButtonEllipseMidpoint"));
        radioButtonEllipseMidpoint->setGeometry(QRect(1100, 210, 110, 24));
        radioButtonCircleBresenham = new QRadioButton(centralWidget);
        radioButtonCircleBresenham->setObjectName(QString::fromUtf8("radioButtonCircleBresenham"));
        radioButtonCircleBresenham->setGeometry(QRect(1170, 150, 110, 24));
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(1000, 250, 151, 20));
        radioButtonFloodFill = new QRadioButton(centralWidget);
        radioButtonFloodFill->setObjectName(QString::fromUtf8("radioButtonFloodFill"));
        radioButtonFloodFill->setGeometry(QRect(1000, 270, 110, 24));
        radioButtonUserPen = new QRadioButton(centralWidget);
        radioButtonUserPen->setObjectName(QString::fromUtf8("radioButtonUserPen"));
        radioButtonUserPen->setGeometry(QRect(710, 340, 81, 24));
        radioButton = new QRadioButton(centralWidget);
        radioButton->setObjectName(QString::fromUtf8("radioButton"));
        radioButton->setGeometry(QRect(710, 370, 191, 24));
        radioButtonBoundaryFill = new QRadioButton(centralWidget);
        radioButtonBoundaryFill->setObjectName(QString::fromUtf8("radioButtonBoundaryFill"));
        radioButtonBoundaryFill->setGeometry(QRect(1090, 270, 110, 24));
        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(710, 400, 101, 20));
        time_elapsed = new QLabel(centralWidget);
        time_elapsed->setObjectName(QString::fromUtf8("time_elapsed"));
        time_elapsed->setGeometry(QRect(820, 400, 91, 21));
        time_elapsed->setFrameShape(QFrame::Box);
        pushButtonShowAxes = new QPushButton(centralWidget);
        pushButtonShowAxes->setObjectName(QString::fromUtf8("pushButtonShowAxes"));
        pushButtonShowAxes->setGeometry(QRect(830, 340, 93, 29));
        spinBoxRadius = new QSpinBox(centralWidget);
        spinBoxRadius->setObjectName(QString::fromUtf8("spinBoxRadius"));
        spinBoxRadius->setGeometry(QRect(720, 570, 42, 26));
        spinBoxRx = new QSpinBox(centralWidget);
        spinBoxRx->setObjectName(QString::fromUtf8("spinBoxRx"));
        spinBoxRx->setGeometry(QRect(860, 570, 42, 26));
        spinBoxRy = new QSpinBox(centralWidget);
        spinBoxRy->setObjectName(QString::fromUtf8("spinBoxRy"));
        spinBoxRy->setGeometry(QRect(920, 570, 42, 26));
        pushButtonScanFill = new QPushButton(centralWidget);
        pushButtonScanFill->setObjectName(QString::fromUtf8("pushButtonScanFill"));
        pushButtonScanFill->setGeometry(QRect(1200, 270, 93, 21));
        comboBoxFillNeighbour = new QComboBox(centralWidget);
        comboBoxFillNeighbour->setObjectName(QString::fromUtf8("comboBoxFillNeighbour"));
        comboBoxFillNeighbour->setGeometry(QRect(1000, 300, 121, 21));
        checkBoxDelay = new QCheckBox(centralWidget);
        checkBoxDelay->setObjectName(QString::fromUtf8("checkBoxDelay"));
        checkBoxDelay->setGeometry(QRect(1140, 300, 111, 24));
        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(1000, 330, 181, 20));
        radioButtonTranslate = new QRadioButton(centralWidget);
        radioButtonTranslate->setObjectName(QString::fromUtf8("radioButtonTranslate"));
        radioButtonTranslate->setGeometry(QRect(1000, 350, 101, 24));
        radioButtonScale = new QRadioButton(centralWidget);
        radioButtonScale->setObjectName(QString::fromUtf8("radioButtonScale"));
        radioButtonScale->setGeometry(QRect(1110, 350, 61, 21));
        radioButtonShear = new QRadioButton(centralWidget);
        radioButtonShear->setObjectName(QString::fromUtf8("radioButtonShear"));
        radioButtonShear->setGeometry(QRect(1000, 380, 61, 24));
        pushButtonDrawPolygon = new QPushButton(centralWidget);
        pushButtonDrawPolygon->setObjectName(QString::fromUtf8("pushButtonDrawPolygon"));
        pushButtonDrawPolygon->setGeometry(QRect(710, 430, 101, 31));
        doubleSpinBoxScaleX = new QDoubleSpinBox(centralWidget);
        doubleSpinBoxScaleX->setObjectName(QString::fromUtf8("doubleSpinBoxScaleX"));
        doubleSpinBoxScaleX->setGeometry(QRect(1180, 350, 51, 21));
        doubleSpinBoxScaleY = new QDoubleSpinBox(centralWidget);
        doubleSpinBoxScaleY->setObjectName(QString::fromUtf8("doubleSpinBoxScaleY"));
        doubleSpinBoxScaleY->setGeometry(QRect(1240, 350, 51, 21));
        doubleSpinBoxShearX = new QDoubleSpinBox(centralWidget);
        doubleSpinBoxShearX->setObjectName(QString::fromUtf8("doubleSpinBoxShearX"));
        doubleSpinBoxShearX->setGeometry(QRect(1070, 380, 51, 21));
        doubleSpinBoxShearY = new QDoubleSpinBox(centralWidget);
        doubleSpinBoxShearY->setObjectName(QString::fromUtf8("doubleSpinBoxShearY"));
        doubleSpinBoxShearY->setGeometry(QRect(1130, 380, 51, 21));
        radioButtonRotate = new QRadioButton(centralWidget);
        radioButtonRotate->setObjectName(QString::fromUtf8("radioButtonRotate"));
        radioButtonRotate->setGeometry(QRect(1000, 410, 81, 21));
        doubleSpinBoxRotationAngle = new QDoubleSpinBox(centralWidget);
        doubleSpinBoxRotationAngle->setObjectName(QString::fromUtf8("doubleSpinBoxRotationAngle"));
        doubleSpinBoxRotationAngle->setGeometry(QRect(1090, 410, 71, 21));
        radioButtonReflect = new QRadioButton(centralWidget);
        radioButtonReflect->setObjectName(QString::fromUtf8("radioButtonReflect"));
        radioButtonReflect->setGeometry(QRect(1170, 410, 91, 21));
        pushButtonReflectXAxis = new QPushButton(centralWidget);
        pushButtonReflectXAxis->setObjectName(QString::fromUtf8("pushButtonReflectXAxis"));
        pushButtonReflectXAxis->setGeometry(QRect(1190, 380, 31, 21));
        pushButtonReflectYAxis = new QPushButton(centralWidget);
        pushButtonReflectYAxis->setObjectName(QString::fromUtf8("pushButtonReflectYAxis"));
        pushButtonReflectYAxis->setGeometry(QRect(1260, 410, 31, 21));
        pushButtonReflectionXYAxis = new QPushButton(centralWidget);
        pushButtonReflectionXYAxis->setObjectName(QString::fromUtf8("pushButtonReflectionXYAxis"));
        pushButtonReflectionXYAxis->setGeometry(QRect(1230, 380, 41, 21));
        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(1000, 440, 161, 20));
        radioButtonClipLine = new QRadioButton(centralWidget);
        radioButtonClipLine->setObjectName(QString::fromUtf8("radioButtonClipLine"));
        radioButtonClipLine->setGeometry(QRect(1000, 460, 51, 21));
        radioButtonClipPolygon = new QRadioButton(centralWidget);
        radioButtonClipPolygon->setObjectName(QString::fromUtf8("radioButtonClipPolygon"));
        radioButtonClipPolygon->setGeometry(QRect(1060, 460, 111, 21));
        pushButtonAnimateClock = new QPushButton(centralWidget);
        pushButtonAnimateClock->setObjectName(QString::fromUtf8("pushButtonAnimateClock"));
        pushButtonAnimateClock->setGeometry(QRect(1220, 590, 61, 31));
        pushButtonBezierCurve = new QPushButton(centralWidget);
        pushButtonBezierCurve->setObjectName(QString::fromUtf8("pushButtonBezierCurve"));
        pushButtonBezierCurve->setGeometry(QRect(1000, 520, 91, 21));
        doubleSpinBoxDu = new QDoubleSpinBox(centralWidget);
        doubleSpinBoxDu->setObjectName(QString::fromUtf8("doubleSpinBoxDu"));
        doubleSpinBoxDu->setGeometry(QRect(1140, 520, 51, 21));
        label_11 = new QLabel(centralWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(1110, 520, 21, 16));
        radioButtonLiangBarsky = new QRadioButton(centralWidget);
        radioButtonLiangBarsky->setObjectName(QString::fromUtf8("radioButtonLiangBarsky"));
        radioButtonLiangBarsky->setGeometry(QRect(1140, 460, 151, 21));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1295, 26));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        frame->setText(QString());
        mouse_movement->setText(QString());
        label_3->setText(QCoreApplication::translate("MainWindow", "    Mouse Movement", nullptr));
        mouse_pressed->setText(QString());
        label_5->setText(QCoreApplication::translate("MainWindow", "Mouse Pressed", nullptr));
        show_axes->setText(QCoreApplication::translate("MainWindow", "Show Axes", nullptr));
        Draw->setText(QCoreApplication::translate("MainWindow", "Draw", nullptr));
        draw_circle->setText(QCoreApplication::translate("MainWindow", "Draw Circle", nullptr));
        draw_line->setText(QCoreApplication::translate("MainWindow", "Draw Line", nullptr));
        set_point1->setText(QCoreApplication::translate("MainWindow", "Set point 1", nullptr));
        set_point2->setText(QCoreApplication::translate("MainWindow", "Set point 2", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "RESET", nullptr));
        show_grid->setText(QCoreApplication::translate("MainWindow", "Draw Grid", nullptr));
        checkBoxDrawBoxPoint->setText(QCoreApplication::translate("MainWindow", "Draw Box Point", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Mouse Released", nullptr));
        mouse_released->setText(QString());
        label_2->setText(QCoreApplication::translate("MainWindow", "LINE DRAWING ALGORITHMS", nullptr));
        radioButtonLineSimple->setText(QCoreApplication::translate("MainWindow", "Simple", nullptr));
        radioButtonLineDDA->setText(QCoreApplication::translate("MainWindow", "DDA", nullptr));
        radioButtonLineBresenham->setText(QCoreApplication::translate("MainWindow", "Bresenham", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "CIRCLE DRAWING ALGORITHMS", nullptr));
        radioButtonCircleSimple->setText(QCoreApplication::translate("MainWindow", "Simple", nullptr));
        radioButtonCircleMidpoint->setText(QCoreApplication::translate("MainWindow", "Midpoint", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "ELLIPSE DRAWING ALGORITHMS", nullptr));
        radioButtonEllipseSimple->setText(QCoreApplication::translate("MainWindow", "Simple", nullptr));
        radioButtonEllipseMidpoint->setText(QCoreApplication::translate("MainWindow", "Midpoint", nullptr));
        radioButtonCircleBresenham->setText(QCoreApplication::translate("MainWindow", "Bresenham", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "FILLING ALGORITHMS", nullptr));
        radioButtonFloodFill->setText(QCoreApplication::translate("MainWindow", "Flood Fill", nullptr));
        radioButtonUserPen->setText(QCoreApplication::translate("MainWindow", "User Pen", nullptr));
        radioButton->setText(QCoreApplication::translate("MainWindow", "Do Nothing and Enjoy  :)", nullptr));
        radioButtonBoundaryFill->setText(QCoreApplication::translate("MainWindow", "Boundary Fill", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Time Elapsed", nullptr));
        time_elapsed->setText(QString());
        pushButtonShowAxes->setText(QCoreApplication::translate("MainWindow", "Show Axes", nullptr));
        pushButtonScanFill->setText(QCoreApplication::translate("MainWindow", "Scanline Fill", nullptr));
        checkBoxDelay->setText(QCoreApplication::translate("MainWindow", "Apply Delay", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "2D TRANSFORMATIONS", nullptr));
        radioButtonTranslate->setText(QCoreApplication::translate("MainWindow", "Translate", nullptr));
        radioButtonScale->setText(QCoreApplication::translate("MainWindow", "Scale", nullptr));
        radioButtonShear->setText(QCoreApplication::translate("MainWindow", "Shear", nullptr));
        pushButtonDrawPolygon->setText(QCoreApplication::translate("MainWindow", "Draw Polygon", nullptr));
        radioButtonRotate->setText(QCoreApplication::translate("MainWindow", "Rotation", nullptr));
        radioButtonReflect->setText(QCoreApplication::translate("MainWindow", "Reflection", nullptr));
        pushButtonReflectXAxis->setText(QCoreApplication::translate("MainWindow", "RX", nullptr));
        pushButtonReflectYAxis->setText(QCoreApplication::translate("MainWindow", "RY", nullptr));
        pushButtonReflectionXYAxis->setText(QCoreApplication::translate("MainWindow", "RXY", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "CLIPPING ALGORITHMS", nullptr));
        radioButtonClipLine->setText(QCoreApplication::translate("MainWindow", "Line", nullptr));
        radioButtonClipPolygon->setText(QCoreApplication::translate("MainWindow", "Polygon", nullptr));
        pushButtonAnimateClock->setText(QCoreApplication::translate("MainWindow", "CLOCK", nullptr));
        pushButtonBezierCurve->setText(QCoreApplication::translate("MainWindow", "Bezier Curve", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "du:", nullptr));
        radioButtonLiangBarsky->setText(QCoreApplication::translate("MainWindow", "Line (Liang Barsky)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
