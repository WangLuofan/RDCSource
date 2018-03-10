#include "RDCPreferencesDialog.h"
#include "ui_RDCPreferencesDialog.h"
#include "RDCConfiguration.h"

RDCPreferencesDialog::RDCPreferencesDialog(QWidget *parent) :
    QDialog(parent, Qt::Sheet),
    ui(new Ui::RDCPreferencesDialog)
{
    ui->setupUi(this);
    ui->listenPortLineEdit->setText(
                RDCConfiguration::standardConfiguration()->valueForKey("ListenPort").toString());
}

RDCPreferencesDialog::~RDCPreferencesDialog()
{
    delete ui;
}

void RDCPreferencesDialog::on_cancelButton_clicked()
{
    this->close();
    return ;
}

void RDCPreferencesDialog::on_saveButton_clicked()
{
    RDCConfiguration::standardConfiguration()->
            setValueForKey(QVariant(ui->listenPortLineEdit->text()), "ListenPort");
    this->close();
    return ;
}
