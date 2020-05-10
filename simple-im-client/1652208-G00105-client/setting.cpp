#include "setting.h"
#include "ui_setting.h"
#include <QColorDialog>

Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
}

Setting::~Setting()
{
    delete ui;
}

void Setting::on_colbutton_clicked()
{
    QColorDialog color;
    QColor c = color.getColor();
    int r = c.red();
    int g = c.green();
    int b = c.blue();
    QPalette palette(this->palette());
    palette.setColor(QPalette::Background, c);
    this->setPalette(palette);
    int sig = 0x00000000;

}
