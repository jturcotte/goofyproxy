/******************************************************************************
**
** Copyright 2009 Jocelyn Turcotte
**
** This file is part of GoofyProxy.
**
** GoofyProxy is free software: you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the
** Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** GoofyProxy is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
** FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
** for more details.
**
** You should have received a copy of the GNU General Public License along
** with GoofyProxy. If not, see http://www.gnu.org/licenses/.
**
******************************************************************************/

#include "main.h"
#include <iostream>
#include <QTcpServer>
#include <QTcpSocket>
#include "httpforwardjobthread.h"


Program::Program(qint16 listenPort, const QString& contentTypePrefix, const QString& transformCmd, const QStringList& transformCmdArgs)
: contentTypePrefix_(contentTypePrefix)
, transformCmd_(transformCmd)
, transformCmdArgs_(transformCmdArgs)
, tcpServer_(new QTcpServer(this))
{
    qDebug() << QString("Proxy listening on port [%1] and convert Content-Type [%2] using command [%3]")
                .arg(listenPort)
                .arg(contentTypePrefix)
                .arg(transformCmd + " " + transformCmdArgs.join(" "));

    connect(tcpServer_, SIGNAL(newConnection()), this, SLOT(onTcpServerNewConnection()));
    tcpServer_->listen(QHostAddress::Any, listenPort);
}


void Program::onTcpServerNewConnection()
{
    QTcpSocket* socket = NULL;
    while ((socket = tcpServer_->nextPendingConnection()) != NULL)
    {
        QThread* thread = new HttpForwardJobThread(socket, contentTypePrefix_, transformCmd_, transformCmdArgs_);
        connect(thread, SIGNAL(finished()), this, SLOT(onThreadFinished()));

        thread->start();
    }
}


void Program::onThreadFinished()
{
    delete sender();
}


void printHelp()
{
    std::cout << "Usage: goofyproxy PORT CONTENTTYPEPREFIX COMMAND [COMMANDARGS]" << std::endl;
    std::cout << "Open an HTTP proxy on port PORT." << std::endl;
    std::cout << "Each response with a Content-Type that starts with CONTENTTYPEPREFIX gets " << std::endl;
    std::cout << "its payload sent through COMMAND's stdin. Stdout is sent back to the client." << std::endl;
    std::cout << std::endl;
    std::cout << "Example: goofyproxy 8080 text/html sed '{s/<p\\([> ]\\)/<h1\\1/; s/<\\/p>/<\\/h1>/}'" << std::endl;
    std::cout << "         would change every occurence of <p></p> for <h1></h1> in" << std::endl;
    std::cout << "         html objects requested through the proxy." << std::endl;
}


int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printHelp();
        return 1;
    }

    bool listenPortParsed;
    qint16 listenPort = QString(argv[1]).toUInt(&listenPortParsed);
    if (!listenPortParsed)
    {
        qFatal("Could not parse port number [%s]", argv[1]);
        printHelp();
        return 1;
    }

    QString contentTypePrefix = argv[2];
    QString transformCmd = argv[3];

    QStringList transformCmdArgs;
    for (int i = 4; i < argc; ++i)
        transformCmdArgs.append(argv[i]);


    QCoreApplication a(argc, argv);
    Program program(listenPort, contentTypePrefix, transformCmd, transformCmdArgs);
    return a.exec();
}


