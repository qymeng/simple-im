#ifndef GROUP_FORM_H
#define GROUP_FORM_H
#include <QtNetwork>
#include <QWidget>
#include "main_form.h"
#include "client.h"
#define MAX_SIZE 1024

namespace Ui {
class Group_form;
}

class Group_form : public QWidget
{
    Q_OBJECT

public:
    QTcpSocket *client;
    char Send_Inf[MAX_SIZE];
    char Recv_Inf[MAX_SIZE];
    msg_window *head;
    Main_form ex_massage[100];
    Main_form *main_form;
    int count;
    lis *list;
    friends *f;
    char name[15];
    //char msg[900];
    //char his_msg[920];
    QTimer m_Timer;
    bool m_IsConnected;
    QString User;
    explicit Group_form(QWidget *parent = nullptr);
    ~Group_form();

private slots:
    void on_Allbutton_clicked();

    void showEvent(QShowEvent *event);

    void closeEvent(QCloseEvent *event);

    void readData();

    void on_Usermsg_cellDoubleClicked(int row, int column);

    void disconnection();

    void slotTimeOut();

    void clientConnected();

private:
    Ui::Group_form *ui;

    void force_offline();

signals:
    void ToRead();

    void ToReadh();
};

#endif // GROUP_FORM_H
