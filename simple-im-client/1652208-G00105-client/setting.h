#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include <QtNetwork>

namespace Ui {
class Setting;
}

class Setting : public QWidget
{
    Q_OBJECT

public:
    char Send_Inf[1024];
    QTcpSocket *client;
    explicit Setting(QWidget *parent = nullptr);
    ~Setting();

private slots:
    void on_colbutton_clicked();

private:
    Ui::Setting *ui;
};

#endif // SETTING_H
