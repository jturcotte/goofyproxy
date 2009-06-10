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

#include "defaultfilter.h"

#include <QBuffer>
#include <QDebug>
#include <QHttpResponseHeader>


DefaultFilter::DefaultFilter(QObject* parent, QBuffer* src, QIODevice* dst)
: AHttpFilter(parent)
, src_(src)
, dst_(dst)
{
}


void DefaultFilter::handleResponse(const QHttpResponseHeader& resp)
{
    dst_->write(resp.toString().toAscii());

    connect(src_, SIGNAL(aboutToClose()), this, SLOT(onSrcAboutToClose()));
    connect(src_, SIGNAL(readyRead()), this, SLOT(onSrcReadyRead()));

    // Catch up on any readyRead() signal we may have missed
    onSrcReadyRead();
}


void DefaultFilter::onSrcAboutToClose()
{
    finished();
}


void DefaultFilter::onSrcReadyRead()
{
    src_->seek(0);

    dst_->write(src_->readAll());

    src_->buffer().resize(0);
    src_->seek(0);
}

