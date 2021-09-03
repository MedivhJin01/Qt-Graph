#include "graph.h"

Graph::Graph(QWidget *parent) : QGraphicsView(parent){
    scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-400, -450, 800, 900);

    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(800, 900);
    setWindowTitle(tr("ECE250 Extended Project"));
    scene->addText("Copyright © 2021 Haoning Jin ECE250 Extended Project. All rights reserved.")->setPos(QPointF(-40, 450));
    createMenuBar();

    MST = 0;
    connected = true;
    DFS.clear();
    BFS.clear();
}

//test functions


uint64_t Graph::file_time_2_utc(const FILETIME* ftime){
    LARGE_INTEGER li;
    assert(ftime);
    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}

int Graph::get_processor_number(){
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return (int)info.dwNumberOfProcessors;
}

int Graph::get_cpu_usage(){
    static int processor_count = -1;
    static int64_t last_time = 0;
    static int64_t last_system_time = 0;

    FILETIME now;
    FILETIME creation_time, exit_time, kernel_time, user_time;
    int64_t system_time;

    int64_t time;
    int64_t system_time_delta;
    int64_t time_delta;

    int cpu = -1;

    if(processor_count == -1){
        processor_count = get_processor_number(); //get number of processors
    }

    GetSystemTimeAsFileTime(&now);
    time = file_time_2_utc(&now);

    if(!GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time, &kernel_time, &user_time)){
        return -1;
    }

    system_time = (file_time_2_utc(&kernel_time) + file_time_2_utc(&user_time)) / processor_count;


    if((last_system_time == 0) || (last_time == 0)){
        last_system_time = system_time;
        last_time = time;
        return -1;
    }
    system_time_delta = system_time - last_system_time;
    time_delta = time - last_time;

    assert(time_delta != 0);
    if(time_delta == 0){
        return -1;
    }
    cpu = (int)((system_time_delta * 100 + time_delta / 2) / time_delta);
    last_system_time = system_time;
    last_time = time;
    return cpu;
}

uint64_t Graph::get_memory_usage(){
    PROCESS_MEMORY_COUNTERS pmc;
    if(GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof (pmc))){
        return pmc.WorkingSetSize/1024/1024;
    }

    return -1;
}
//

void Graph::itemMoved(){
    if(!timerId){
        timerId = startTimer(1000 / 25);
    }
}

void Graph::timerEvent(QTimerEvent *event){
    Q_UNUSED(event);
//    QVector<Vertex *> nodes;
    const QList<QGraphicsItem *> items = scene -> items();
    for(Vertex *temp : qAsConst(vertexList)){
        temp->calculateForces();
    }
    bool itemsMoved = false;
    for(Vertex *temp : qAsConst(vertexList)){
        if(temp->advancePosition()){
            itemsMoved = true;
        }
    }

    if(!itemsMoved){
        killTimer(timerId);
        timerId = 0;
    }
}

void Graph::drawBackground(QPainter *painter, const QRectF &rect){
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect)){
        painter->fillRect(rightShadow, Qt::darkGray);
    }

    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect)){
        painter->fillRect(bottomShadow, Qt::darkGray);
    }

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);
    // Text
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + sceneRect.height()/9 + 35, sceneRect.width() - 4, 20);
    QString message(tr("Click and drag the nodes around"));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), Qt::AlignCenter, message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, Qt::AlignCenter, message);

    // Tools
    createTabWidget(rect);


}

void Graph::createMenuBar(){
    QToolButton *fileButton = new QToolButton();
    QRectF sceneRect = this -> sceneRect();
    QFont textfont = QFont();
    textfont.setPointSize(11);

    fileButton->setText(tr("&File"));
    fileButton->setStyleSheet("QToolButton:selected, QToolButton:hover{background-color: lightBlue; color: darkBlue;} "
                             "QToolButton:pressed{background-color: lightBlue;} "
                             "QToolButton{background-color: white; border: none;} "
                             "QToolButton::menu-indicator{image: none;}");
    fileButton->setFont(textfont);
    fileButton->setGeometry(sceneRect.left() + 1, sceneRect.top() + 1, 90, 25);

    QMenu *fileMenu = new QMenu;
    fileMenu->setFixedWidth(200);
    QAction *newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    connect(newAct, &QAction::triggered, this, &Graph::clear);
    fileMenu->addAction(newAct);

    QAction *openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &Graph::open);
    fileMenu->addAction(openAct);

    QAction *saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &Graph::save);
    fileMenu->addAction(saveAct);

    QAction *saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    connect(saveAsAct, &QAction::triggered, this, &Graph::saveAs);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();

    QAction *exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, &QAction::triggered, qApp, &QApplication::quit);
    fileMenu->addAction(exitAct);

    fileButton->setPopupMode(QToolButton::InstantPopup);
    fileButton->setMenu(fileMenu);
    scene->addWidget(fileButton);

    QToolButton *helpButton = new QToolButton();
    helpButton->setText(tr("&Help"));
    helpButton->setStyleSheet("QToolButton:selected, QToolButton:hover{background-color: lightBlue; color: darkBlue;} "
                             "QToolButton:pressed{background-color: lightBlue;} "
                             "QToolButton{background-color: white; border: none;}"
                             "QToolButton::menu-indicator{image: none;}");
    helpButton->setFont(textfont);
    helpButton->setGeometry(sceneRect.left() + 100, sceneRect.top() + 1, 90, 25);

    QMenu *helpMenu = new QMenu;
    helpMenu->setFixedWidth(120);

    QAction *tourAct = new QAction(tr("UI Tour"), this);
    connect(tourAct, &QAction::triggered, this, &Graph::tour);
    helpMenu->addAction(tourAct);

    QAction *aboutAct = new QAction(tr("About Us"), this);
    connect(aboutAct, &QAction::triggered, this, &Graph::about);
    helpMenu->addAction(aboutAct);

    helpButton->setPopupMode(QToolButton::InstantPopup);
    helpButton->setMenu(helpMenu);

    scene->addWidget(helpButton);
}

