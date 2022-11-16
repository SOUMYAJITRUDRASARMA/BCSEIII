#include "matrix.h"

Matrix::Matrix(int xDim, int yDim)
{
    v = QVector<QVector<float>>(xDim, QVector<float>(yDim, 0));
//    for(int i = 0; i < MAT_DIM; i++) for(int j = 0; j < MAT_DIM; j++) v[i][j] = 0;
}

Matrix::Matrix(QPoint p): Matrix(3, 1)
{
    // v = QVector<QVector<float>>(MAT_DIM, QVector<float>(MAT_DIM, 0));

    v[0][0] = p.x();
    v[1][0] = p.y();
    v[2][0] = 1;
}

Matrix Matrix::multiply(Matrix m1, Matrix m2)
{
    if(m1[0].size() != m2.v.size()) return Matrix();

    int p = m1.v.size(), q = m2.v.size(), r = m2[0].size();
    Matrix m(p, r);

    for(int i = 0; i < p; i++)
    {
        for(int j = 0; j < r; j++)
        {
            float ans = 0.0;
            for(int k = 0; k < q; k++) ans += m1[i][k] * m2[k][j];
            m[i][j] = ans;
        }
    }

    return m;
}

Matrix::operator QPoint()
{
    float xTemp, yTemp;
    int x, y;
    xTemp = (v[0][0] / v[2][0]);
    yTemp = (v[1][0] / v[2][0]);
    if(xTemp < 0) x = (int)(xTemp - 0.5); else x = (int)(xTemp + 0.5);
    if(yTemp < 0) y = (int)(yTemp - 0.5); else y = (int)(yTemp + 0.5);
    return QPoint(x, y);
}

QVector<float> & Matrix::operator [] (int n)
{
    return v[n];
}




void Matrix::setTranslationMatrix(float tx, float ty)
{
    float x = tx, y = ty;
    v = {{ 1, 0, x },
         { 0, 1, y },
         { 0, 0, 1 }};
}

void Matrix::setScaleMatrix(float sx, float sy)
{
    v = {{ sx,  0,  0 },
         {  0, sy,  0 },
         {  0,  0,  1 }};
}

void Matrix::setShearMatrix(float shx, float shy)
{
    v = {{   1, shx,   0 },
         { shy,   1,   0 },
         {   0,   0,   1 }};
}

void Matrix::setRotationMatrix(float angleRad)
{
    float c = (float)cos(angleRad), s = (float)sin(angleRad);
    v = {{  c, -s,  0 },
         {  s,  c,  0 },
         {  0,  0,  1 }};
}

void Matrix::setReflectionMatrixXAxis()
{
    v = {{  1,  0,  0 },
         {  0, -1,  0 },
         {  0,  0,  1 }};
}

void Matrix::setReflectionMatrixYAxis()
{
    v = {{ -1,  0,  0 },
         {  0,  1,  0 },
         {  0,  0,  1 }};
}

void Matrix::setReflectionMatrixXYAxis()
{
    v = {{ -1,  0,  0 },
         {  0, -1,  0 },
         {  0,  0,  1 }};
}




std::ostream & operator << (std::ostream &ost, Matrix m)
{
    for(int i = 0; i < m.v.size(); i++)
    {
        for(int j = 0; j < m[0].size(); j++)
        {
            ost << m[i][j] << "\t";
        }
        ost << std::endl;
    }

    return ost;
}
