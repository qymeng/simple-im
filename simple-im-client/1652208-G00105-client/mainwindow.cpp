#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "main_form.h"
#include "load_msg.h"
#include "client.h"
#include <QKeyEvent>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    kind = SECCUSS;
    ui->psw_edit->setEchoMode(QLineEdit::Password);
    for(int i = 0; i < MAX_SIZE; i++){
        Send_Inf[i] = '\0';
        Recv_Inf[i] = '\0';
    }
    list = new lis;
    list->num = 0;
    list->header = new friends;
    for (int i = 0; i < 15; i++){
        list->header->name[i] = 0;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Button_LOAD_clicked()
{
    /*建立socket*/
    QTcpSocket *socket;
    socket = new QTcpSocket;
    client = socket;
    //client->connectToHost(QHostAddress("129.28.117.156"), 8080, QTcpSocket::ReadWrite);
    //client->connectToHost(QHostAddress("192.168.80.230"), 4000, QTcpSocket::ReadWrite);
    client->connectToHost(QHostAddress("10.60.102.252"), 22000, QTcpSocket::ReadWrite);
    connect(this->client,SIGNAL(readyRead()),SLOT(readData()));
    connect(this->client,SIGNAL(connected()),SLOT(connection()));
    connect(this->client,SIGNAL(disconnected()),SLOT(disconnection()));
}

void MainWindow::on_user_edit_editingFinished()
{
    char *p;
    QString arg1 = ui->user_edit->text();
    QByteArray ba = arg1.toLocal8Bit();
    p = ba.data();
    memcpy(&Send_Inf[4], p, strlen(p));
    User = arg1;
}

void MainWindow::on_psw_edit_editingFinished()
{
    char *p;
    QString arg1 = ui->psw_edit->text();
    QByteArray ba = arg1.toLocal8Bit();
    p = ba.data();
    memcpy(&Send_Inf[19], p, strlen(p));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
        MainWindow::on_Button_LOAD_clicked();
    }
}

void MainWindow::connection()
{
    kind = SECCUSS;
    int sig;
    sig = login_send;
    memcpy(Send_Inf, &sig, 4);
    client->write(Send_Inf, MAX_SIZE);
}

void MainWindow::disconnection()
{
    //
}

void MainWindow::readData()
{
    short sig;
    client->read(Recv_Inf, MAX_SIZE);
    memcpy(&sig, &Recv_Inf, 2);
    if (sig == login_success_re)//登录成功，挤下另一个客户端
    {
        kind = LOADED;
        Load_msg *load_msg = new Load_msg;
        load_msg->client = client;
        load_msg->kind = kind;
        setgroupmsg();
        load_msg->list = list;
        load_msg->User = User;
        load_msg->exec();
        disconnect(this->client,SIGNAL(readyRead()),this,SLOT(readData()));
        ui->psw_edit->setText("\0");
        ui->user_edit->setText("\0");
        this->close();
    }
    else if (sig == login_success)//登录成功
    {
        kind = SECCUSS;
        Load_msg *load_msg = new Load_msg;
        load_msg->client = client;
        load_msg->kind = kind;
        setgroupmsg();
        load_msg->list = list;
        load_msg->User = User;
        load_msg->exec();
        disconnect(this->client,SIGNAL(readyRead()),this,SLOT(readData()));
        ui->psw_edit->setText("\0");
        ui->user_edit->setText("\0");
        this->close();
    }
    else if (sig == login_failed)//密码错误
    {
        kind = NOPSW;
        Load_msg *load_msg = new Load_msg;
        load_msg->client = client;
        load_msg->kind = kind;
        load_msg->exec();
        ui->psw_edit->setText("\0");
        ui->user_edit->setText("\0");
    }
    else if (sig == login_pwset)//第一次登录成功，修改密码提醒
    {
        kind = FIRSTLOAD;
        Load_msg *load_msg = new Load_msg;
        load_msg->client = client;
        load_msg->kind = kind;
        setgroupmsg();
        load_msg->list = list;
        load_msg->exec();
        disconnect(this->client,SIGNAL(readyRead()),this,SLOT(readData()));
        ui->psw_edit->setText("\0");
        ui->user_edit->setText("\0");
    }
    else if(sig == login_notarget)//没有用户信息
    {
        kind = NOUSER;
        Load_msg *load_msg = new Load_msg;
        load_msg->client = client;
        load_msg->kind = kind;
        load_msg->exec();
        ui->psw_edit->setText("\0");
        ui->user_edit->setText("\0");
    }
}

void MainWindow::setgroupmsg()
{
    char *p = &Recv_Inf[4];
    for(int i = 0; i < MAX_SIZE && *p != '\0'; i++, p+=16){
        f = new friends;
        f->status[0] = *p;
        for(int j = 0; j < 15; j++){
            f->name[j] = *(p + j + 1);
        }
        f->next = list->header;
        list->header = f;
        list->num++;
    }
}
