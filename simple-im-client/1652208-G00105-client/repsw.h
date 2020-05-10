#ifndef REPSW_H
#define REPSW_H

#include <QDialog>
#include <QtNetwork>
#include "client.h"
#define MAX_SIZE 1024

namespace Ui {
class Repsw;
}

class Repsw : public QDialog
{
    Q_OBJECT

public:
    bool repsw;
    QTcpSocket *client;
    int kind;
    char Send_Inf[MAX_SIZE];
    char Recv_Inf[MAX_SIZE];
    lis *list;
    explicit Repsw(QWidget *parent = nullptr);
    ~Repsw();

private slots:
    void on_lineEdit_1_editingFinished();

    void on_pushButton_clicked();

    void keyPressEvent(QKeyEvent *event);

    void readData();

    void showEvent(QShowEvent *event);

    void closeEvent(QCloseEvent *event);

private:
    Ui::Repsw *ui;
};

#endif // REPSW_H
