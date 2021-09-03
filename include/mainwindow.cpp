#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
//    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    createWindowFlags();
    createTabWidget();
    createActions();
    createFileButton();

//    ui->setupUi(this);
}

void MainWindow::createWindowFlags(){
//    setWindowFlags(Qt::WindowFrameSection);

}

void MainWindow::createFileButton(){
    QPushButton *file_button = new QPushButton(tr("FILE"), this);
    file_button->setGeometry(0, 22, 68, 26);
    file_button->setObjectName("file");
    file_button->setStyleSheet("background-color: green;" "color: white;" "border: none"); //transition background color is not working need to be fixed here
    connect(file_button, &QPushButton::clicked, this, &MainWindow::sideBarOpen);
}


void MainWindow::createTabWidget(){
    const QSize *tabSize = new QSize(20, 20);
    QTabWidget *tab = new QTabWidget(this);

    tab->setStyleSheet("QTabWidget::tab-bar {left: 80px;}" "QTabBar::tab:selected, QTabBar::tab:hover{color: green;}");


    ///////////////////view_tab properties/////////////////////
    QWidget *view_tab = new QWidget;



    QHBoxLayout *view_Layout = new QHBoxLayout;

    view_tab->setLayout(view_Layout);
    QVBoxLayout *pick_Layout = new QVBoxLayout;



    QGroupBox *view_groupBox = new QGroupBox(tr("Pick"));


//    view_groupBox->setStyleSheet("QGroupBox{border-color:black;top:0px;}" "QGroupBox:title{left:30px; subcontrol-position:bottom left}");
//    view_groupBox->setMaximumWidth(100);
//    view_groupBox->setMaximumHeight(200);
//    view_Layout->addWidget(view_groupBox);
//    QGroupBox *view_groupBox_1 = new QGroupBox(tr("Manipulate"));
//    view_groupBox_1->setStyleSheet("QGroupBox{border-color:green; margin-right:1300px;}");
//    view_Layout->addWidget(view_groupBox_1);

//    view_Layout->addWidget(view_groupBox);
//    view_tabLayout->addWidget(view_groupBox, 0, 0);
//    view_tabLayout->addWidget(view_groupBox, 1, 0);

//    view_tabLayout->setColumnMinimumWidth(0, 50);

//    QHBoxLayout *hlayout = new QHBoxLayout;
//    hlayout->setSizeConstraint(QLayout::SetMaximumSize);


    QToolButton *Rect = new QToolButton(this);
    Rect->setIcon(QIcon(":/imgs/Rect.png"));
    const QSize *rectSize = new QSize(40, 40);
    Rect->setIconSize(*rectSize);
    Rect->setText("Rect");
    Rect->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    Rect->setPopupMode(QToolButton::MenuButtonPopup);
    Rect->setStyleSheet("QToolButton{background-color:white; border:none;} QToolButton::menu");


    QGroupBox *pick_groupBox = new QGroupBox(tr("Pick"));
    pick_groupBox->setMaximumWidth(100);
    pick_groupBox->setMaximumHeight(30);
    pick_groupBox->setStyleSheet("QGroupBox:title{left: 6px;}" "QGroupBox{border:none;}");


//    QLabel *pick_label = new QLabel;
//    pick_label->setText(tr(" Pick"));
//    pick_label->setStyleSheet("QLabel{left:40px;}");
    pick_Layout->setSpacing(20);

    view_Layout->setSpacing(0);

    pick_Layout->addWidget(Rect);
    pick_Layout->addWidget(pick_groupBox);
//    pick_Layout->addWidget(pick_label);
    pick_Layout->addStretch();

    view_Layout->addLayout(pick_Layout);
    view_Layout->addStretch();



//    QLabel *pick = new QLabel;
//    pick->setText("Pick");
//    pick->setGeometry(0, 200, 50, 10);
//    pick->setStyleSheet("QLabel{top:200px; left:500px}");



//    hlayout->addWidget(Rect);
//    hlayout->addWidget(pick);

//    QGroupBox *pick_group = new QGroupBox(tr("Pick"));
//    pick_group->setStyleSheet("QGroupBox{border:none;}");
//    pick_group->setGeometry(50, 50, 50, 70);
//    pick_group->setMaximumSize(150, 100);
//    pick_group->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
//    pick_group->set




    /*QGroupBox{
        border-width:1px;
        border-style:solid;
        border-color:lightGray;
        margin-top:1ex;
        }

        QGroupBox:title{
        subcontrol-origin: margin;
        subcontrol-position: bottom left;
        left: 30px;
        margin-left: 0px;
        padding: 1px;}
     */


//    QGroupBox *manipulate_group = new QGroupBox(tr("Manipulate"));
//    manipulate_group->setMaximumSize(250, 100);
//    manipulate_group->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
//    hlayout->addWidget(manipulate_group);

    QWidget *create_tab = new QWidget;
    QWidget *modify_tab = new QWidget;
    QWidget *make_tab = new QWidget;
    QWidget *tool_tab = new QWidget;
    QWidget *optimize_bar = new QWidget;
    QWidget *effect_bar = new QWidget;
    QWidget *manage_bar = new QWidget;
    QWidget *solution_bar = new QWidget;
    QWidget *debug_bar = new QWidget;
    QWidget *option_bar = new QWidget;


    tab -> setIconSize(*tabSize);
    tab -> addTab(view_tab, QIcon(":/images/example"), tr("VIEW"));
    tab -> addTab(create_tab, QIcon(":/images/example"), tr("CREATE"));
    tab -> addTab(modify_tab, QIcon(":/images/example"), tr("MODIFY"));
    tab -> addTab(make_tab, QIcon(":/images/example"), tr("MAKE"));
    tab -> addTab(tool_tab, QIcon(":/images/example"), tr("TOOLS"));
    tab -> addTab(optimize_bar, QIcon(":/images/example"), tr("OPTIMIZE"));
    tab -> addTab(effect_bar, QIcon(":/images/example"), tr("EFFECTS"));
    tab -> addTab(manage_bar, QIcon(":/images/example"), tr("MANAGE"));
    tab -> addTab(solution_bar, QIcon(":/images/example"), tr("SOLUTION"));
    tab -> addTab(debug_bar, QIcon(":/images/example"), tr("DEBUG"));
    tab -> addTab(option_bar, QIcon(":/images/example"), tr("OPTIONS"));

    tab -> setGeometry(-10, 20, 1930, 160);
    connect(tab, SIGNAL(currentchanged(int)), this, SLOT(currentTab(int)));
}

