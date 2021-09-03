#ifndef EDGE_H
#define EDGE_H

#include <mainwindow.h>
#include <vertex.h>

#include <QMainWindow>
#include <QGraphicsItem>
#include <QVector>
#include <QPointF>
#include <QGraphicsView>
#include <QWidget>
#include <QStyleOptionGraphicsItem>
#include <QKeyEvent>

class Vertex;
class Edge;
class Graph;


class Edge : public QGraphicsItem {
public:
    //constructor
    Edge(Vertex *source, Vertex *dest, double weight);
    ~Edge();

    //rendering method
    void adjust();

    //basic functions
    Vertex *sourceVertex() const;
    Vertex *destVertex() const;
    double getWeight();
    void setWeight(double w);
    QString getColor();
    void setColor(QString color);

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


private:
    Vertex *source, *dest;
    QPointF sourcePoint;
    QPointF destPoint;
    qreal arrowSize = 7;

    QString color;

    double weight;


};
#endif // EDGE_H
