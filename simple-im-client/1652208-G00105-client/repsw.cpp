#include "repsw.h"
#include "ui_repsw.h"
#include "client.h"
#include "main_form.h"
#include "group_form.h"
#include "repswfail.h"
#include <QKeyEvent>

Repsw::Repsw(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Repsw)
{
    ui->setupUi(this);
    ui->lineEdit_1->setEchoMode(QLineEdit::Password);
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);
    for(int i = 0; i < MAX_SIZE; i++){
        Send_Inf[i] = '\0';
        Recv_Inf[i] = '\0';
    }
}

Repsw::~Repsw()
{
    delete ui;
}

void Repsw::on_lineEdit_1_editingFinished()
{
    char *p;
    QString arg1 = ui->lineEdit_1->text();
    QByteArray ba = arg1.toLatin1();
    p = ba.data();
    memcpy(&Send_Inf[4], p, strlen(p));
}

void Repsw::on_pushButton_clicked()
{
    int sig;
    sig = password_send;
    memcpy(Send_Inf, &sig, 4);
    if(ui->lineEdit_1->text() != ui->lineEdit_2->text()){
        repsw = false;
        kind = FIR_SEC_FAIL;
        ui->lineEdit_1->setText("\0");
        ui->lineEdit_2->setText("\0");
        Repswfail *fail = new Repswfail;
        fail->kind = kind;
        fail->show();
    }
    else {
        repsw = true;
        client->write(Send_Inf, MAX_SIZE);
    }
}

void Repsw::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
        Repsw::on_pushButton_clicked();
    }
}

void Repsw::showEvent(QShowEvent *event)
{
    connect(this->client,SIGNAL(readyRead()),SLOT(readData()));
}

void Repsw::closeEvent(QCloseEvent *event)
{
    int sig = logout_quest;
    memcpy(Send_Inf, &sig, 4);
    client->write(Send_Inf, MAX_SIZE);
    client->close();
}

void Repsw::readData()
{
    short sig;
    client->read(Recv_Inf, MAX_SIZE);
    memcpy(&sig, &Recv_Inf, 2);
    if (sig == pwset_failed)
    {
        repsw = false;
        kind = SET_PSW_FAIL;
    }
    else if(sig == pwset_success)
    {
        repsw = true;
    }
    if(repsw == false){
        disconnect(this->client,SIGNAL(readyRead),this,SLOT(readData()));
        Repswfail *fail = new Repswfail;
        fail->kind = kind;
        fail->show();
    }
    else if(repsw == true){
        disconnect(this->client,SIGNAL(readyRead),this,SLOT(readData()));
        this->close();
    }
}