void Graph::createTabWidget(const QRectF &rect){
    Q_UNUSED(rect);

    //Geometery and positioning
    QRectF sceneRect = this -> sceneRect();

    QTabWidget *tab = new QTabWidget();
    tab->setGeometry(sceneRect.left() + 1, sceneRect.top() + 26, sceneRect.width() - 1, sceneRect.height()/9);

    //Tabwidgets
    QWidget *vertexTab = new QWidget;
    QWidget *edgeTab = new QWidget;
    QWidget *graphTab = new QWidget;
    QWidget *aboutTab = new QWidget;

    //Tabbar view
    tab->setAttribute(Qt::WA_StyledBackground);

    //Tabbar font size
    QFont textfont = QFont();
    textfont.setPointSize(12);
    tab -> setFont(textfont);
    tab->setIconSize(QSize(20, 25));

    tab->addTab(vertexTab, QIcon(":/images/icon"), tr("Vertex"));
    tab->addTab(edgeTab, QIcon(":/images/icon"), tr("Edge"));
    tab->addTab(graphTab, QIcon(":/images/icon"), tr("Graph"));
    tab->addTab(aboutTab, QIcon(":/images/icon"), tr("About"));

    QHBoxLayout *vertexTabLayout = new QHBoxLayout;
    QPushButton *addVertexButton = new QPushButton();
    addVertexButton->setText(tr("Add Vertex"));
    addVertexButton->setFont(textfont);
    addVertexButton->setMaximumSize(QSize(120, 35));
    addVertexButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                                   "QPushButton:pressed{background-color: lightBlue;} "
                                   "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(addVertexButton, &QPushButton::clicked, this, &Graph::insertVertex);

    QPushButton *eraseVertexButton = new QPushButton();
    eraseVertexButton->setText(tr("Erase Vertex"));
    eraseVertexButton->setFont(textfont);
    eraseVertexButton->setMaximumSize(QSize(130, 35));
    eraseVertexButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                                   "QPushButton:pressed{background-color: lightBlue;} "
                                   "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");

    connect(eraseVertexButton, &QPushButton::clicked, this, &Graph::createEraseVertexWindow);

    QPushButton *setVertexPosButton = new QPushButton();
    setVertexPosButton->setText(tr("Set Position"));
    setVertexPosButton->setFont(textfont);
    setVertexPosButton->setMaximumSize(QSize(125, 35));
    setVertexPosButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                                      "QPushButton:pressed{background-color: lightBlue;} "
                                      "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");

    connect(setVertexPosButton, &QPushButton::clicked, this, &Graph::createSetVertexPosWindow);

    QPushButton *vertexInfoButton = new QPushButton();
    vertexInfoButton->setText(tr("Vertex Info"));
    vertexInfoButton->setFont(textfont);
    vertexInfoButton->setMaximumSize(QSize(120, 35));
    vertexInfoButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                                   "QPushButton:pressed{background-color: lightBlue;} "
                                   "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");

    connect(vertexInfoButton, &QPushButton::clicked, this, &Graph::createVertexInfoWindow);

    vertexTabLayout->addWidget(addVertexButton);
    vertexTabLayout->addWidget(eraseVertexButton);
    vertexTabLayout->addWidget(setVertexPosButton);
    vertexTabLayout->addWidget(vertexInfoButton);

    QHBoxLayout *edgeTabLayout = new QHBoxLayout;
    QPushButton *addEdgeButton = new QPushButton();
    addEdgeButton->setText(tr("Add Edge"));
    addEdgeButton->setFont(textfont);
    addEdgeButton->setMaximumSize(QSize(120, 35));
    addEdgeButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                                 "QPushButton:pressed{background-color: lightBlue;} "
                                 "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(addEdgeButton, &QPushButton::clicked, this, &Graph::createAddEdgeWindow);

    QPushButton *updateWeightButton = new QPushButton();
    updateWeightButton->setText(tr("Update Weight"));
    updateWeightButton->setFont(textfont);
    updateWeightButton->setMaximumSize(QSize(150, 35));
    updateWeightButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                                  "QPushButton:pressed{background-color: lightBlue;} "
                                  "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(updateWeightButton, &QPushButton::clicked, this, &Graph::createUpdateWeightWindow);



    QPushButton *eraseEdgeButton = new QPushButton();
    eraseEdgeButton->setText(tr("Erase Edge"));
    eraseEdgeButton->setFont(textfont);
    eraseEdgeButton->setMaximumSize(QSize(130, 35));
    eraseEdgeButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                                 "QPushButton:pressed{background-color: lightBlue;} "
                                 "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(eraseEdgeButton, &QPushButton::clicked, this, &Graph::createEraseEdgeWindow);

    edgeTabLayout->addWidget(addEdgeButton);
    edgeTabLayout->addWidget(updateWeightButton);
    edgeTabLayout->addWidget(eraseEdgeButton);

    QHBoxLayout *graphTabLayout = new QHBoxLayout;

    QToolButton *functionButton = new QToolButton();
    functionButton->setText(tr("Functions"));
    functionButton->setFont(textfont);
    functionButton->setMaximumSize(QSize(120, 35));
    functionButton->setStyleSheet("QToolButton:selected, QToolButton:hover{color: darkBlue;} "
                             "QToolButton:pressed{background-color: lightBlue;} "
                             "QToolButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    QMenu *menu = new QMenu;
    menu->setFixedWidth(200);

    QAction *dfs = new QAction(tr("Depth First Search"), this);
    dfs->setStatusTip(tr("Depth First Search"));
    connect(dfs, &QAction::triggered, this, &Graph::createDFSWindow);
    menu->addAction(dfs);
    menu->addSeparator();

    QAction *bfs = new QAction(tr("Breadth First Search"), this);
    bfs->setStatusTip(tr("Breadth First Search"));
    connect(bfs, &QAction::triggered, this, &Graph::createBFSWindow);
    menu->addAction(bfs);
    menu->addSeparator();

    QAction *mst = new QAction(tr("Minimum Spanning Tree"), this);
    mst->setStatusTip(tr("Minimum Spanning Tree"));
    connect(mst, &QAction::triggered, this, &Graph::createMSTWindow);
    menu->addAction(mst);
    menu->addSeparator();

    QAction *floydWarshall = new QAction(tr("Optimal Path"), this);
    mst->setStatusTip(tr("Floyd Warshall Algorithm"));
    connect(floydWarshall, &QAction::triggered, this, &Graph::createShortestPathWindow);
    menu->addAction(floydWarshall);
    menu->addSeparator();

    functionButton->setPopupMode(QToolButton::InstantPopup);
    functionButton->setMenu(menu);

    QPushButton *resetButton = new QPushButton();
    resetButton->setText(tr("Reset"));
    resetButton->setFont(textfont);
    resetButton->setMaximumSize(QSize(90, 35));
    resetButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                             "QPushButton:pressed{background-color: lightBlue;} "
                             "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(resetButton, &QPushButton::clicked, this, &Graph::reset);

    QPushButton *clearButton = new QPushButton();
    clearButton->setText(tr("Clear"));
    clearButton->setFont(textfont);
    clearButton->setMaximumSize(QSize(100, 35));
    clearButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                             "QPushButton:pressed{background-color: lightBlue;} "
                             "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(clearButton, &QPushButton::clicked, this, &Graph::clear);

    QPushButton *graphInfoButton = new QPushButton();
    graphInfoButton->setText(tr("Graph Info"));
    graphInfoButton->setFont(textfont);
    graphInfoButton->setMaximumSize(QSize(110, 35));
    graphInfoButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                             "QPushButton:pressed{background-color: lightBlue;} "
                             "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(graphInfoButton, &QPushButton::clicked, this, &Graph::getGraphInfo);

    graphTabLayout->addWidget(functionButton);
    graphTabLayout->addWidget(resetButton);
    graphTabLayout->addWidget(clearButton);
    graphTabLayout->addWidget(graphInfoButton);

    vertexTab->setLayout(vertexTabLayout);
    edgeTab->setLayout(edgeTabLayout);
    graphTab->setLayout(graphTabLayout);
//    aboutTab->setLayout(otherTabLayout);

    scene->addWidget(tab);

}

