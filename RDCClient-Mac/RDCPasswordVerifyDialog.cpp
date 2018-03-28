#include "RDCPasswordVerifyDialog.h"
#include "ui_RDCPasswordVerifyDialog.h"

RDCPasswordVerifyDialog::RDCPasswordVerifyDialog(QWidget *parent) :
    QDialog(parent, Qt::Sheet),
    ui(new Ui::RDCPasswordVerifyDialog)
{
    ui->setupUi(this);
}

RDCPasswordVerifyDialog::~RDCPasswordVerifyDialog()
{
    delete ui;
}

QString RDCPasswordVerifyDialog::password()
{
    return ui->passwordLineEdit->text();
}

void RDCPasswordVerifyDialog::setVerifyCount(int count)
{
    ui->passwordLineEdit->setPlaceholderText(QString("请输入密码(还剩%1次机会)").arg(count));
    return ;
}

void RDCPasswordVerifyDialog::on_cancelPushButton_clicked()
{
    return this->reject();
}

void RDCPasswordVerifyDialog::on_verifyPushButton_clicked()
{
    return this->accept();
}
