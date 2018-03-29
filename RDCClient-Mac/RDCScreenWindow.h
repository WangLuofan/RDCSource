#ifndef RDCSCREENWINDOW_H
#define RDCSCREENWINDOW_H

#include <QSize>
#include <QImage>
#include <QMainWindow>

namespace Ui {
class RDCScreenWindow;
}

class QPaintEvent;
class RDCScreenWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RDCScreenWindow(QWidget *parent = 0);
    ~RDCScreenWindow();

    void updateScreenImage(QImage);

private:
    void paintEvent(QPaintEvent*);

private:
    Ui::RDCScreenWindow *ui;
    QImage m_pScreenImage;
};

#endif // RDCSCREENWINDOW_H
