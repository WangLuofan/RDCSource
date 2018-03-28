#include "RDCScreenWindow.h"
#include "ui_RDCScreenWindow.h"

RDCScreenWindow::RDCScreenWindow(QWidget *parent) :
    QMainWindow(parent), m_pImageBuffer(nullptr),
    ui(new Ui::RDCScreenWindow)
{
    ui->setupUi(this);
}

RDCScreenWindow::~RDCScreenWindow()
{
    delete ui;
}

void RDCScreenWindow::onClientResolutionSlots(QSize resolution)
{
    if(this->m_pImageBuffer != nullptr)
    {
        free(this->m_pImageBuffer);
        this->m_pImageBuffer = nullptr;
    }

    int bytes_count = resolution.width() * resolution.height() * 4;
    this->m_pImageBuffer = (unsigned char*)malloc(sizeof(unsigned char) * bytes_count);
    memset(this->m_pImageBuffer, 0, sizeof(unsigned char) * bytes_count);

    return ;
}
