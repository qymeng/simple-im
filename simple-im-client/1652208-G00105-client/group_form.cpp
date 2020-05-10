#include "group_form.h"
#include "ui_group_form.h"
#include "main_form.h"
#include "f_offline.h"
#include "client.h"
#include "f_offline.h"
#include <QScrollBar>
#include <QTableWidget>
#include <QTableWidgetItem>

Group_form::Group_form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Group_form)
{
    ui->setupUi(this);
    ui->Usermsg->setSelectionBehavior (QAbstractItemView::SelectRows);
    ui->Usermsg->setSelectionMode (QAbstractItemView::SingleSelection);
    ui->Usermsg->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->Usermsg->horizontalHeader()->resizeSection(0,200);
    ui->Usermsg->horizontalHeader()->resizeSection(1,100);
    ui->Usermsg->horizontalScrollBar()->setEnabled(false);
    ui->Usermsg->verticalHeader()->setVisible(false);
    ui->Usermsg->horizontalHeader()->setVisible(false);
    ui->network->setText(QTextCodec::codecForName("GBK")->toUnicode("已连接"));
    ui->network->setStyleSheet("color: green;");
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(slotTimeOut()));
    ui->Usermsg->insertRow(0);
    ui->Usermsg->setItem(0, 0, new QTableWidgetItem(QTextCodec::codecForName("GBK")->toUnicode("用户名")));
    ui->Usermsg->setItem(0, 1, new QTableWidgetItem(QTextCodec::codecForName("GBK")->toUnicode("状态")));
    count = 0;
}

Group_form::~Group_form()
{
    delete ui;
}

void Group_form::showEvent(QShowEvent *event)
{
    setWindowTitle(QString(User));
    connect(this->client,SIGNAL(readyRead()),this,SLOT(readData()));
    char name_ch[15];
    char sta_ch;
    QString name_str;
    f = list->header;
    friends del;
    del.next = f;
    for(int i = 1; i <= list->num; i++){
        ui->Usermsg->insertRow(i);
        memcpy(&sta_ch, f->status, 1);
        memcpy(name_ch, f->name, 15);
        name_str = QTextCodec::codecForName("GBK")->toUnicode(name_ch);
        ui->Usermsg->setItem(i, 0, new QTableWidgetItem(name_str));
        if(sta_ch == '0'){
            ui->Usermsg->setItem(i, 1, new QTableWidgetItem(QTextCodec::codecForName("GBK")->toUnicode("离线")));
        }
        else if(sta_ch == '1'){
            ui->Usermsg->setItem(i, 1, new QTableWidgetItem(QTextCodec::codecForName("GBK")->toUnicode("在线")));
        }
        f = f->next;
        delete del.next;
        del.next = f;
    }
    list->num = 0;
    ui->Usermsg->sortItems(1, Qt::DescendingOrder);
    connect(this->client,SIGNAL(disconnected()),this,SLOT(disconnection()));
    connect(this->client,SIGNAL(connected()),this,SLOT(clientConnected()));
}

void Group_form::closeEvent(QCloseEvent *event)
{
    int sig = logout_quest;
    memcpy(Send_Inf, &sig, 4);
    client->write(Send_Inf, MAX_SIZE);
    client->close();
    for(int i = 0; i < count; i++){
        ex_massage[i].close();
    }
}

void Group_form::on_Allbutton_clicked()
{
    int i;
    for(i = 0; i < count; i++){
        if(ex_massage[i].Username == "all"){
            ex_massage[i].raise();
            break;
        }
    }
    if(i == count){
        ex_massage[count].client = client;
        ex_massage[count].Username = "all";
        ex_massage[count].show();
        count++;
    }
}

