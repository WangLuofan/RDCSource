#include "RDCServer.h"
#include "RDCMainWindow.h"
#include "RDCPreferencesDialog.h"
#include "ui_RDCMainWindow.h"
#include "RDCClientInfo.h"
#include "RDCClientInfoList.h"

#include <QTimer>
#include <QThread>
#include <QMessageBox>
#include <QStandardItemModel>

RDCMainWindow::RDCMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RDCMainWindow)
{
    ui->setupUi(this);
    this->setupTableView();

    QTimer::singleShot(1000, this, SLOT(startServer()));
}

void RDCMainWindow::setupTableView(void)
{
    this->m_pStandardItemModel = new QStandardItemModel(this);

    this->m_pStandardItemModel->setColumnCount(8);

    this->m_pStandardItemModel->setHeaderData(0, Qt::Horizontal, tr("主机名"), Qt::DisplayRole);
    this->m_pStandardItemModel->setHeaderData(1, Qt::Horizontal, tr("IP地址"), Qt::DisplayRole);
    this->m_pStandardItemModel->setHeaderData(2, Qt::Horizontal, tr("端口号"), Qt::DisplayRole);
    this->m_pStandardItemModel->setHeaderData(3, Qt::Horizontal, tr("系统信息"), Qt::DisplayRole);
    this->m_pStandardItemModel->setHeaderData(4, Qt::Horizontal, tr("Token"), Qt::DisplayRole);
    this->m_pStandardItemModel->setHeaderData(5, Qt::Horizontal, tr("Password"), Qt::DisplayRole);
    this->m_pStandardItemModel->setHeaderData(6, Qt::Horizontal, tr("当前状态"), Qt::DisplayRole);
    this->m_pStandardItemModel->setHeaderData(7, Qt::Horizontal, tr("上线时间"), Qt::DisplayRole);

    ui->clientTableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->clientTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->clientTableView->setModel(this->m_pStandardItemModel);

    return ;
}

void RDCMainWindow::closeEvent(QCloseEvent *)
{
    this->m_pServer->Stop();
    this->m_pServerThread->exit();
    return ;
}


RDCMainWindow::~RDCMainWindow()
{
    delete ui;
    delete this->m_pServer;
    delete this->m_pServerThread;
}

void RDCMainWindow::on_actionPreferences_triggered()
{
    RDCPreferencesDialog* prefDlg = new RDCPreferencesDialog(this);
    prefDlg->show();
    return ;
}

void RDCMainWindow::clientConnectedSignalHandler(void)
{

    return ;
}

void RDCMainWindow::startServer(void)
{
    //启动服务器
    this->m_pServer = new RDCServer();
    this->m_pServerThread = new QThread(this);

    QObject::connect(this->m_pServer, SIGNAL(client_connected_signal()), this, SLOT(clientConnectedSignalHandler()));
    QObject::connect(this->m_pServerThread, SIGNAL(started()), this->m_pServer, SLOT(Start()));

    this->m_pServer->moveToThread(this->m_pServerThread);
    this->m_pServerThread->start();
    return ;
}
