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
}

void Dialog::startServer()
{
    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
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
    //QByteArray block;
    //QDataStream out(&block, QIODevice::WriteOnly);
    //out.setVersion(QDataStream::Qt_4_0);
    //out << (quint16)0;
    //out << fortunes.at(qrand() % fortunes.size());
    //out.device()->seek(0);
    //out << (quint16)(block.size() - sizeof(quint16));

    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();

    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));

    connect(clientConnection, SIGNAL(readyRead()), this, SLOT(onReadFromSocket()));

    mLogServerEdit->append(tr("Connected:"));
    mLogServerEdit->append(tr("Adress: %1").arg(clientConnection->peerAddress().toString()));
    mLogServerEdit->append(tr("Name: %1").arg(clientConnection->peerName()));
    mLogServerEdit->append(tr("Port: %1").arg(clientConnection->peerPort()));

    //clientConnection->write(block);

    //clientConnection->disconnectFromHost();
}
float X = 100;
float Y = 100;
void Dialog::onReadFromSocket()
{
    mLogServerEdit->append(tr("onReadFromSocket"));
//    QTcpSocket *tcpSocket = (QTcpSocket*)sender();
//    QDataStream clientReadStream(tcpSocket);
//    QString str1;
//    clientReadStream >> str1;
//    mLogServerEdit->append(tr("Data1: %1").arg(str1));



    qint64 bytesExpected=0;
    QByteArray buffer;
    QTcpSocket *tcpSocket = (QTcpSocket*)sender();
//    if (bytesExpected == 0 && tcpSocket->bytesAvailable() >= sizeof(bytesExpected)) {
//        tcpSocket->read((char *)&bytesExpected, sizeof(bytesExpected));
//        qDebug() << "Expecting:" << &bytesExpected;
//    }

    if (/*bytesExpected > 0 &&*/ tcpSocket->bytesAvailable() > 0) {
        QByteArray chunk = tcpSocket->read(qMax(bytesExpected, tcpSocket->bytesAvailable()));
        buffer += chunk;
        bytesExpected -= chunk.size();
       // qDebug() << "Received chunk of:" << chunk.size();

        QString res = buffer.data();
        res = res.trimmed();
        qDebug() << "res:" << res;
        if(res.at(0)=='M' && res.at(res.size()-1)==';'){

            QStringList ls = res.split(QRegExp("[,;]"));
            qDebug() << "ls size:" << ls.size();
            float x = ls.at(1).toDouble();
            float y = ls.at(2).toDouble();
          //  X += x;
          //  Y += y;

            QCursor::setPos (QCursor::pos().x() + x, QCursor::pos().y() + y);
            qDebug() << tr("X: %1; Y: %2").arg(QString::number(X)).arg(QString::number(Y)) ;//"Received block of size: " << buffer.size();
            //QMouseEvent* event = new QMouseEvent( QEvent::MouseMove, QPoint( X, Y ), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
           // QApplication::postEvent(QApplication::desktop(), event);
        //this->mouseMoveEvent(event);
        }else if(res.startsWith("CLICK")){
            QTest::keyClick(QApplication::desktop(),Qt::LeftButton);
            qDebug() << tr("CLICK!");
//            QMouseEvent *klik = new QMouseEvent(QEvent::MouseButtonPress, QCursor::pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
//            QCoreApplication::postEvent(QApplication::desktop(), klik);
//            QMouseEvent* klik2 = new QMouseEvent(QEvent::MouseButtonRelease, QCursor::pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
//            QCoreApplication::postEvent(QApplication::desktop(), klik2);
        }




        mLogServerEdit->append(tr("Data: %1").arg(buffer.data()));
        if (bytesExpected == 0) {
           // qDebug() << "Received block of size:" << buffer.size();
           // qDebug() << "Bytes left in socket:" << tcpSocket->bytesAvailable();

            //tcpSocket->deleteLater();
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
//        mLogServerEdit->append(tr("Data: %1").arg(str));
//        next_block_size = 0;
//    }
}
