#ifndef RDCPASSWORDVERIFYDIALOG_H
#define RDCPASSWORDVERIFYDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class RDCPasswordVerifyDialog;
}

class RDCPasswordVerifyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RDCPasswordVerifyDialog(QWidget *parent = 0);
    ~RDCPasswordVerifyDialog();

    QString password(void);
    void setVerifyCount(int);

private slots:
    void on_cancelPushButton_clicked();
    void on_verifyPushButton_clicked();

private:
    Ui::RDCPasswordVerifyDialog *ui;
};

#endif // RDCPASSWORDVERIFYDIALOG_H
