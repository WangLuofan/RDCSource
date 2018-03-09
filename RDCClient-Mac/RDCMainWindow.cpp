#include "RDCMainWindow.h"
#include "ui_RDCMainWindow.h"

RDCMainWindow::RDCMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RDCMainWindow)
{
    ui->setupUi(this);
}

RDCMainWindow::~RDCMainWindow()
{
    delete ui;
}