void Graph::open(){
    QString fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty()){
        loadFile(fileName);
    }
}

bool Graph::save(){
    if(curFile.isEmpty()){
        return saveAs();
    }
    return saveFile(curFile);
}

bool Graph::saveAs(){
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if(dialog.exec() != QDialog::Accepted){
        return false;
    }
    return saveFile(dialog.selectedFiles().first());
}

void Graph::tour(){
    QMessageBox::about(this, tr("UI Tour"), tr("<div><b>Vertex</b> -> Insert Vertex: add a vertex to the graph</div>"
                                               "<div>              -> Erase Vertex: erase a vertex with the given index</div>"
                                               "<div>              -> Set Position: set a vertex's geometric position</div>"
                                               "<div>              -> Get Info: get all the information of a vertex</div>"
                                               "<div><b>Edge</b>   -> Insert Edge: add a edge between two vertices with a given weight</div>"
                                               "<div>              -> Update Weight: udpate the weight between two vertices</div>"
                                               "<div>              -> Erase Edge: erase the edge between two vertices</div>"
                                               "<div><b>Graph</b>  -> Functions: various graph algorithms can be found here</div>"
                                               "<div>              -> Reset: reset all the vertices and edges</div>"
                                               "<div>              -> Clear: clear all the vertices and edges</div>"
                                               "<div>              -> Graph Info: get all the possible information of the graph</div>"
                                               ));
}

