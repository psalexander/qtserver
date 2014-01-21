#include "dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    initGUI();
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

    mLogServerEdit->append(tr("Connected:"));
    mLogServerEdit->append(tr("Adress: %1").arg(clientConnection->peerAddress().toString()));
    mLogServerEdit->append(tr("Name: %1").arg(clientConnection->peerName()));
    mLogServerEdit->append(tr("Port: %1").arg(clientConnection->peerPort()));

    //clientConnection->write(block);
    clientConnection->disconnectFromHost();
}
