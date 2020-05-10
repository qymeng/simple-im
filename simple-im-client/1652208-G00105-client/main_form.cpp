#include "main_form.h"
#include "ui_main_form.h"
#include "f_offline.h"
#include "setting.h"
#include "group_form.h"
#include "client.h"
#include <QKeyEvent>
#include <QString>
#include <QScrollBar>

Main_form::Main_form(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Main_form)
{
    ui->setupUi(this);
    ui->Massagetext->setFocusPolicy(Qt::NoFocus);
    ui->Sendtext->setMaxLength(900);
    ui->Hsttext->setFocusPolicy(Qt::NoFocus);

    QPalette palette(this->palette());
    palette.setColor(QPalette::Background, Qt::white);
    this->setPalette(palette);

    ui->Massagetext->setFont(QFont(tr("Consolas"), 14));
}

Main_form::~Main_form()
{
    delete ui;
}

void Main_form::showEvent(QShowEvent *event)
{
    setWindowTitle(QString(Username));
}

void Main_form::closeEvent(QCloseEvent *event)
{
    //
}

void Main_form::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
        Main_form::on_Sendbutton_clicked();
    }
}

void Main_form::on_Sendbutton_clicked()
{
    if((ui->Sendtext->text().length()) > 0){
        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss");

        total = ui->Massagetext->toPlainText();
        total = total + "[" + current_date + "]" + "[" + QTextCodec::codecForName("GBK")->toUnicode("local messages") + "]" + '\n';
        total = total + ui->Sendtext->text() + '\n';
        ui->Massagetext->setText(total);

        ui->Sendtext->setText("\0");

        int sig;
        sig = msg_send;
        memcpy(Send_Inf, &sig, 4);

        client->write(Send_Inf, MAX_SIZE);
    }
}

void Main_form::on_Sendtext_editingFinished()
{
    char *p1, *p2;
    for(int i = 0; i < MAX_SIZE; i++){
        Send_Inf[i] = '\0';
    }
    QString arg1 = "@" + Username + ":";
    QByteArray ba = arg1.toLocal8Bit();
    p1 = ba.data();
    QString arg2 = ui->Sendtext->text();
    QByteArray bb = arg2.toLocal8Bit();
    p2 = bb.data();
    memcpy(&Send_Inf[4], p1, strlen(p1));
    if(strlen(p2) > 0){
        memcpy(&Send_Inf[4 + strlen(p1)], p2, strlen(p2));
    }
}

void Main_form::on_Setbutton_clicked()
{
    Setting *set = new Setting;
    set->show();
}

void Main_form::on_Hstbutton_clicked()
{
    char *p1;
    QString arg1 = Username;
    QByteArray ba = arg1.toLocal8Bit();
    p1 = ba.data();
    for(int i = 0; i < MAX_SIZE; i++){
        Send_Inf[i] = '\0';
    }
    ui->Hsttext->setText("\0");
    int sig = log_quest;
    memcpy(Send_Inf, &sig, 4);
    memcpy(&Send_Inf[4], p1, strlen(p1));
    client->write(Send_Inf, MAX_SIZE);
}

void Main_form::on_Massagetext_textChanged()
{
    ui->Massagetext->moveCursor(QTextCursor::End);
}

void Main_form::readData()
{
    int i;
    for(i = 0; i < 900; i++){
        if(msg[i] == '\0')break;
    }
    if(i != 0){
        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss");
        QString msg_str = QTextCodec::codecForName("GBK")->toUnicode(msg);
        total = ui->Massagetext->toPlainText();
        total = total + "[" + current_date + "]" + '\n';
        total = total + msg_str + '\n';
        ui->Massagetext->setText(total);
        for(int j = 0; j < i; j++){
            msg[j] = '\0';
        }
    }
}

void Main_form::readDatah()
{
    int i;
    for(i = 0; i < 900; i++){
        if(his_msg[i] == '\0')break;
    }
    if(i != 0){
        QDateTime current_date_time =QDateTime::currentDateTime();
        QString his_msg_str = QTextCodec::codecForName("GBK")->toUnicode(his_msg);
        total = ui->Hsttext->toPlainText();
        total = total + his_msg_str + '\n';
        ui->Hsttext->setText(total);
        for(int j = 0; j < i; j++){
            his_msg[j] = '\0';
        }
    }
}
