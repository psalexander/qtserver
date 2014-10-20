#include "dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    initGUI();
    QApplication::desktop()->setMouseTracking(true);

}

Dialog::~Dialog()
{
    if(mStartServerButton != NULL)
        delete mStartServerButton;
    if(mStopServerButton != NULL)
        delete mStopServerButton;
    if(mLogServerEdit != NULL)
        delete mLogServerEdit;
}

void Dialog::sessionOpened()
{
    // Save the used configuration
    if (networkSession) {
    //    QNetworkConfiguration config = networkSession->configuration();
        QString id;
   //     if (config.type() == QNetworkConfiguration::UserChoice)
   //         id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
   //     else
           // id = config.identifier();
            id = "DefaultNetworkConfiguration";
    }

    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, 28333)) {
        QMessageBox::critical(this, tr("Fortune Server"), tr("Unable to start the server: %1.").arg(tcpServer->errorString()));
        mLogServerEdit->append(tr("Unable to start the server: %1.").arg(tcpServer->errorString()));
        return;
    }
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    mLogServerEdit->append(tr("The server is running on\n\nIP: %1\nport: %2\n\nRun the Fortune Client example now.\n").arg(ipAddress).arg(tcpServer->serverPort()));
}

void Dialog::initGUI()
{
    mLogServerEdit = new QTextEdit(this);
    mStartServerButton = new QPushButton(tr("Start"), this);
    mStopServerButton = new QPushButton(tr("Stop"), this);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(mStartServerButton);
    buttonLayout->addWidget(mStopServerButton);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(mLogServerEdit);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    connect(mStartServerButton, &QPushButton::clicked, this, &Dialog::onClickStartButton);
    connect(mStopServerButton, &QPushButton::clicked, this, &Dialog::onClickStopButton);


    player = new QMediaPlayer(this, QMediaPlayer::StreamPlayback);

    //mainLayout->addWidget(player);

    QVideoWidget* widget = new QVideoWidget;
    widget->show();
    player->setVideoOutput(widget);



    qDebug() << "Qt version:" << QT_VERSION_STR;

    // APIs common to Qt 4 and Qt 5.1
    int screen = QX11Info::appScreen();
        qDebug() << "Screen:" << QX11Info::appScreen();
        qDebug() << "DPI X:" << QX11Info::appDpiX(screen);
        qDebug() << "DPI Y:" << QX11Info::appDpiY(screen);
        qDebug() << "Root window handle:" << QX11Info::appRootWindow(screen);
        qDebug() << "Time:" << QX11Info::appTime();
        qDebug() << "User time:" << QX11Info::appUserTime();
        qDebug() << "Display:" << QX11Info::display();

        // Qt 5.1 APIs
    #if QT_VERSION >= 0x050100
        qDebug() << "XCB connection:" << QX11Info::connection();
    #endif
}

void Dialog::startServer()
{
    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        const QString id = "DefaultNetworkConfiguration";

        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }
        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        mLogServerEdit->append(tr("Opening network session."));
        networkSession->open();
    } else {
        sessionOpened();
    }
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

void Dialog::stopServer()
{
    tcpServer->close();
}

void Dialog::mouseClick(int button)
{
    Display *display = XOpenDisplay(NULL);

    XEvent event;

    if(display == NULL){
        return;
    }

    memset(&event, 0x00, sizeof(event));

    event.type = ButtonPress;
    event.xbutton.button = button;
    event.xbutton.same_screen = True;
    event.xcrossing.focus = True;

    XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

    event.xbutton.subwindow = event.xbutton.window;

    while(event.xbutton.subwindow){
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
    }

    if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0){
        fprintf(stderr, "Errore nell'invio dell'evento !!!\n");
    }

    XFlush(display);
    usleep(100000);

    event.type = ButtonRelease;
    event.xbutton.state = 0x100;

    if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0){
        fprintf(stderr, "Errore nell'invio dell'evento !!!\n");
    }

    XFlush(display);
    XCloseDisplay(display);
}

void Dialog::mouseScroll(int button)
{
    Display *display = XOpenDisplay(NULL);
    XEvent event;
    if(display == NULL){
        return;
    }

    memset(&event, 0x00, sizeof(event));

    event.type = ButtonPress;
    event.xbutton.button = button;
    event.xbutton.same_screen = True;

    XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

    event.xbutton.subwindow = event.xbutton.window;

    while(event.xbutton.subwindow){
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
    }

    if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0){
        fprintf(stderr, "Errore nell'invio dell'evento !!!\n");
    }

    XFlush(display);
    XCloseDisplay(display);
}

void Dialog::onClickStartButton()
{
    mLogServerEdit->append(tr("Start server...\n"));
    startServer();
}

void Dialog::onClickStopButton()
{
    mLogServerEdit->append(tr("Stop server...\n"));
    stopServer();
}

void Dialog::onNewConnection()
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();

    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
    connect(clientConnection, SIGNAL(readyRead()), this, SLOT(onReadFromSocket()));

    mLogServerEdit->append(tr("Connected:"));
    mLogServerEdit->append(tr("Adress: %1").arg(clientConnection->peerAddress().toString()));
    mLogServerEdit->append(tr("Name: %1").arg(clientConnection->peerName()));
    mLogServerEdit->append(tr("Port: %1").arg(clientConnection->peerPort()));

    QMediaResource rss;
    rss.setVideoCodec("H.264");
    QMediaContent mc(rss);
    player->setMedia( mc, clientConnection);

}

