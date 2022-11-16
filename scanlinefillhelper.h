#ifndef SCANLINEFILLHELPER_H
#define SCANLINEFILLHELPER_H

#include <QtCore>

struct Edge
{
    int yUpper;
    float xIntersect, dxPerScan;
    Edge *next;
};

class ScanlineFillHelper
{
public:
    Edge *list;

    ScanlineFillHelper();
    void insertEdge(Edge* list, Edge *edge);
    int yNext(int k, QVector<QPoint> &pts);
    void makeEdgeRec(QPoint lower, QPoint upper, int yComp, Edge *edge, Edge *edges[]);
    void buildEdgeList(QVector<QPoint> &pts, Edge *edges[]);
    void buildActiveList(int scan, Edge *active, Edge *edges[]);
//    void fillScan(int scan, Edge *active);
    void deleteAfter(Edge *q);
    void updateActiveList(int scan, Edge *active);
    void resortActiveList(Edge *active);
//    void scanfill(QVector<QPoint> &pts);
};

#endif // SCANLINEFILLHELPER_H
