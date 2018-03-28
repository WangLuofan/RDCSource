#ifndef RDCSCREENWINDOW_H
#define RDCSCREENWINDOW_H

#include <QSize>
#include <QMainWindow>

namespace Ui {
class RDCScreenWindow;
}

class RDCScreenWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RDCScreenWindow(QWidget *parent = 0);
    ~RDCScreenWindow();

private slots:
    void onClientResolutionSlots(QSize);

private:
    Ui::RDCScreenWindow *ui;
    unsigned char* m_pImageBuffer;
};

#endif // RDCSCREENWINDOW_H
