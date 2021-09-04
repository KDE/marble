/*

    SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
    All rights reserved.
    Contact: Nokia Corporation (qt-info@nokia.com)

    This file is part of the examples of the Qt Toolkit.

    $QT_BEGIN_LICENSE:BSD$
    SPDX-License-Identifier: BSD-3-Clause

*/

#ifndef THREAD_H
#define THREAD_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>

class QSignalMapper;

class Thread: public QThread
{
    Q_OBJECT

public:
    explicit Thread( QObject * const parent = nullptr );
    ~Thread() override;

    void launchWorker( QObject * const worker );
    void stop();

Q_SIGNALS:
    void aboutToStop();
    
private Q_SLOTS:
    void stopExecutor();
    void setReadyStatus();

private:
    QObject * m_worker;
    QSignalMapper * m_shutDownHelper;
    QWaitCondition m_waitCondition;
    QMutex m_mutex;
};

#endif
