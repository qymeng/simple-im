#include "repswfail.h"
#include "ui_repswfail.h"
#include "client.h"
#include<QString>

Repswfail::Repswfail(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Repswfail)
{
    ui->setupUi(this);
}

Repswfail::~Repswfail()
{
    delete ui;
}

void Repswfail::on_pushButton_clicked()
{
    this->close();
}

void Repswfail::showEvent(QShowEvent *event)
{
    QString n;
    if(kind == FIR_SEC_FAIL)
    {
        n = QTextCodec::codecForName("GBK")->toUnicode("Á½´ÎÃÜÂë²»Ò»ÖÂ");
    }
    else if(kind == SET_PSW_FAIL)
    {
        n = QTextCodec::codecForName("GBK")->toUnicode("ÉèÖÃÃÜÂëÊ§°Ü");
    }
    ui->label->setText(n);
}
