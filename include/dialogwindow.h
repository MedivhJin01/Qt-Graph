#ifndef DIALOGWINDOW_H
#define DIALOGWINDOW_H



class dialogwindow{
public:
    dialogwindow();
    void createEraseVertexWindow();

private slots:
    void eraseVertexSignal();

private:
    QWidget *eraseVertexWindow;
    QLineEdit *vertex;
    QLineEdit *dest;
};

#endif // DIALOGWINDOW_H
