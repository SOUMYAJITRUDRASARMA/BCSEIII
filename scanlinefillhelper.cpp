#include "scanlinefillhelper.h"

ScanlineFillHelper::ScanlineFillHelper()
{
    list = nullptr;
}

void ScanlineFillHelper::insertEdge(Edge* list, Edge *edge)
{
    // Inserts edge into list in order of increasing xIntersect field
    Edge * q = list;
    Edge * p = q->next;

    while (p != nullptr)
        if (edge->xIntersect < p->xIntersect) p = nullptr;
        else q = p, p = p ->next;

    edge->next = q->next;
    q->next = edge;

    /* if(!list)
    {
        list = edge;
        edge->next = nullptr;
        return;
    }
    else if(edge->xIntersect <= list->xIntersect)
    {
        edge->next = list;
        list = edge;
        return;
    }

    Edge *prev = list, *next = list->next;
    while(next)
    {
        if(next->xIntersect >= edge->xIntersect)
        {
            prev->next = edge;
            edge->next = next;
            return;
        }
    }

    prev->next = edge;
    edge->next = nullptr; */
}

int ScanlineFillHelper::yNext(int k, QVector<QPoint> &pts)
{
    // For an index, returns y co-ordinate of the next non-horizontal line
    int cnt = pts.size(),
          j; // = (k + 1) % cnt;

    if ((k + 1) > cnt - 1) j = 0;
    else j = k+1;

    while(pts[k].y() == pts[j].y()) // j = (j + 1) % cnt;
        if (j + 1 > cnt - 1) j = 0;
        else j++;

    return pts[j].y();
}

void ScanlineFillHelper::makeEdgeRec(QPoint lower, QPoint upper, int yComp, Edge *edge, Edge *edges[])
{
    edge->dxPerScan = (float)(upper.x() - lower.x()) / (upper.y() - lower.y());
    edge->xIntersect = lower.x();
    edge->yUpper = upper.y() + ((upper.y() < yComp)? -1: 0);

    insertEdge(edges[lower.y()], edge);
}

void ScanlineFillHelper::buildEdgeList(QVector<QPoint> &pts, Edge *edges[])
{
    Edge *edge;
    QPoint v1, v2;
    int cnt = pts.size();
    int i, yPrev = pts[cnt - 2].y();

    v1.setX(pts[cnt - 1].x());
    v1.setY(pts[cnt - 1].y());

    for(i = 0; i < cnt; i++)
    {
        v2 = pts[i];
        if(v1.y() != v2.y())
        {
            // non-horizontal line
            edge = new Edge();
            if (v1.y() < v2.y()) makeEdgeRec(v1, v2, yNext(i, pts), edge, edges);  // up going edge
            else makeEdgeRec(v2, v1, yPrev, edge, edges);  // down going edge
        }

        yPrev = v1.y();
        v1 = v2;
    }
}

void ScanlineFillHelper::buildActiveList(int scan, Edge *active, Edge *edges[])
{
    Edge *p = edges[scan]->next, *q;
    while(p)
    {
        q = p->next;
        insertEdge(active, p);
        p = q;
    }
}

//void ScanlineFillHelper::fillScan(int scan, Edge *active)
//{
//    Edge *p1 = active->next, *p2;
//    int i;
//    while(p1)
//    {
//        p2 = p1->next;
//        for(i = p1->xIntersect; i < p2->xIntersect; i++) drawUserPoint(i, scan, defaultFillColor);
//    }
//}

void ScanlineFillHelper::deleteAfter(Edge *q)
{
    // if(!q || !q->next) return;

    Edge *p = q->next;
    q->next = p->next;
    delete p;
}

void ScanlineFillHelper::updateActiveList(int scan, Edge *active)
{
    // Delete completed edges, update xIntersect for others
    Edge *q = active;
    Edge *p = active->next;

    while (p)
    {
        if (scan >= p->yUpper)
        {
            p = p->next;
            deleteAfter(q);
        }
        else
        {
            p->xIntersect = p->xIntersect + p->dxPerScan;
            q = p;
            p = p->next;
        }
    }
}

void ScanlineFillHelper::resortActiveList(Edge *active)
{
    Edge *p = active->next;
    active->next = NULL;
    while (p)
    {
        Edge *q = p->next;
        insertEdge(active, p);
        p = q;
    }
}

