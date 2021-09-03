#ifndef FRAME_H
#define FRAME_H

#include "graph.h"
#include <QMainWindow>
#include <QtWidgets>

class Frame : public QMainWindow{
    Q_OBJECT
public:
    Frame();

    void createAction();
};

#endif // FRAME_H
