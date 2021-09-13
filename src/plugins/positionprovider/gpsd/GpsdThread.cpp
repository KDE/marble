// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
//

#include "GpsdThread.h"

#include <QMetaType>

#include "GpsdConnection.h"

namespace Marble
{

GpsdThread::GpsdThread() : m_connection( nullptr )
{
    // nothing to do
}

GpsdThread::~GpsdThread()
{
    delete m_connection;
}

void GpsdThread::run()
{
    qRegisterMetaType<gps_data_t>( "gps_data_t" );
    qRegisterMetaType<PositionProviderStatus>("PositionProviderStatus");
    m_connection = new GpsdConnection;
    connect( m_connection, SIGNAL(statusChanged(PositionProviderStatus)),
             this, SIGNAL(statusChanged(PositionProviderStatus)) );
    connect( m_connection, SIGNAL(gpsdInfo(gps_data_t)),
             this, SIGNAL(gpsdInfo(gps_data_t)) );
    m_connection->initialize();
    exec();
}

QString GpsdThread::error() const
{
    return m_connection->error();
}

} // namespace Marble

#include "moc_GpsdThread.cpp"
