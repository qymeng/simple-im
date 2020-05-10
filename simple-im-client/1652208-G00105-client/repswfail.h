#ifndef REPSWFAIL_H
#define REPSWFAIL_H

#include <QDialog>

namespace Ui {
class Repswfail;
}

class Repswfail : public QDialog
{
    Q_OBJECT

public:
    int kind;
    explicit Repswfail(QWidget *parent = nullptr);
    ~Repswfail();

private slots:
    void on_pushButton_clicked();

    void showEvent(QShowEvent *event);

private:
    Ui::Repswfail *ui;
};

#endif // REPSWFAIL_H