void MainWindow::createActions(){
    QMenu *operationMenu = menuBar() -> addMenu(tr("&Operation"));
    const QIcon restoreIcon = QIcon::fromTheme("document-restore", QIcon(":/images/restore.png"));
    QAction *restoreAct = new QAction(restoreIcon, tr("&Restore"), this);
    connect(restoreAct, &QAction::triggered, this, &MainWindow::restore);
    operationMenu->addAction(restoreAct);

    const QIcon moveIcon = QIcon::fromTheme("document-move", QIcon(":/images/move.png"));
    QAction *moveAct = new QAction(moveIcon, tr("&Move"), this);
    connect(moveAct, &QAction::triggered, this, &MainWindow::move);
    operationMenu->addAction(moveAct);

    const QIcon sizeIcon = QIcon::fromTheme("document-size", QIcon(":/images/size.png"));
    QAction *sizeAct = new QAction(sizeIcon, tr("&Size"), this);
    connect(sizeAct, &QAction::triggered, this, &MainWindow::size);
    operationMenu->addAction(sizeAct);

    const QIcon minimizeIcon = QIcon::fromTheme("document-minimize", QIcon(":/images/minimize.png"));
    QAction *minimizeAct = new QAction(minimizeIcon, tr("minimize"), this);
    connect(minimizeAct, &QAction::triggered, this, &MainWindow::minimize);
    operationMenu->addAction(minimizeAct);

    const QIcon maximizeIcon = QIcon::fromTheme("document-maximize", QIcon(":/images/maximize.png"));
    QAction *maximizeAct = new QAction(minimizeIcon, tr("maximize"), this);
    connect(maximizeAct, &QAction::triggered, this, &MainWindow::maximize);
    operationMenu->addAction(maximizeAct);

    const QIcon quitIcon = QIcon::fromTheme("document-quit", QIcon(":/images/quit.png"));
    QAction *quitAct = new QAction(quitIcon, tr("&Quit"), this);
    connect(quitAct, &QAction::triggered, qApp, &QApplication::quit);
    operationMenu->addSeparator();
    operationMenu->addAction(quitAct);

    QMenu *newMenu = menuBar() -> addMenu(tr("New"));
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    connect(newAct, &QAction::triggered, this, &MainWindow::newfile);
    newMenu->addAction(newAct);

    QMenu *openMenu = menuBar() -> addMenu(tr("Open"));
    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("Open"), this);
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    openMenu->addAction(openAct);

    QMenu *saveMenu = menuBar() -> addMenu(tr("Save"));
    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/open.png"));
    QAction *saveAct = new QAction(saveIcon, tr("Save"), this);
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    saveMenu->addAction(saveAct);

    QMenu *screenShotMenu = menuBar() -> addMenu(tr("Screen Shot"));
    const QIcon screenShotIcon = QIcon::fromTheme("document-screenShot", QIcon(":/images/screenShot.png"));
    QAction *screenShotAct = new QAction(screenShotIcon, tr("Screen Shot"), this);
    connect(screenShotAct, &QAction::triggered, this, &MainWindow::screenshot);
    screenShotMenu->addAction(screenShotAct);

    QMenu *pickMenu = menuBar() -> addMenu(tr("Pick"));
    const QIcon pickIcon = QIcon::fromTheme("document-pick", QIcon(":/images/pick.png"));
    QAction *pickAct = new QAction(pickIcon, tr("Pick"), this);
    connect(pickAct, &QAction::triggered, this, &MainWindow::pick);
    pickMenu->addAction(pickAct);
}

void MainWindow::currentTab(int index){
    qDebug("Current Tab : %d", index);
}

void MainWindow::closeEvent(QCloseEvent *event){
    if(maybeSave()){
//        writeSettings();
        event->accept();
    }
    else{
        event->ignore();
    }
}

void MainWindow::docModified(){
    setWindowModified(textEdit->document()->isModified());
}

bool MainWindow::isDocModified(){
    return textEdit->document()->isModified();
}

bool MainWindow::maybeSave(){
    if(!isDocModified()){
        return true;
    }
    const QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Application"), tr("The document has been modified.\n""Do you wat tot save your changes?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}



void MainWindow::newfile(){

}
void MainWindow::open(){

}
bool MainWindow::save(){
    return true;
}
void MainWindow::screenshot(){

}
void MainWindow::pick(){

}

void MainWindow::restore(){

}

void MainWindow::move(){

}
void MainWindow::size(){

}
void MainWindow::minimize(){

}
void MainWindow::maximize(){

}

void MainWindow::sideBarOpen(){

}


MainWindow::~MainWindow()
{
//    delete ui;
}



