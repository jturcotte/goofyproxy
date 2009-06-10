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

#include "cmdpipefilter.h"

#include <QBuffer>
#include <QDebug>
#include <QHttpResponseHeader>


CmdPipeFilter::CmdPipeFilter(QObject* parent, QBuffer* src, QIODevice* dst, QString command, QStringList args)
: AHttpFilter(parent)
, command_(command)
, args_(args)
, src_(src)
, dst_(dst)
, cmdProcess_(NULL)
{
}


void CmdPipeFilter::handleResponse(const QHttpResponseHeader& resp)
{
    dst_->write(resp.toString().toAscii());

    cmdProcess_ = new QProcess(this);
    connect(cmdProcess_, SIGNAL(readyRead()), this, SLOT(onCmdProcessReadyRead()));
    connect(cmdProcess_, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onCmdProcessFinished(int,QProcess::ExitStatus)));
    connect(cmdProcess_, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onCmdProcessError(QProcess::ProcessError)));

    cmdProcess_->start(command_, args_);

    connect(src_, SIGNAL(aboutToClose()), this, SLOT(onSrcAboutToClose()));
    connect(src_, SIGNAL(readyRead()), this, SLOT(onSrcReadyRead()));

    // Catch up on any readyRead() signal we may have missed
    onSrcReadyRead();
}


void CmdPipeFilter::onSrcAboutToClose()
{
    onSrcReadyRead();
    cmdProcess_->closeWriteChannel();
}


void CmdPipeFilter::onCmdProcessReadyRead()
{
    dst_->write(cmdProcess_->readAll());
}


void CmdPipeFilter::onCmdProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode != 0 || exitStatus != QProcess::NormalExit)
    {
        qWarning("!!!  Command [%s] exited with code [%d] and exitStatus [%d]", command_.toAscii().data(), exitCode, exitStatus);

        QByteArray stderrText = cmdProcess_->readAllStandardError();
        if (stderrText.length() > 0)
            qWarning("     stderr: %s", stderrText.data());
    }

    finished();
}


void CmdPipeFilter::onCmdProcessError(QProcess::ProcessError error)
{
    qCritical("!!!  Error executing command [%s], ProcessError [%d] returned", command_.toAscii().data(), error);
    finished();
}


void CmdPipeFilter::onSrcReadyRead()
{
    src_->reset();

    cmdProcess_->write(src_->readAll());

    src_->buffer().resize(0);
    src_->reset();
}

