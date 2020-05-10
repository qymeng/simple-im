#ifndef F_OFFLINE_H
#define F_OFFLINE_H

#include <QDialog>

namespace Ui {
class F_offline;
}

class F_offline : public QDialog
{
    Q_OBJECT

public:
    explicit F_offline(QWidget *parent = nullptr);
    ~F_offline();

private slots:
    void on_OKbutton_clicked();

private:
    Ui::F_offline *ui;
};

#endif // F_OFFLINE_H