void Group_form::readData()
{
    short sig;
    client->read(Recv_Inf, MAX_SIZE);
    memcpy(&sig, Recv_Inf, 2);
    //判断read内容执行
    /*1下线信息*/
    if(sig == force_logout){
        F_offline *f = new F_offline;
        f->show();
        this->close();
    }
    /*2聊天信息*/
    else if(sig == msg_get){
        memcpy(name, &Recv_Inf[4], 15);
        int i;
        QString name_str = QTextCodec::codecForName("GBK")->toUnicode(name);
        for(i = 0; i < count; i++){
            if(ex_massage[i].Username == name_str){
                if(!ex_massage->isHidden()){
                    ex_massage[i].raise();
                }
                else {
                    ex_massage[i].show();
                    ex_massage[i].raise();
                }
                main_form = &ex_massage[i];
                memcpy(ex_massage[i].msg, &Recv_Inf[20], 900);
                connect(this,SIGNAL(ToRead()),main_form,SLOT(readData()));
                emit ToRead();
                disconnect(this,SIGNAL(ToRead()),main_form,SLOT(readData()));
                break;
            }
        }
        if(i == count){
            main_form = &ex_massage[count];
            connect(this,SIGNAL(ToRead()),main_form,SLOT(readData()));
            ex_massage[count].client = client;
            ex_massage[count].Username = name_str;
            memcpy(ex_massage[i].msg, &Recv_Inf[20], 900);
            ex_massage[count].show();
            ex_massage[count].raise();
            emit ToRead();
            disconnect(this,SIGNAL(ToRead()),main_form,SLOT(readData()));
            count++;
        }
    }
    /*3历史记录*/
    else if(sig == log_get){
        memcpy(name, &Recv_Inf[4], 15);
        int i;
        QString name_str = QTextCodec::codecForName("GBK")->toUnicode(name);
        for(i = 0; i < count; i++){
            if(ex_massage[i].Username == name_str){
                if(!ex_massage->isHidden()){
                    ex_massage[i].raise();
                }
                else {
                    ex_massage[i].show();
                    ex_massage[i].raise();
                }
                main_form = &ex_massage[i];
                memcpy(ex_massage[i].his_msg, &Recv_Inf[20], 900);
                connect(this,SIGNAL(ToReadh()),main_form,SLOT(readDatah()));
                emit ToReadh();
                disconnect(this,SIGNAL(ToReadh()),main_form,SLOT(readDatah()));
                break;
            }
        }
        /*if(i == count){
            main_form = &ex_massage[count];
            connect(this,SIGNAL(ToReadh()),main_form,SLOT(readDatah()));
            ex_massage[count].client = client;
            ex_massage[count].Username = name_str;
            memcpy(ex_massage[count].his_msg, &Recv_Inf[20], 900);
            ex_massage[count].show();
            emit ToReadh();
            disconnect(this,SIGNAL(ToReadh()),main_form,SLOT(readDatah()));
            count++;
        }*/
    }
    /*4文件*/
    else if(sig == file_get){
        //
    }
    /*5设置*/

    /*6用户信息更新*/
    else if(sig == friend_login){
        int row = ui->Usermsg->rowCount();
        int i;
        QString name_str;
        char name_ch[15];
        memcpy(&name_ch, &Recv_Inf[4], 15);
        name_str = QTextCodec::codecForName("GBK")->toUnicode(name_ch);
        for(i = 1; i < row; i++){
            if(name_str == ui->Usermsg->item(i, 0)->text()){
                ui->Usermsg->setItem(i, 1, new QTableWidgetItem(QTextCodec::codecForName("GBK")->toUnicode("在线")));
                break;
            }
        }
        ui->Usermsg->sortItems(1, Qt::DescendingOrder);
    }
    else if(sig == friend_logout){
        int row = ui->Usermsg->rowCount();
        int i;
        QString name_str;
        char name_ch[15];
        memcpy(&name_ch, &Recv_Inf[4], 15);
        name_str = QTextCodec::codecForName("GBK")->toUnicode(name_ch);
        for(i = 1; i < row; i++){
            if(name_str == ui->Usermsg->item(i, 0)->text()){
                ui->Usermsg->setItem(i, 1, new QTableWidgetItem(QTextCodec::codecForName("GBK")->toUnicode("离线")));
                if(!ex_massage[i].isHidden()){
                    ex_massage[i].close();
                }
                break;
            }
        }
        ui->Usermsg->sortItems(1, Qt::DescendingOrder);
    }
}


void Group_form::force_offline()
{
    F_offline *off = new F_offline;
    off->show();
    client->close();
    this->close();
}

void Group_form::on_Usermsg_cellDoubleClicked(int row, int column)
{
    int i;
    if(ui->Usermsg->item(row, 1)->text() == QTextCodec::codecForName("GBK")->toUnicode("在线")){
        for(i = 0; i < count; i++){
            if(ex_massage[i].Username == ui->Usermsg->item(row, 0)->text()){
                if(!ex_massage->isHidden()){
                    ex_massage[i].raise();
                }
                else {
                    ex_massage[i].show();
                    ex_massage[i].raise();
                }
                break;
            }
        }
        if(i == count){
            ex_massage[count].client = client;
            ex_massage[count].Username = ui->Usermsg->item(row, 0)->text();
            ex_massage[count].show();
            ex_massage[count].raise();
            count++;
        }
    }
}

void Group_form::disconnection()
{
    ui->network->setText(QTextCodec::codecForName("GBK")->toUnicode("断开连接"));
    ui->network->setStyleSheet("color: red;");
    m_Timer.start(2000);
    m_IsConnected = false;
}

void Group_form::clientConnected()
{
    m_Timer.stop();
    m_IsConnected = true;
    ui->network->setText(QTextCodec::codecForName("GBK")->toUnicode("已连接"));
    ui->network->setStyleSheet("color: green;");
}

void Group_form::slotTimeOut()
{
    if (!m_IsConnected)
    {
        client->connectToHost(QHostAddress("10.60.102.252"), 22000, QTcpSocket::ReadWrite);
    }
}