void Graph::about(){
    QMessageBox::about(this, tr("About Us"), tr("<div>The <b>Applicatoin</b> is an extension on ECE 250 Project3, which "
                                                "visualizes graph data structure and related algorithms using Qt. "
                                                "Users are allowed to load and save graphs they created. "
                                                "Various functionalities are implemented for basic graph manipulations. "
                                                "Moreover, algorithms are designed dynamically to help users better understand "
                                                "the backend logistics.</div> "
                                                "<div>Copyright © 2021 Haoning Jin ECE250 Extended Project. All rights reserved.</div>"));
}

void Graph::loadFile(const QString &fileName){
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, tr("Application"), tr("Cannot read file %1:\n%2.").arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }
    QTextStream in(&file);

#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    clear();
    while(!in.atEnd()){
        QString line = in.readLine();
        if(line.front() == "v"){
            int pos = line.indexOf(" ");
            QString num = line.mid(pos + 1);

            for(int i{0}; i < num.toInt(); i++){
                insertVertex();
            }
        }

        if(line.front() == "p"){
            QString sub = line.mid(2);
            int pos1 = sub.indexOf(" ");
            QString src = sub.mid(0, pos1);

            QString sub2 = sub.mid(pos1 + 1);
            int pos2 = sub2.indexOf(" ");
            QString x = sub2.mid(0, pos2);
            QString y = sub2.mid(pos2 + 1);

            setVertexPos(src.toInt(), x.toDouble(), y.toDouble());
        }

        else if(line.front() == "e"){
            QString sub = line.mid(2);
            int pos1 = sub.indexOf(" ");
            QString src = sub.mid(0, pos1);

            QString sub2 = sub.mid(pos1 + 1);
            int pos2 = sub2.indexOf(" ");
            QString dest = sub2.mid(0, pos2);
            QString weight = sub2.mid(pos2 + 1);

            insertEdge(src.toInt(), dest.toInt(), weight.toDouble());
        }
    }
    QGuiApplication::restoreOverrideCursor();
    setCurrentFile(fileName);
}


