#ifndef RDCMAINWINDOW_H
#define RDCMAINWINDOW_H

#include <QTimer>
#include <QSize>
#include <QMainWindow>

namespace Ui {
class RDCMainWindow;
}

class QThread;
class RDCClient;
class QCloseEvent;
class RDCScreenWindow;
class RDCMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RDCMainWindow(QWidget *parent = 0);
    ~RDCMainWindow();

private slots:
    void startConnection(void);
    void onClientInfoUpdated(QString, QString);
    void onClientShowMessage(int, QString);
    void onClientVerifyPasswordSlots(QString);
    void onClientConnectionReadySlots(QString);
    void onClientResolutionSlots(QSize);
    void on_doConnectionButton_clicked(void);
    void on_doDisconnectionButton_clicked(void);
    void on_otherTokenLineEdit_textChanged(const QString &);
    void on_actionPreferences_triggered(void);

signals:
    void client_do_connection_signal(QString);
    void client_question_choose_signal(bool);
    void client_verify_result_signal(bool);

private:
    void closeEvent(QCloseEvent*);
    bool verifyPasswordRecursive(QString, int);

private:
    Ui::RDCMainWindow *ui;
    RDCClient* m_pClient;
    QThread* m_pClientThread;
    QTimer* m_pScreenTimer;
    RDCScreenWindow* m_pScreenWindow;
};

#endif // RDCMAINWINDOW_H
