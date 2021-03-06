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

#ifndef CMDPIPEFILTER_H
#define CMDPIPEFILTER_H

#include <QProcess>

#include "ahttpfilter.h"

class QBuffer;
class QIODevice;


class CmdPipeFilter : public AHttpFilter
{
    Q_OBJECT
public:
    CmdPipeFilter(QObject* parent, QBuffer* src, QIODevice* dst, QString command, QStringList args);
    void handleResponse(const QHttpResponseHeader& resp);

private slots:
    void onSrcAboutToClose();
    void onSrcReadyRead();
    void onCmdProcessReadyRead();
    void onCmdProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onCmdProcessError(QProcess::ProcessError error);
private:
    QString command_;
    QStringList args_;
    QBuffer* src_;
    QIODevice* dst_;

    QProcess* cmdProcess_;
};

#endif // CMDPIPEFILTER_H
