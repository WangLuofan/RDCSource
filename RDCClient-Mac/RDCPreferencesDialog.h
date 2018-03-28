#ifndef RDCPREFERENCESDIALOG_H
#define RDCPREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
class RDCPreferencesDialog;
}

class RDCPreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RDCPreferencesDialog(QWidget *parent = 0);
    ~RDCPreferencesDialog();

private slots:
    void on_cancelPushButton_clicked();

    void on_savePushButton_clicked();

private:
    void loadDefaultConfiguration(void);

private:
    Ui::RDCPreferencesDialog *ui;
};

#endif // RDCPREFERENCESDIALOG_H
