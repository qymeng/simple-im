#ifndef LOAD_MSG_H
#define LOAD_MSG_H

#include <QDialog>
#include <QtNetwork>
#include "client.h"

namespace Ui {
class Load_msg;
}

class Load_msg : public QDialog
{
    Q_OBJECT

public:
    int kind;
    int loading;
    lis *list;
    QTcpSocket *client;
    QString User;
    explicit Load_msg(QWidget *parent = nullptr);
    ~Load_msg();

private slots:
    void on_OKbutton_clicked();

    void showEvent(QShowEvent *event);

private:
    Ui::Load_msg *ui;
};

#endif // LOAD_MSG_H
