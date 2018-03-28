#include "RDCPreferencesDialog.h"
#include "ui_RDCPreferencesDialog.h"
#include "RDCConfiguration.h"

RDCPreferencesDialog::RDCPreferencesDialog(QWidget *parent) :
    QDialog(parent, Qt::Sheet),
    ui(new Ui::RDCPreferencesDialog)
{
    ui->setupUi(this);
    this->loadDefaultConfiguration();
}

RDCPreferencesDialog::~RDCPreferencesDialog()
{
    delete ui;
}

void RDCPreferencesDialog::loadDefaultConfiguration(void)
{
    ui->serverAddrLineEdit->setText(RDCConfiguration::standardConfiguration()
                                    ->valueForKey("ServerAddr").toString());
    ui->serverPortLineEdit->setText(RDCConfiguration::standardConfiguration()
                                    ->valueForKey("ServerPort").toString());
    ui->localPortLineEdit->setText(RDCConfiguration::standardConfiguration()
                                   ->valueForKey("LocalPort").toString());
    ui->bgDeleteCheckBox->setChecked(RDCConfiguration::standardConfiguration()
                                     ->valueForKey("bgDelete").toBool());
    ui->verifyCheckBox->setChecked(RDCConfiguration::standardConfiguration()
                                   ->valueForKey("verifyPassword").toBool());
    ui->autoConnectCheckBox->setChecked(RDCConfiguration::standardConfiguration()
                                        ->valueForKey("autoAgreeConn").toBool());
    return ;
}

void RDCPreferencesDialog::on_cancelPushButton_clicked()
{
    this->close();
    return ;
}

void RDCPreferencesDialog::on_savePushButton_clicked()
{
    RDCConfiguration::standardConfiguration()->
            setValueForKey(QVariant(ui->serverAddrLineEdit->text()), "ServerAddr");
    RDCConfiguration::standardConfiguration()->
            setValueForKey(QVariant(ui->serverPortLineEdit->text()), "ServerPort");
    RDCConfiguration::standardConfiguration()->
            setValueForKey(QVariant(ui->localPortLineEdit->text()), "LocalPort");
    RDCConfiguration::standardConfiguration()->
            setValueForKey(QVariant(ui->bgDeleteCheckBox->checkState() == Qt::Checked), "bgDelete");
    RDCConfiguration::standardConfiguration()->
            setValueForKey(QVariant(ui->verifyCheckBox->checkState() == Qt::Checked), "verifyPassword");
    RDCConfiguration::standardConfiguration()->
            setValueForKey(QVariant(ui->autoConnectCheckBox->checkState() == Qt::Checked), "autoAgreeConn");

    this->close();
    return ;
}
