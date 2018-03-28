#include "RDCClient.h"
#include "RDCMainWindow.h"
#include "ui_RDCMainWindow.h"
#include "RDCMessagePool.h"
#include "RDCScreenWindow.h"
#include "RDCPreferencesDialog.h"
#include "RDCPasswordVerifyDialog.h"

#include <QMessageBox>
#include <QThread>
#include <QTimer>

RDCMainWindow::RDCMainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint),
    ui(new Ui::RDCMainWindow), m_pClient(nullptr), m_pClientThread(nullptr),
    m_pScreenWindow(new RDCScreenWindow(this)), m_pScreenTimer(nullptr)
{
    ui->setupUi(this);
    this->setFixedSize(480, 270);

    QTimer::singleShot(1000, this, SLOT(startConnection()));
}

RDCMainWindow::~RDCMainWindow()
{
    delete this->m_pClient;
    delete ui;
    delete RDCMessagePool::pool();
}

void RDCMainWindow::on_doConnectionButton_clicked(void)
{
    QString token = ui->otherTokenLineEdit->text();

    emit client_do_connection_signal(token);
    return ;
}

void RDCMainWindow::on_doDisconnectionButton_clicked()
{
    return ;
}

void RDCMainWindow::on_otherTokenLineEdit_textChanged(const QString &arg1)
{
    if(arg1.length() <= 0)
        ui->doConnectionButton->setEnabled(false);
    else
        ui->doConnectionButton->setEnabled(true);
    return ;
}

void RDCMainWindow::closeEvent(QCloseEvent*)
{
    this->m_pClient->Stop();
    this->m_pClientThread->exit();

    return ;
}

bool RDCMainWindow::verifyPasswordRecursive(QString password, int count)
{
    if(count == 0) return false;

    RDCPasswordVerifyDialog* veriDlg = new RDCPasswordVerifyDialog(this);
    veriDlg->setVerifyCount(count);
    int res = veriDlg->exec();

    if(res == QDialog::Rejected)
        return false;

    //用户通过验证
    if(veriDlg->password().compare(password) == 0)
        return true;
    return this->verifyPasswordRecursive(password, count - 1);
}

void RDCMainWindow::startConnection(void)
{
    this->m_pClient = new RDCClient();
    this->m_pClientThread = new QThread(this);

    QObject::connect(this, SIGNAL(client_verify_result_signal(bool)),
                     this->m_pClient, SLOT(doPasswordVerifyResultSlots(bool)), Qt::DirectConnection);
    QObject::connect(this->m_pClient, SIGNAL(client_verify_password_signal(QString)),
                     this, SLOT(onClientVerifyPasswordSlots(QString)));
    QObject::connect(this, SIGNAL(client_question_choose_signal(bool)),
                     this->m_pClient, SLOT(doConnectionQuerySlots(bool)), Qt::DirectConnection);
    QObject::connect(this->m_pClient, SIGNAL(client_show_message_signal(int,QString)),
                     this, SLOT(onClientShowMessage(int,QString)));
    QObject::connect(this, SIGNAL(client_do_connection_signal(QString)),
                     this->m_pClient, SLOT(doConnectionSlots(QString)), Qt::DirectConnection);
    QObject::connect(this->m_pClient, SIGNAL(client_info_update_signal(QString,QString)),
                     this, SLOT(onClientInfoUpdated(QString,QString)));
    QObject::connect(this->m_pClient, SIGNAL(client_connection_ready_signal(QString)),
                     this, SLOT(onClientConnectionReadySlots(QString)));
    QObject::connect(this->m_pClientThread, SIGNAL(started()), this->m_pClient, SLOT(Start()));
    QObject::connect(this->m_pClient, SIGNAL(client_resolution_response_signal(QSize)),
                     this, SLOT(onClientResolutionSlots(QSize)));

    this->m_pClient->moveToThread(this->m_pClientThread);
    this->m_pClientThread->start();
    return ;
}

void RDCMainWindow::onClientInfoUpdated(QString token, QString password)
{
    ui->yourTokenLineEdit->setText(token);
    ui->yourPassLineEdit->setText(password);
    return ;
}

void RDCMainWindow::onClientShowMessage(int level, QString msg)
{
    QMessageBox::Icon icon = QMessageBox::NoIcon;
    QMessageBox::StandardButtons buttons = QMessageBox::NoButton;
    QString title;
    if(level == RDCClient::MessageLevelInfomation)
    {
        icon = QMessageBox::Information;
        title = QString("Infomation");
        buttons = QMessageBox::Ok;
    }
    else if(level == RDCClient::MessageLevelQuestion)
    {
        icon = QMessageBox::Question;
        title = QString("Question");
        buttons = QMessageBox::Yes | QMessageBox::No;
    }
    else if(level == RDCClient::MessageLevelWarning)
    {
        icon = QMessageBox::Warning;
        title = QString("Warning");
        buttons = QMessageBox::Ok;
    }
    else
    {
        icon = QMessageBox::Critical;
        title = QString("Critical");
        buttons = QMessageBox::Ok;
    }

    QMessageBox msgBox(icon, title, msg, buttons, this, Qt::Sheet);
    msgBox.setButtonText(QMessageBox::Ok, QString("确定"));
    msgBox.setButtonText(QMessageBox::Yes, QString("同意"));
    msgBox.setButtonText(QMessageBox::No, QString("拒绝"));

    int ret = msgBox.exec();

    if(level == RDCClient::MessageLevelQuestion)
        emit client_question_choose_signal(ret == QMessageBox::Yes);

    return ;
}

void RDCMainWindow::onClientConnectionReadySlots(QString title)
{
    if(this->m_pScreenWindow != nullptr)
    {
        //最小化当前窗口
        if(this->isMinimized() == false)
            this->showMinimized();
        //显示当前图像窗口
        this->m_pScreenWindow->setWindowTitle(QString("%1 [已连接]").arg(title));
        this->m_pScreenWindow->showNormal();

        QObject::disconnect(this->m_pClient, SIGNAL(client_resolution_response_signal(QSize)),
                            this, SLOT(onClientResolutionSlots(QSize)));
        QObject::connect(this->m_pClient, SIGNAL(client_resolution_response_signal(QSize)),
                         this->m_pScreenWindow, SLOT(onClientResolutionSlots(QSize)));
    }
    return ;
}

void RDCMainWindow::onClientResolutionSlots(QSize)
{
    //主控端请求分辨率，启动定时器
    if(this->m_pScreenTimer == nullptr)
    {
        this->m_pScreenTimer = new QTimer(this);

        QObject::connect(this->m_pScreenTimer, SIGNAL(timeout()),
                         this->m_pClient, SLOT(doScreenGenerate()), Qt::DirectConnection);
        this->m_pScreenTimer->start(200);
    }
    return ;
}

void RDCMainWindow::onClientVerifyPasswordSlots(QString password)
{
    bool bSuccess = this->verifyPasswordRecursive(password, 3);
    emit client_verify_result_signal(bSuccess);
    return ;
}

void RDCMainWindow::on_actionPreferences_triggered()
{
    RDCPreferencesDialog* prefDlg = new RDCPreferencesDialog(this);
    prefDlg->show();
    return ;
}
