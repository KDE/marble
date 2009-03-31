//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
//
#include "QNamNetworkPlugin.h"

#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>

#include "QNamDownloadJob.h"

namespace Marble
{

QNamNetworkPlugin::~QNamNetworkPlugin()
{
    qDeleteAll( m_networkAccessManagers );
}

QString QNamNetworkPlugin::name() const
{
    return tr( "QNam Network Plugin" );
}

QString QNamNetworkPlugin::nameId() const
{
    return QString::fromLatin1( "QNamNetworkPlugin" );
}

QString QNamNetworkPlugin::guiString() const
{
    return tr( "QNetworkAccessManager based Network Plugin" );
}

QString QNamNetworkPlugin::description() const
{
    return tr( "A network plugin which keeps connected to the hosts used "
               "to deliver a better user experience." );
}

QIcon QNamNetworkPlugin::icon() const
{
    return QIcon();
}

void QNamNetworkPlugin::initialize()
{
}

bool QNamNetworkPlugin::isInitialized() const
{
    return true;
}

HttpJob * QNamNetworkPlugin::createJob( const QUrl & sourceUrl,
                                        const QString & destination,
                                        const QString & id )
{
    QNetworkAccessManager * qnam = findOrCreateNetworkAccessManager( sourceUrl.host() );
    return new QNamDownloadJob( sourceUrl, destination, id, qnam );
}

NetworkPlugin * QNamNetworkPlugin::create() const
{
    return new QNamNetworkPlugin;
}

QNetworkAccessManager *
QNamNetworkPlugin::findOrCreateNetworkAccessManager( const QString & hostname )
{
    QNetworkAccessManager * result = 0;
    QMap<QString, QNetworkAccessManager *>::const_iterator const pos =
        m_networkAccessManagers.find( hostname );
    if ( pos != m_networkAccessManagers.constEnd() ) {
        qDebug() << "returning existing QNAM for" << hostname;
        result = pos.value();
    } else {
        qDebug() << "creating QNAM for" << hostname;
        result = new QNetworkAccessManager;
        m_networkAccessManagers.insert( hostname, result );
    }
    return result;
}


} // namespace Marble

#include "QNamNetworkPlugin.moc"

Q_EXPORT_PLUGIN2( QNamNetworkPlugin, Marble::QNamNetworkPlugin )
