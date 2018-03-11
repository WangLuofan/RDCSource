#ifndef RDCMAINWINDOW_H
#define RDCMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class RDCMainWindow;
}

class QThread;
class RDCServer;
class QStandardItemModel;
class RDCMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RDCMainWindow(QWidget *parent = 0);
    ~RDCMainWindow();

private:
    void setupTableView(void);
    void closeEvent(QCloseEvent*);

private slots:
    void on_actionPreferences_triggered();
    void startServer(void);
    void clientConnectedSignalHandler(void);

private:
    Ui::RDCMainWindow *ui;
    QStandardItemModel* m_pStandardItemModel;
    RDCServer* m_pServer;
    QThread* m_pServerThread;
};

#endif // RDCMAINWINDOW_H
