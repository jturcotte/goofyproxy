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


#ifndef MAIN_H
#define MAIN_H

#include <QtCore/QCoreApplication>
#include <QStringList>
#include <QTcpServer>


class Program : public QObject
{
    Q_OBJECT
public:
    Program(qint16 listenPort, const QString& contentTypePrefix, const QString& transformCmd, const QStringList& transformCmdArgs);

private slots:
    void onTcpServerNewConnection();
    void onThreadFinished();

private:
    QString localHost_;
    QString contentTypePrefix_;
    QString transformCmd_;
    QStringList transformCmdArgs_;

    QTcpServer* tcpServer_;
};

#endif // MAIN_H
