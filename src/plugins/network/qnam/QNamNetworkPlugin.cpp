//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
//
#include "QNamNetworkPlugin.h"

#include "MarbleDebug.h"
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>

#include "QNamDownloadJob.h"

namespace Marble
{

QNamNetworkPlugin::QNamNetworkPlugin()
    : m_networkAccessManager( new QNetworkAccessManager )
{
}

QNamNetworkPlugin::~QNamNetworkPlugin()
{
    delete m_networkAccessManager;
}

QString QNamNetworkPlugin::name() const
{
    return tr( "QNam Network Plugin" );
}

QString QNamNetworkPlugin::nameId() const
{
    return QString::fromLatin1( "QNamNetworkPlugin" );
}

QString QNamNetworkPlugin::version() const
{
    return "1.0";
}

QString QNamNetworkPlugin::description() const
{
    return tr( "A network plugin which keeps connected to the hosts used "
               "to deliver a better user experience." );
}

QString QNamNetworkPlugin::copyrightYears() const
{
    return "2009";
}

QList<PluginAuthor> QNamNetworkPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Jens-Michael Hoffmann", "jensmh@gmx.de" );
}

QIcon QNamNetworkPlugin::icon() const
{
    return QIcon();
}

HttpJob * QNamNetworkPlugin::createJob( const QUrl & sourceUrl,
                                        const QString & destination,
                                        const QString & id )
{
    HttpJob * const job = new QNamDownloadJob( sourceUrl, destination, id, m_networkAccessManager );
    job->setUserAgentPluginId( "QNamNetworkPlugin" );
    return job;
}

NetworkPlugin * QNamNetworkPlugin::newInstance() const
{
    return new QNamNetworkPlugin;
}

} // namespace Marble

#include "QNamNetworkPlugin.moc"

Q_EXPORT_PLUGIN2( QNamNetworkPlugin, Marble::QNamNetworkPlugin )
