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

#ifndef HTTPFORWARDJOB_H
#define HTTPFORWARDJOB_H

#include <QAbstractSocket>
#include <QObject>
#include <QStringList>

class QBuffer;
class QHttp;
class QHttpResponseHeader;
class QTcpSocket;


class HttpForwardJob : public QObject
{
    Q_OBJECT
public:
    HttpForwardJob(QTcpSocket* requestingSocket, const QString& contentTypePrefix, const QString& transformCmd, const QStringList& transformCmdArgs);

private slots:
    void onRequestSocketReadyRead();
    void onRequestSocketDisconnected();
    void onRequestSocketError(QAbstractSocket::SocketError);
    void onDestinationHttpClientResponseHeaderReceived(const QHttpResponseHeader& resp);
    void onDestinationHttpClientRequestFinished(int requestId, bool error);
    void onHttpFilterFinished();

signals:
    void done();

private:
    QTcpSocket* requestingSocket_;
    QString contentTypePrefix_;
    QString transformCmd_;
    QStringList transformCmdArgs_;

    QHttp* destinationHttpClient_;
    int forwardedHttpRequestId_;

    QBuffer* dstHttpClientDataBuffer_;
    QString requestPath_;
};


#endif // HTTPFORWARDJOB_H
