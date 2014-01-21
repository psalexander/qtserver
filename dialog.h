#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QTcpServer>
#include <QTcpSocket>
#include <QStringList>
#include <QNetworkSession>
#include <QNetworkConfigurationManager>
#include <QMessageBox>

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);
    ~Dialog();
private:
    QPushButton *mStartServerButton;
    QPushButton *mStopServerButton;
    QTextEdit *mLogServerEdit;

    QTcpServer *tcpServer;
    QStringList fortunes;
    QNetworkSession *networkSession;
    void sessionOpened();
    void sendFortune();

    void initGUI();
    void startServer();
    void stopServer();
public slots:
    void onClickStartButton();
    void onClickStopButton();

    void onNewConnection();
};

#endif // DIALOG_H
