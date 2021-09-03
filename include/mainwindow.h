#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QIcon>
#include <QCloseEvent>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QKeySequence>
#include <QFont>
#include <QPaintEvent>
#include <QPainter>
#include <QTabBar>
#include <QLabel>
#include <QBoxLayout>


#include <QDebug>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QCheckBox>
#include <QGridLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void currentTab(int);

private:
    Ui::MainWindow *ui;
    void createWindowFlags();
    void createActions();
    void createTabWidget();
    void createFileButton();
    void docModified();
    bool isDocModified();
    bool maybeSave();
    void sideBarOpen();

    //view_tab properties functions/////////////


    QPlainTextEdit *textEdit;
    QString curFile;


protected:
    void closeEvent(QCloseEvent *event) override;

private slots:

    void newfile();
    void open();
    bool save();
    void screenshot();
    void pick();


    void restore();
    void move();
    void size();
    void minimize();
    void maximize();

//    void on_tabWidget_currentChanged(int index);
//    void on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint);
};
#endif // MAINWINDOW_H