float X = 100;
float Y = 100;

void Dialog::onReadFromSocket()
{
 //   mLogServerEdit->append(tr("onReadFromSocket"));
//    QTcpSocket *tcpSocket = (QTcpSocket*)sender();
//    QDataStream clientReadStream(tcpSocket);
//    QString str1;
//    clientReadStream >> str1;
//    mLogServerEdit->append(tr("Data1: %1").arg(str1));


player->play();

    qint64 bytesExpected=0;
    QByteArray buffer;
    QTcpSocket *tcpSocket = (QTcpSocket*)sender();
//    if (bytesExpected == 0 && tcpSocket->bytesAvailable() >= sizeof(bytesExpected)) {
//        tcpSocket->read((char *)&bytesExpected, sizeof(bytesExpected));
//        qDebug() << "Expecting:" << &bytesExpected;
//    }

    player->setMedia(QMediaContent(), tcpSocket);
    if (tcpSocket->canReadLine()){
        player->play();
    }


    //return;

    if (/*bytesExpected > 0 &&*/ tcpSocket->bytesAvailable() > 0) {
        QByteArray chunk = tcpSocket->read(qMax(bytesExpected, tcpSocket->bytesAvailable()));
        buffer += chunk;
        bytesExpected -= chunk.size();
       // qDebug() << "Received chunk of:" << chunk.size();

        QString res = buffer.data();
        res = res.trimmed();

        //qDebug() << "res:" << buffer;
        qDebug() << "buffer size: " << buffer.size();

//        player->setMedia(QMediaContent(), tcpSocket);
//        if (tcpSocket->canReadLine()){
//            player->play();
//        }


        return;

       // qDebug() << "res:" << res;
        if(res.at(0)=='M' && res.at(res.size()-1)==';'){//Move

            QStringList ls = res.split(QRegExp("[,;]"));
            float x = ls.at(1).toDouble();
            float y = ls.at(2).toDouble();

            //QCursor::setPos (QCursor::pos().x() + x, QCursor::pos().y() + y);
            SimulationMouseEvent::moveMouse(QPointF(x,y));

            //qDebug() << tr("X: %1; Y: %2").arg(QString::number(X)).arg(QString::number(Y)) ;//"Received block of size: " << buffer.size();


        }else if(res.startsWith("CLICK")){
           // QTest::keyClick(QApplication::desktop(),Qt::LeftButton);
            //qDebug() << tr("CLICK!");
            //mouseScroll(Button5);
      //      mouseClick(Button1);

            SimulationMouseEvent::leftMouseClick();



  /*          QPointF pointF;
            pointF.setX(QCursor::pos().x());
            pointF.setY(QCursor::pos().y());

            XEvent event;
            memset(&event, 0x00, sizeof(event));
            Display *display = QX11Info::display();
            //XDefaultScreen(display);
            //display = XOpenDisplay("0");

            event.type = ButtonPress;
            event.xbutton.button = Button1;
            event.xbutton.same_screen = True;

            XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

            event.xbutton.subwindow = event.xbutton.window;
            while(event.xbutton.subwindow){
                event.xbutton.window = event.xbutton.subwindow;
                XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
            }

            if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0)
                fprintf(stderr, "Error\n");
            XFlush(display);
*/

            //QWidget *w = QApplication::widgetAt(QCursor::pos());
           // qDebug() << w;
           // if(w != NULL){

           // QMouseEvent *e = new QMouseEvent(QEvent::MouseButtonPress, pointF, Qt::RightButton, Qt::NoButton, Qt::NoModifier);
           // QApplication::postEvent(w, e);
            //QApplication::processEvents();
   //         }
            //            QMouseEvent *klik = new QMouseEvent(QEvent::MouseButtonPress, QCursor::pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
//            QCoreApplication::postEvent(QApplication::desktop(), klik);
//            QMouseEvent* klik2 = new QMouseEvent(QEvent::MouseButtonRelease, QCursor::pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
//            QCoreApplication::postEvent(QApplication::desktop(), klik2);
        }else if(res.startsWith("RC")){//right click
            SimulationMouseEvent::rightMouseClick();

        }else if(res.startsWith("SU")){//scroll up
            SimulationMouseEvent::upMouseScroll();
            qDebug() << "SU";
        }else if(res.startsWith("SD")){//scroll down
            SimulationMouseEvent::downMouseScroll();
            qDebug() << "SD";
        }else if(res.startsWith("RD")){//right down
            //qDebug() << "RD";//
            SimulationMouseEvent::rightMouseDown();
        }else if(res.startsWith("RU")){//right up
           // qDebug() << "RU";
            SimulationMouseEvent::rightMouseUp();
        }else if(res.startsWith("LD")){//left down
           // qDebug() << "LD";
            SimulationMouseEvent::leftMouseDown();
        }else if(res.startsWith("LU")){//left up
          // qDebug() << "LU";
            SimulationMouseEvent::leftMouseUp();
        }


        if (bytesExpected == 0) {
            deleteLater();
        }
    }




//    int next_block_size = 0;
//    while(true) {
//        if (!next_block_size) {
//            if (tcpSocket->bytesAvailable() < sizeof(quint16)) { // are size data available
//                break;
//            }
//            clientReadStream >> next_block_size;
//        }

//        if (tcpSocket->bytesAvailable() < next_block_size) {
//            break;
//        }
//        QString str;
//        clientReadStream >> str;
//        next_block_size = 0;
//    }
}

