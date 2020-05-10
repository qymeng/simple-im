#include "f_offline.h"
#include "ui_f_offline.h"

F_offline::F_offline(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::F_offline)
{
    ui->setupUi(this);
}

F_offline::~F_offline()
{
    delete ui;
}

void F_offline::on_OKbutton_clicked()
{
    this->close();
}
