#ifndef MAIN_FORM_H
#define MAIN_FORM_H
#include <QMainWindow>
#include <QtNetwork>
#define MAX_SIZE 1024

namespace Ui {
class Main_form;
}

class Main_form : public QMainWindow
{
    Q_OBJECT

public:
    QTcpSocket *client;
    QString total;
    QString Username;
    char Send_Inf[MAX_SIZE];
    char Recv_Inf[MAX_SIZE];
    char msg[900];
    char his_msg[920];
    explicit Main_form(QWidget *parent = nullptr);
    ~Main_form();

private slots:
    void on_Sendbutton_clicked();

    void on_Sendtext_editingFinished();

    void on_Setbutton_clicked();

    void on_Hstbutton_clicked();

    void showEvent(QShowEvent *event);

    void on_Massagetext_textChanged();

    void readData();

    void readDatah();

private:
    Ui::Main_form *ui;

    void keyPressEvent(QKeyEvent *event);

    void closeEvent(QCloseEvent *event);

};

#endif // MAIN_FORM_H
