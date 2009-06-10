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

#include "httpforwardjobthread.h"

#include <QTcpSocket>

#include "httpforwardjob.h"


HttpForwardJobThread::HttpForwardJobThread(QTcpSocket* requestingSocket, const QString& contentTypePrefix, const QString& transformCmd, const QStringList& transformCmdArgs)
: requestingSocket_(requestingSocket)
, contentTypePrefix_(contentTypePrefix)
, transformCmd_(transformCmd)
, transformCmdArgs_(transformCmdArgs)
{
    // TODO?: connect to get onclose and delete socket
    requestingSocket->setParent(NULL);
    requestingSocket->moveToThread(this);
}


void HttpForwardJobThread::run()
{

    // TODO: connect to get onclose and delete socket
    HttpForwardJob job(requestingSocket_, contentTypePrefix_, transformCmd_, transformCmdArgs_);
    connect(&job, SIGNAL(done()), this, SLOT(quit()));

    requestingSocket_->setParent(&job);

    exec();
}
