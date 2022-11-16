#ifndef MATRIX_H
#define MATRIX_H

#include <QtCore>
#include <iostream>


class Matrix
{
public:
    QVector<QVector<float>> v;

    Matrix(int xDim = 0, int yDim = 0);
    Matrix(QPoint p);
    static Matrix multiply(Matrix m1, Matrix m2);
    operator QPoint();

    void setTranslationMatrix(float tx, float ty);
    void setScaleMatrix(float sx, float sy);
    void setShearMatrix(float shx, float shy);
    void setRotationMatrix(float angleRad);
    void setReflectionMatrixXAxis();
    void setReflectionMatrixYAxis();
    void setReflectionMatrixXYAxis();

    QVector<float> & operator [] (int n);


    friend std::ostream & operator << (std::ostream &ost, Matrix m);
};

std::ostream & operator << (std::ostream &ost, Matrix m);

#endif // MATRIX_H
