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

#include "httpforwardjob.h"

#include <QBuffer>
#include <QHttp>
#include <QHttpResponseHeader>
#include <QTcpSocket>
#include <QUrl>

#include "cmdpipefilter.h"
#include "defaultfilter.h"


HttpForwardJob::HttpForwardJob(QTcpSocket* requestingSocket
                              ,const QString& contentTypePrefix
                              ,const QString& transformCmd
                              ,const QStringList& transformCmdArgs)
: requestingSocket_(requestingSocket)
, contentTypePrefix_(contentTypePrefix)
, transformCmd_(transformCmd)
, transformCmdArgs_(transformCmdArgs)
, destinationHttpClient_(NULL)
, forwardedHttpRequestId_(0)
, dstHttpClientDataBuffer_(NULL)
{
    connect(requestingSocket, SIGNAL(readyRead()), this, SLOT(onRequestSocketReadyRead()));
    connect(requestingSocket, SIGNAL(disconnected()), this, SLOT(onRequestSocketDisconnected()));
    connect(requestingSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onRequestSocketError(QAbstractSocket::SocketError)));
}


void HttpForwardJob::onRequestSocketReadyRead()
{
    QByteArray headerSeeker = requestingSocket_->peek(4096);
    int endOfHeadersPos = headerSeeker.indexOf("\r\n\r\n");
    if (endOfHeadersPos != -1)
    {
        // Remove header from buffer
        requestingSocket_->read(endOfHeadersPos+4);

        QHttpRequestHeader headers(headerSeeker.mid(0, endOfHeadersPos));

        QUrl destUrl = QUrl::fromEncoded(headers.path().toAscii());
        QByteArray newPath = destUrl.encodedPath();
        if (destUrl.hasQuery())
            newPath += '?' + destUrl.encodedQuery();

        headers.setRequest(headers.method(), newPath, headers.majorVersion(), headers.minorVersion());
        headers.removeValue("Accept-Encoding");     // To prevent receiving compressed content
        headers.removeValue("Proxy-Connection");    // Not relevant for server

        int hostPort = destUrl.port();

        destinationHttpClient_ = new QHttp(this);
        destinationHttpClient_->setHost(destUrl.host(), hostPort != -1 ? hostPort : 80);
        connect(destinationHttpClient_, SIGNAL(requestFinished(int,bool)), this, SLOT(onDestinationHttpClientRequestFinished(int,bool)));

        dstHttpClientDataBuffer_ = new QBuffer(this);
        dstHttpClientDataBuffer_->open(QIODevice::ReadWrite | QIODevice::Unbuffered);

        connect(destinationHttpClient_, SIGNAL(responseHeaderReceived(QHttpResponseHeader)), this, SLOT(onDestinationHttpClientResponseHeaderReceived(QHttpResponseHeader)));

        requestPath_ = destUrl.toString().toAscii();
        if (requestPath_.length() > 70)
        {
            requestPath_.truncate(70);
            requestPath_ += "...";
        }

        qDebug() << "+++ " << requestPath_ << ": Forwarding request";

        forwardedHttpRequestId_ = destinationHttpClient_->request(headers, requestingSocket_, dstHttpClientDataBuffer_);
    }
}


void HttpForwardJob::onRequestSocketDisconnected()
{
    done();
}


void HttpForwardJob::onRequestSocketError(QAbstractSocket::SocketError error)
{
    qDebug() << "!!! " << requestPath_ << ": Error received:" << error;
}


void HttpForwardJob::onHttpFilterFinished()
{
    qDebug() << "    " << requestPath_ << ": Request finished, closing client socket";// << (const char*)(flushedSome ? " and flushed some bytes" : "");
    requestingSocket_->disconnectFromHost();
}


void HttpForwardJob::onDestinationHttpClientResponseHeaderReceived(const QHttpResponseHeader& resp)
{
    QHttpResponseHeader modifiedHeaders(resp);
    // Since the size of the body may change, remove the content-length field and close the connection when finished
    modifiedHeaders.removeValue("Content-Length");
    modifiedHeaders.setValue("Connection", "close");

    // Ugly: Disable chunked transfer in client since QHttp dechunked received data.
    if (modifiedHeaders.value("Transfer-Encoding") == "chunked")
        modifiedHeaders.removeValue("Transfer-Encoding");

    AHttpFilter* filter = NULL;
    if (resp.contentType().startsWith(contentTypePrefix_))
        filter = new CmdPipeFilter(this, dstHttpClientDataBuffer_, requestingSocket_, transformCmd_, transformCmdArgs_);
    else
        filter = new DefaultFilter(this, dstHttpClientDataBuffer_, requestingSocket_);

    connect(filter, SIGNAL(finished()), this, SLOT(onHttpFilterFinished()));
    filter->handleResponse(modifiedHeaders);
}


void HttpForwardJob::onDestinationHttpClientRequestFinished(int requestId, bool /*error*/)
{
    if (requestId == forwardedHttpRequestId_)
    {
        dstHttpClientDataBuffer_->close();
    }
}
