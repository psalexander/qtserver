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
#include <QMouseEvent>
#include <QEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QCursor>
#include <QScreen>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <QX11Info>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <QPointF>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QMediaResource>
#include <QMediaContent>

#include "simulationmouseevent.h"

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

    QMediaPlayer *player;

    QTcpServer *tcpServer;
    QStringList fortunes;
    QNetworkSession *networkSession;
    void sessionOpened();
    void sendFortune();

    void initGUI();
    void startServer();
    void stopServer();
    void mouseClick(int);
    void mouseScroll(int);
public slots:
    void onClickStartButton();
    void onClickStopButton();

    void onNewConnection();
    void onReadFromSocket();
};

#endif // DIALOG_H
