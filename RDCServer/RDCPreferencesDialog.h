#ifndef RDCPREFERENCESDIALOG_H
#define RDCPREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
class RDCPreferencesDialog;
}

class QSettings;
class RDCPreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RDCPreferencesDialog(QWidget *parent = 0);
    ~RDCPreferencesDialog();

private slots:
    void on_cancelButton_clicked();
    void on_saveButton_clicked();

private:
    Ui::RDCPreferencesDialog *ui;
};

#endif // RDCPREFERENCESDIALOG_H