bool Graph::saveFile(const QString &fileName){
    QString errorMessage;
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if(file.open(QFile::WriteOnly | QFile::Text)){
        QTextStream out(&file);
        out << "v" << " " << vertexNum << "\n";
        for(int i{0}; i < vertexNum; i++){
            out << "p" << " " << i << " " << vertexList[i]->getPos().x() << " " << vertexList[i]->getPos().y() << "\n";
        }
        for(int j{0}; j < vertexNum; j++){
            for(Edge *edge : vertexList[j]->getEdges()){
                out << "e" << " " << j << " " << edge->destVertex()->getIndex() << " " << edge->getWeight() << "\n";
            }
        }

        if(!file.commit()){
            errorMessage = tr("Cannot write file %1:\n").arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.").arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if(!errorMessage.isEmpty()){
        QMessageBox::warning(this, tr("Application"), errorMessage);
        return false;
    }
//    setCurrentFile(fileName);
    return true;
}

void Graph::setCurrentFile(const QString &fileName){
    curFile = fileName;
    QString shownName = curFile;
    if(curFile.isEmpty()){
        shownName = "untitled.txt";
    }
    setWindowFilePath(shownName);
}


int Graph::getVertexNum(){
    return vertexList.size();
}

void Graph::scaleView(qreal scaleFactor){
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if(factor < 0.07 || factor > 100){
        return;
    }
    scale(scaleFactor, scaleFactor);
}

void Graph::insertVertex(){
    int vertexIndex = vertexList.size();
    vertexList.append(new Vertex(this, vertexIndex));
    vertexList[vertexIndex] -> setPos(0, 50);
    scene->addItem(vertexList[vertexIndex]);
    vertexNum++;
}

void Graph::updateIndex(){
    for(int i{0}; i < vertexList.size(); i++){
        vertexList[i] -> setIndex(i);
    }
}

void Graph::setVertexPos(int vertexIndex, double x, double y){
    Vertex *temp = vertexList[vertexIndex];
    temp->setPos(QPointF(x, y));
}

void Graph::eraseVertex(int vertexIndex){
    Vertex *temp = vertexList[vertexIndex];
    for(Edge *edge : temp->getEdges()){
        eraseEdge(vertexIndex, edge->destVertex()->getIndex());
    } 
    temp->clearEdge();
    scene->removeItem(temp);
    edgeNum -= vertexList[vertexIndex]->getEdges().size()*2;
    vertexList.erase(vertexList.begin() + vertexIndex);
    updateIndex();
    vertexNum--;
}

void Graph::getVertexInfo(int vertexIndex){
    QTableWidget *infoWindow = new QTableWidget;
    Vertex *temp = vertexList[vertexIndex];
    int vertexDegree = temp->getDegree();

    infoWindow->setRowCount(vertexDegree + 5);
    infoWindow->setColumnCount(2);
    infoWindow->setItem(0, 0, new QTableWidgetItem("Property"));
    infoWindow->setItem(0, 1, new QTableWidgetItem("Value"));

    infoWindow->setItem(1, 0, new QTableWidgetItem("Vertex Index"));
    infoWindow->setItem(1, 1, new QTableWidgetItem(QString::number(vertexIndex)));

    infoWindow->setItem(2, 0, new QTableWidgetItem("Coordinate"));
    QString x = QString::number(temp->getPos().x());
    QString y = QString::number(temp->getPos().y());
    QString coordinate = "X: " + x + " ; y: " + y;
    infoWindow->setItem(2, 1, new QTableWidgetItem(coordinate));

    infoWindow->setItem(3, 0, new QTableWidgetItem("Visited"));
    bool visited = false;
    if(vertexList[vertexIndex]->getColor() == "black"){
        visited = true;
    }
    infoWindow->setItem(3, 1, new QTableWidgetItem(QString::number(visited)));

    infoWindow->setItem(4, 0, new QTableWidgetItem("Vertex Degree"));
    infoWindow->setItem(4, 1, new QTableWidgetItem(QString::number(vertexDegree)));

    infoWindow->setItem(5, 0, new QTableWidgetItem("Adjacent Vertices"));
    int rowCounter = 5;
    for(Edge *edge : vertexList[vertexIndex]->getEdges()){
        QString adjacentVertex = QString::number(vertexIndex) + " -> " + QString::number(edge->destVertex()->getIndex()) + " ; weight: " + QString::number(edge->getWeight());
        infoWindow->setItem(rowCounter, 1, new QTableWidgetItem(adjacentVertex));
        rowCounter++;
    }
    infoWindow->setWindowTitle(tr("Vertex Information Window"));
    infoWindow->setColumnWidth(0, 140);
    infoWindow->setColumnWidth(1, 220);
    infoWindow->setFixedSize(QSize(380, 400));
    infoWindow->show();
}

void Graph::getGraphInfo(){   
    int rows = vertexList.size() + 8;
    int cols = vertexList.size() + 1;
    QTableWidget *infoWindow = new QTableWidget(rows, cols);
    infoWindow->setItem(0, 0, new QTableWidgetItem("Vertex Number"));
    infoWindow->setItem(0, 1, new QTableWidgetItem(QString::number(vertexNum)));
    infoWindow->setItem(1, 0, new QTableWidgetItem("Edge Number"));
    infoWindow->setItem(1, 1, new QTableWidgetItem(QString::number(edgeNum)));
    infoWindow->setItem(2, 0, new QTableWidgetItem("Minimum Spanning Tree"));
    infoWindow->setItem(2, 1, new QTableWidgetItem(QString::number(MST)));
    infoWindow->setItem(3, 0, new QTableWidgetItem("Connected Graph"));
    infoWindow->setItem(3, 1, new QTableWidgetItem(QString::number(connected)));


    QString dfs = "Depth First Search at index ";
    if(!DFS.empty()){
        dfs += QString::number(DFS[0]);
        for(int i{0}; i < DFS.size(); i++){
            infoWindow->setItem(4, i+1, new QTableWidgetItem(QString::number(DFS[i])));
        }
    }
    infoWindow->setItem(4, 0, new QTableWidgetItem(dfs));

    QString bfs = "Breadth First Search at index ";
    if(!BFS.empty()){
        bfs += QString::number(BFS[0]);
        for(int i{0}; i < BFS.size(); i++){
            infoWindow->setItem(5, i+1, new QTableWidgetItem(QString::number(BFS[i])));
        }
    }
    infoWindow->setItem(5, 0, new QTableWidgetItem(bfs));

    QString path = "Optimal Path: ";
    if(!optimalPath.empty()){
        path += QString::number(optimalPath.front()) + " -> " + QString::number(optimalPath.last());
        for(int i{0}; i < optimalPath.size(); i++){
            infoWindow->setItem(6, i+1, new QTableWidgetItem(QString::number(optimalPath[i])));
        }
    }
    infoWindow->setItem(6, 0, new QTableWidgetItem(path));

    infoWindow->setItem(7, 0, new QTableWidgetItem("Graph Matrix"));
    infoWindow->item(7, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    for(int i{0}; i < vertexNum; i++){
        infoWindow->setItem(7, i+1, new QTableWidgetItem(QString::number(i)));
        infoWindow->item(7, i+1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    for(int i{0}; i < vertexNum; i++){
        infoWindow->setItem(8+i, 0, new QTableWidgetItem(QString::number(i)));
        infoWindow->item(8+i, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        for(int j{0}; j < vertexNum; j++){
            infoWindow->setItem(8+i, 1+j, new QTableWidgetItem("0"));
            infoWindow->item(8+i, 1+j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
        for(Edge *edge : vertexList[i]->getEdges()){
            int pos = edge->destVertex()->getIndex();
            infoWindow->setItem(8+i, 1+pos, new QTableWidgetItem(QString::number(edge->getWeight())));
            infoWindow->item(8+i, 1+pos)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
        infoWindow->setColumnWidth(i+1, 30);
    }
    infoWindow->setWindowTitle("Graph Information Window");
    infoWindow->setColumnWidth(0, 200);
    infoWindow->setFixedSize(QSize(600, 700));
    infoWindow->show();
}

void Graph::insertEdge(int source, int dest, double weight){
    if(checkAdjacent(source, dest)){
        return;
    } else {
        scene->addItem(new Edge(vertexList[source], vertexList[dest], weight));
        scene->addItem(new Edge(vertexList[dest], vertexList[source], weight));
        edgeNum += 2;
    }
}

void Graph::updateWeight(int source, int dest, double weight){
    if(!checkAdjacent(source, dest)){
        return;
    }
    vertexList[source]->updateWeight(dest, weight);
    vertexList[dest]->updateWeight(source, weight);
}

void Graph::eraseEdge(int source, int dest){
    if(!checkAdjacent(source, dest)){
        scene->addText(tr("not connected"));
        return;
    }
    for(Edge *edge : vertexList[source]->getEdges()){
        if(edge -> destVertex() ->getIndex() == dest){
            vertexList[source]->eraseEdge(dest);
            scene->removeItem(edge);
        }
    }
    for(Edge *edge : vertexList[dest]->getEdges()){
        if(edge -> destVertex() ->getIndex() == source){
            vertexList[dest]->eraseEdge(source);
            scene->removeItem(edge);
        }
    }
    edgeNum -= 2;
}

bool Graph::checkAdjacent(int source, int dest){
    return vertexList[source] -> pathExist(dest);
}

int Graph::getDegree(int vertexIndex){
    return vertexList[vertexIndex]->getDegree();
}

void Graph::clear(){
    while(!vertexList.empty()){
        eraseVertex(0);
    }
}

void Graph::checkConnected(){
    connected = true;
    for(Vertex *vertex : vertexList){
        bool visited = false;
        if(vertex->getColor() == "black"){
            visited = true;
        }
        connected &= visited;
    }
}

void Graph::reset(){
    if(vertexList.size() == 0){
        return;
    }
    for(int i{0}; i < vertexList.size(); i++){
        vertexList[i] -> setColor("white");
        for(Edge *edge : vertexList[i]->getEdges()){
            edge -> setColor("white");
        }
    }
}

void Graph::dfsUtil(int vertexIndex, QVector<bool> &visited){
    QStack<int> dfsStack;
    dfsStack.push(vertexIndex);
    while(!dfsStack.empty()){
        int temp = dfsStack.top();
        Vertex *tempVertex = vertexList[temp];
        dfsStack.pop();
        if(!visited[temp]){
            visited[temp] = true;
            vertexList[temp]->setColor("black");
            DFS.push_back(temp);
            delay(50);
        }
        for(Edge *tempEdge : tempVertex->getEdges()){
            if(!visited[tempEdge -> destVertex() -> getIndex()]){
                tempEdge->destVertex()->setColor("gray");
                delay(70);
                dfsStack.push(tempEdge->destVertex()->getIndex());
            }
        }
    }
}

void Graph::dfs(int vertexIndex){
    DFS.clear();
    QVector<bool> visited(vertexList.size(), false);
    dfsUtil(vertexIndex, visited);
    checkConnected();
}

void Graph::bfsUtil(int vertexIndex, QVector<bool> &visited){
    QQueue<int> bfsQueue;
    bfsQueue.push_back(vertexIndex);
    while(!bfsQueue.empty()){
        int temp = bfsQueue.front();

        Vertex *tempVertex = vertexList[temp];
        bfsQueue.pop_front();
        if(!visited[temp]){
            visited[temp] = true;
            tempVertex->setColor("black");
            BFS.push_back(temp);
            delay(50);
        }
        for(Edge *tempEdge : tempVertex->getEdges()){
            if(!visited[tempEdge -> destVertex() -> getIndex()]){
                tempEdge->destVertex()->setColor("gray");
                bfsQueue.push_back(tempEdge->destVertex()->getIndex());
                delay(70);
            }
        }
    }
}

void Graph::bfs(int vertexIndex){
    BFS.clear();
    QVector<bool> visited(vertexList.size(), false);
    bfsUtil(vertexIndex, visited);
    checkConnected();
}

void Graph::mst(int vertexIndex){
    QVector<bool> vertexMark = QVector<bool>(vertexList.size(), false);
    QVector<Edge *> edgeSet;
    QQueue<int> vertexSet;
    double mst = 0;
    vertexSet.push_back(vertexIndex);
    int tempIndex;

    while(!vertexSet.empty()){
        tempIndex = vertexSet.front();
        vertexSet.pop_front();
        vertexMark[tempIndex] = true;
        vertexList[tempIndex] -> setColor("black");
        delay(50);

        bool breakCondition = true;
        for(bool tempMark : vertexMark){
            breakCondition &= tempMark;
        }
        if(breakCondition){
            break;
        }

        for(Edge *tempEdge : vertexList[tempIndex]->getEdges()){
            if(!vertexMark[tempEdge->destVertex()->getIndex()]){
                edgeSet.append(tempEdge);
                tempEdge->destVertex()->setColor("gray");
                delay(70);
            }
        }

        int edgePos = 0;
        for(int i{0}; i < edgeSet.size(); i++){
            if(!vertexMark[edgeSet[i]->destVertex()->getIndex()]){
                edgePos = i;
            }
        }
        Edge *minWeight = edgeSet[edgePos];

        for(int i = 0; i < edgeSet.size(); i++){
            if(edgeSet[i]->getWeight() < minWeight->getWeight() && !vertexMark[edgeSet[i]->destVertex()->getIndex()]){
                minWeight = edgeSet[i];
                edgePos = i;
            }
        }
        minWeight->setColor("black");
        vertexList[minWeight->destVertex()->getIndex()]->changeEdgeColor(minWeight->sourceVertex()->getIndex());

        if(!vertexMark[minWeight->destVertex()->getIndex()]){
            vertexSet.append(minWeight->destVertex()->getIndex());
        }
        edgeSet.erase(edgeSet.begin() + edgePos);
        mst += minWeight -> getWeight();
    }
    MST = mst;
    checkConnected();
}

void Graph::printPath(QVector<QVector<int> > path, int v, int u){
    if(path[v][u] == v){
        return;
    }
    printPath(path, v, path[v][u]);
    optimalPath.push_back(path[v][u]);
}

void Graph::floydWarshall(QVector<QVector<double>> matrix, int src, int dest){
    QVector<QVector<double>> cost;
    QVector<QVector<int>> path;

    for(int v{0}; v < vertexNum; v++){
        QVector<double> tempCost;
        QVector<int> tempPath;
        for(int u{0}; u < vertexNum; u++){
            tempCost.push_back(matrix[v][u]);
            if(v == u){
                tempPath.push_back(0);
            }
            else if(matrix[v][u] != INT_MAX){
                tempPath.push_back(v);
            }
            else{
                tempPath.push_back(-1);
            }
        }
        cost.push_back(tempCost);
        path.push_back(tempPath);
    }

    for(int k{0}; k < vertexNum; k++){
        for(int i{0}; i < vertexNum; i++){
            for(int j{0}; j < vertexNum; j++){
                if(cost[i][k] != INT_MAX && cost[k][j] != INT_MAX && cost[i][k] + cost[k][j] < cost[i][j]){
                    cost[i][j] = cost[i][k] + cost[k][j];
                    path[i][j] = path[k][j];
                }
            }
        }
    }
    optimalPath.push_back(src);
    printPath(path, src, dest);
    optimalPath.push_back(dest);

    for(int i{0}; i < optimalPath.size() - 1; i++){
        vertexList[optimalPath[i]]->setColor("black");
        for(Edge *edge : vertexList[i]->getEdges()){
            if(edge->destVertex()->getIndex() == optimalPath[i + 1]){
                edge->setColor("black");
            }
        }
        delay(60);
    }
    vertexList[optimalPath.last()]->setColor("black");
}


void Graph::delay(int time){
    clock_t now = clock();
    while(clock() - now < time);
}

void Graph::createEraseVertexWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Erase Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(okButton, 1, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Erase Vertex Window");

    connect(okButton, &QPushButton::clicked, this, &Graph::eraseVertexSignal);
    window->show();
}

void Graph::eraseVertexSignal(){
    int srcIndex = input1->text().toInt();
    window -> close();
    if(srcIndex >= vertexList.size()){
        return;
    }
    eraseVertex(srcIndex);
}

void Graph::createSetVertexPosWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    input2 = new QLineEdit;
    input3 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));
//    int vertexIndex = input1->text().toInt();

//    input2->setText(QString::number(vertexList[vertexIndex]->getPos().x()));
//    input3->setText(QString::number(vertexList[vertexIndex]->getPos().y()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(new QLabel(tr("X Coordinate")), 1, 0);
    layout->addWidget(input2, 1, 1);
    layout->addWidget(new QLabel(tr("Y Coordinate:")), 2, 0);
    layout->addWidget(input3, 2, 1);
    layout->addWidget(okButton, 3, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Set Vertex Position Window");
    connect(okButton, &QPushButton::clicked, this, &Graph::setVertexPosSignal);
    window->show();
}

void Graph::setVertexPosSignal(){
    int vertexIndex = input1->text().toInt();
    double xvel = input2->text().toDouble();
    double yvel = input3->text().toDouble();
    window->close();

    if(vertexIndex >= vertexList.size()){
        return;
    }
    setVertexPos(vertexIndex, xvel, yvel);
}

void Graph::createVertexInfoWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    QLabel *label = new QLabel;
    label -> setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(okButton, 1, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Vertex Info Window");

    connect(okButton, &QPushButton::clicked, this, &Graph::vertexInfoSignal);
    window->show();
}

void Graph::vertexInfoSignal(){
    int index = input1->text().toInt();
    window->close();
    if(index >= vertexList.size()){
        return;
    }
    getVertexInfo(index);
}

void Graph::createAddEdgeWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    input2 = new QLineEdit;
    input3 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Source Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(new QLabel(tr("Dest Vertex Index:")), 1, 0);
    layout->addWidget(input2, 1, 1);
    layout->addWidget(new QLabel(tr("Edge Weight:")), 2, 0);
    layout->addWidget(input3, 2, 1);
    layout->addWidget(okButton, 3, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Add Edge Window");

    connect(okButton, &QPushButton::clicked, this, &Graph::addEdgeSignal);
    window->show();
}

void Graph::addEdgeSignal(){
    int srcIndex = input1->text().toInt();
    int destIndex = input2->text().toInt();
    double edgeWeight = input3->text().toDouble();
    window->close();
    if(srcIndex >= vertexList.size() || destIndex >= vertexList.size() || edgeWeight <= 0){
        return;
    }
    insertEdge(srcIndex, destIndex, edgeWeight);
}

void Graph::createEraseEdgeWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    input2 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Source Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(new QLabel(tr("Dest Vertex Index:")), 1, 0);
    layout->addWidget(input2, 1, 1);
    layout->addWidget(okButton, 2, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Erase Edge Window");

    connect(okButton, &QPushButton::clicked, this, &Graph::eraseEdgeSignal);
    window->show();
}

void Graph::eraseEdgeSignal(){
    int srcIndex = input1->text().toInt();
    int destIndex = input2->text().toInt();
    window->close();
    if(srcIndex >= vertexList.size() || destIndex >= vertexList.size()){
        return;
    }
    eraseEdge(srcIndex, destIndex);
}

void Graph::createUpdateWeightWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    input2 = new QLineEdit;
    input3 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Source Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(new QLabel(tr("Dest Vertex Index:")), 1, 0);
    layout->addWidget(input2, 1, 1);
    layout->addWidget(new QLabel(tr("Edge Weight:")), 2, 0);
    layout->addWidget(input3, 2, 1);
    layout->addWidget(okButton, 3, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Update Weight Window");

    connect(okButton, &QPushButton::clicked, this, &Graph::updateWeightSignal);
    window->show();
}

void Graph::updateWeightSignal(){
    int srcIndex = input1->text().toInt();
    int destIndex = input2->text().toInt();
    double edgeWeight = input3->text().toDouble();
    window->close();
    if(srcIndex >= vertexList.size() || destIndex >= vertexList.size() || edgeWeight <= 0){
        return;
    }
    updateWeight(srcIndex, destIndex, edgeWeight);
}

void Graph::createDFSWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Starting Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(okButton, 1, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("DFS Window");

    connect(okButton, &QPushButton::clicked, this, &Graph::dfsSignal);
    window->show();
}

void Graph::dfsSignal(){
    int index = input1->text().toInt();
    window->close();
    if(index >= vertexList.size()){
        return;
    }
    dfs(index);
}

void Graph::createBFSWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Starting Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(okButton, 1, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("BFS Window");

    connect(okButton, &QPushButton::clicked, this, &Graph::bfsSignal);
    window->show();
}

void Graph::bfsSignal(){
    int index = input1->text().toInt();
    window->close();
    if(index >= vertexList.size()){
        return;
    }
    bfs(index);
}

void Graph::createMSTWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Starting Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(okButton, 1, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("MST Window");

    connect(okButton, &QPushButton::clicked, this, &Graph::mstSignal);
    window->show();
}

void Graph::mstSignal(){
    int index = input1->text().toInt();
    window->close();
    if(index >= vertexList.size()){
        return;
    }
    mst(index);
}

void Graph::createShortestPathWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    input2 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Source Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(new QLabel(tr("Dest Vertex Index:")), 1, 0);
    layout->addWidget(input2, 1, 1);
    layout->addWidget(okButton, 2, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Shortest Path Window");

    connect(okButton, &QPushButton::clicked, this, &Graph::shortestPathSignal);
    window->show();
}

void Graph::shortestPathSignal(){
    optimalPath.clear();
    int src = input1->text().toInt();
    int dest = input2->text().toInt();
    window->close();
    if(src >= vertexNum || dest >= vertexNum){
        return;
    }
    QVector<QVector<double>> matrix;
    for(int i{0}; i < vertexNum; i++){
        QVector<double> temp;
        for(int j{0} ; j < vertexNum; j++){
            if(i == j){
                temp.push_back(0);
            }
            else temp.push_back(M);
        }
        for(Edge *edge : vertexList[i]->getEdges()){
            temp[edge->destVertex()->getIndex()] = edge->getWeight();
        }
        matrix.push_back(temp);
    }
    floydWarshall(matrix, src, dest);
}



//typedef struct{
//    DWORD dwUnknow1;
//    ULONG ukeMaximumIncrement;
//    ULONG uPageSize;
//    ULONG uMmNumberOfPhysicalPages;
//    ULONG uMmLowestPhysicalPage;
//    ULONG uMnHighestPhysicalPage;
//    ULONG uAllocationGranularity;
//    PVOID pLowestUserAddress;
//    PVOID pMmHighestUserAddress;
//    ULONG uKeActiveProcessors;
//    BYTE bKeNumberProcessors;
//    BYTE bUnknown2;
//    WORD wUnknown3;
//} SYSTEM_BASIC_INFORMTION;

//typedef struct{
//    LARGE_INTEGER   liIdleTime;
//    DWORD           dwSpare;
//} SYSTEM_PERFORMANCE_INFORMATION;

//typedef struct{
//    LARGE_INTEGER liKeBootTime;
//    LARGE_INTEGER liKeSystemTime;
//    LARGE_INTEGER liExpTimeZoneBias;
//    ULONG         uCurrentTimeZoneId;
//    DWORD         dwReserved;
//} SYSTEM_TIME_INFORMATION;

//typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
//PROCNTQSI NtQuerySystemInformation;

//int getCpuStat(){
//    SYSTEM_PERFORMANCE_INFORMATION SysperfInfo;
//    SYSTEM_TIME_INFORMATION SysTimeInfo;
//    SYSTEM_BASIC_INFORMTION SysBaseInfo;

//    double dbIdleTime;
//    double dbSystemTime;
//    LONG status;
//    LARGE_INTEGER liOldIdleTime = {0, 0};
//    LARGE_INTEGER liOldSystemTime = {0, 0};
//    int UsageCpu = 0;
//    NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(
//                GetModuleHandleA("ntdll"), "NtQuertSystemInformation");



//}














