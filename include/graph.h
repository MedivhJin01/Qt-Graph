#ifndef GRAPH_H
#define GRAPH_H

#include <vertex.h>
#include <edge.h>

#include <ctime>
#include <climits>
#include <iomanip>

#include <QMainWindow>
#include <QGraphicsItem>
#include <QVector>
#include <QStack>
#include <QQueue>
#include <QPointF>
#include <QGraphicsView>
#include <QWidget>
#include <QStyleOptionGraphicsItem>
#include <QKeyEvent>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStyle>
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QWidget>
#include <QTableWidget>
#include <QApplication>
#include <QGroupBox>
#include <QToolButton>
#include <QMenu>
#include <QDir>
#include <QSaveFile>
#include <QFileDialog>


#include <windows.h>
#include <conio.h>
#include <process.h>
#include "Psapi.h"
#include <TlHelp32.h>
#include <assert.h>
#include <stdio.h>
#include <sstream>



typedef enum nvmlReturn_enum{
    NVML_SUCCESS = 0,                   //!< The operation was successful
    NVML_ERROR_UNINITIALIZED = 1,       //!< NVML was not first initialized with nvmlInit()
    NVML_ERROR_INVALID_ARGUMENT = 2,    //!< A supplied argument is invalid
    NVML_ERROR_NOT_SUPPORTED = 3,       //!< The requested operation is not available on target device
    NVML_ERROR_NO_PERMISSION = 4,       //!< The current user does not have permission for operation
    NVML_ERROR_ALREADY_INITIALIZED = 5, //!< Deprecated: Multiple initializations are now allowed through ref counting
    NVML_ERROR_NOT_FOUND = 6,           //!< A query to find an object was unsuccessful
    NVML_ERROR_INSUFFICIENT_SIZE = 7,   //!< An input argument is not large enough
    NVML_ERROR_INSUFFICIENT_POWER = 8,  //!< A device's external power cables are not properly attached
    NVML_ERROR_DRIVER_NOT_LOADED = 9,   //!< NVIDIA driver is not loaded
    NVML_ERROR_TIMEOUT = 10,            //!< User provided timeout passed
    NVML_ERROR_UNKNOWN = 999            //!< An internal driver error occurred
} nvmlReturn_t;

typedef long long           int64_t;
typedef unsigned long long  uint64_t;

typedef void *nvmlDevice_t;
typedef struct nvmlMemory_st{
    uint64_t total;
    uint64_t free;
    uint64_t user;
} nvmlMemory_t;
typedef struct nvmlUtilization_st{
    unsigned int gpu;
    unsigned int memory;
} nvmlUtilization_t;

class vertex;
class Edge;
class Graph;


#define M INT_MAX

class Graph : public QGraphicsView {
    Q_OBJECT

public:
    Graph(QWidget *parent = nullptr);
    void itemMoved();
    int getVertexNum();

protected:
    //calculate animation
    void timerEvent(QTimerEvent *event) override;

    //create overview
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void createTabWidget(const QRectF &rect);
    void createMenuBar();
    void scaleView(qreal scaleFactor);

    //basic funcitons
    void updateIndex();
    void insertVertex();
    void setVertexPos(int vertexIndex, double x, double y);
    void eraseVertex(int vertexIndex);
    void insertEdge(int source, int dest, double weight);
    void updateWeight(int source, int dest, double weight);
    void eraseEdge(int source, int dest);
    bool checkAdjacent(int source, int dest);
    int getDegree(int vertexIndex);
    void clear();
    void checkConnected();

    void reset();

    //info functions
    void getVertexInfo(int vertexIndex);
    void getGraphInfo();

    //depth first search implementation
    void dfsUtil(int vertexIndex, QVector<bool> &visited);
    void dfs(int vertexIndex);

    //breadth first search implementation

    void bfsUtil(int vertexIndex, QVector<bool> &visited);
    void bfs(int vertexIndex);

    //minimum spanning tree implementation
    void mst(int vertexIndex);

    //floyd warshall algorithm implementation
    void printPath(QVector<QVector<int>> path, int v, int u);
    void printSolution(QVector<QVector<int>> path, int src, int dest);
    void floydWarshall(QVector<QVector<double>> matrix, int src, int dest);


    //delay function
    void delay(int time);

    //cpu function
    static uint64_t file_time_2_utc(const FILETIME* ftime);
    static int get_processor_number();
    int get_cpu_usage();
    uint64_t get_memory_usage();

    //dialog boxes
    void createEraseVertexWindow();
    void createSetVertexPosWindow();
    void createVertexInfoWindow();
    void createAddEdgeWindow();
    void createEraseEdgeWindow();
    void createUpdateWeightWindow();
    void createDFSWindow();
    void createBFSWindow();
    void createMSTWindow();
    void createShortestPathWindow();


    //creating signals
private slots:
    void eraseVertexSignal();
    void setVertexPosSignal();
    void vertexInfoSignal();
    void addEdgeSignal();
    void eraseEdgeSignal();
    void updateWeightSignal();
    void dfsSignal();
    void bfsSignal();
    void mstSignal();
    void shortestPathSignal();

    //read and write functions
    void open();
    bool save();
    bool saveAs();
    void tour();
    void about();


    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
//    void saveAS();

private:
    int edgeNum;
    int vertexNum;
    double MST;
    bool connected;

    QVector<int> DFS;
    QVector<int> BFS;
    QVector<int> optimalPath;


    QVector<Vertex *> vertexList;
    QGraphicsScene *scene;
    int timerId = 0;

    QWidget *window;
    QLineEdit *input1;
    QLineEdit *input2;
    QLineEdit *input3;

    QString curFile;

};

#endif // GRAPH_H
