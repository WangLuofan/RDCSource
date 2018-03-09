#ifndef RDCMAINWINDOW_H
#define RDCMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class RDCMainWindow;
}

class RDCMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RDCMainWindow(QWidget *parent = 0);
    ~RDCMainWindow();

private:
    Ui::RDCMainWindow *ui;
};

#endif // RDCMAINWINDOW_H
