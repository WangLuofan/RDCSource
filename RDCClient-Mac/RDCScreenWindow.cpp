#include "RDCScreenWindow.h"
#include "ui_RDCScreenWindow.h"

#include <QPainter>
#include <QPaintEvent>

RDCScreenWindow::RDCScreenWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::RDCScreenWindow)
{
    ui->setupUi(this);
}

RDCScreenWindow::~RDCScreenWindow()
{
    delete ui;
}

void RDCScreenWindow::updateScreenImage(QImage image)
{
    this->m_pScreenImage = image;
    return this->repaint();
}

void RDCScreenWindow::paintEvent(QPaintEvent* paintEvent)
{
    QPainter painter(this);
    painter.drawImage(0, 0, this->m_pScreenImage);

    return ;
}
