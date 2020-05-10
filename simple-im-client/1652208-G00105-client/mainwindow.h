#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QtNetwork>
#include "client.h"
#define MAX_SIZE 1024

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QTcpSocket *client;
    int kind;
    char Send_Inf[MAX_SIZE];
    char Recv_Inf[MAX_SIZE];
    friends *f;
    lis *list;
    QString User;
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_Button_LOAD_clicked();

    void on_user_edit_editingFinished();

    void on_psw_edit_editingFinished();

    void keyPressEvent(QKeyEvent *event);

    void readData();

    void connection();

    void disconnection();

private:
    Ui::MainWindow *ui;
    void setgroupmsg();
};

#endif // MAINWINDOW_H
