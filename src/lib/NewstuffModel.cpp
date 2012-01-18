//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "NewstuffModel.h"

#include "MarbleDebug.h"

#include <QtCore/QUrl>
#include <QtCore/QVector>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDir>
#include <QtCore/QProcessEnvironment>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>

namespace Marble
{

class NewstuffItem
{
public:
    QString m_name;
    QString m_author;
    QString m_licence;
    QString m_summary;
    QString m_version;
    QString m_releaseDate;
    QUrl m_preview;
    QUrl m_payload;
};

class NewstuffModelPrivate
{
public:
    NewstuffModel* m_parent;

    QVector<NewstuffItem> m_items;

    QNetworkAccessManager* m_networkAccessManager;

    QString m_provider;

    QNetworkReply* m_currentReply;

    QTemporaryFile* m_currentFile;

    QString m_targetDirectory;

    int m_currentIndex;

    QProcess* m_unpackProcess;

    NewstuffModelPrivate( NewstuffModel* parent );

    void handleProviderData( QNetworkReply* reply );

    bool canExecute( const QString &executable ) const;

    void installMap();

    template<class T>
    void readValue( const QDomNode &node, const QString &key, T* target );
};

NewstuffModelPrivate::NewstuffModelPrivate( NewstuffModel* parent ) : m_parent( parent ),
    m_networkAccessManager( 0 ), m_currentReply( 0 ), m_currentFile( 0 ),
    m_currentIndex( -1 ), m_unpackProcess( 0 )
{
    m_targetDirectory = QDir::home().filePath( ".local/share/marble/maps" );
}

void NewstuffModelPrivate::handleProviderData(QNetworkReply *reply)
{
    QDomDocument xml;
    if ( !xml.setContent( reply->readAll() ) ) {
        mDebug() << "Cannot parse newstuff xml data ";
        return;
    }

    m_items.clear();

    QDomElement root = xml.documentElement();
    QDomNodeList items = root.elementsByTagName( "stuff" );
    for ( unsigned int i = 0; i < items.length(); ++i ) {
        NewstuffItem item;
        QDomNode node = items.item( i );
        readValue<QString>( node, "name", &item.m_name );
        readValue<QString>( node, "author", &item.m_author );
        readValue<QString>( node, "licence", &item.m_licence );
        readValue<QString>( node, "summary", &item.m_summary );
        readValue<QString>( node, "version", &item.m_version );
        readValue<QString>( node, "releasedate", &item.m_releaseDate );
        readValue<QUrl>( node, "preview", &item.m_preview );
        readValue<QUrl>( node, "payload", &item.m_payload );
        m_items << item;
    }

    m_parent->reset();
}

bool NewstuffModelPrivate::canExecute( const QString &executable ) const
{
    QString path = QProcessEnvironment::systemEnvironment().value( "PATH", "/usr/local/bin:/usr/bin:/bin" );
    foreach( const QString &dir, path.split( ":" ) ) {
        QFileInfo application( QDir( dir ), executable );
        if ( application.exists() ) {
            return true;
        }
    }

    return false;
}

void NewstuffModelPrivate::installMap()
{
    if ( m_unpackProcess ) {
        m_unpackProcess->close();
        delete m_unpackProcess;
        m_unpackProcess = 0;
    } else if ( m_currentFile->fileName().endsWith( "tar.gz" ) && canExecute( "tar" ) ) {
        m_unpackProcess = new QProcess;
        QObject::connect( m_unpackProcess, SIGNAL( finished( int ) ),
                 m_parent, SLOT( mapInstalled( int ) ) );
        QStringList arguments = QStringList() << "-x" << "-z" << "-f" << m_currentFile->fileName();
        m_unpackProcess->setWorkingDirectory( m_targetDirectory );
        m_unpackProcess->start( "tar", arguments );
    } else {
        if ( !m_currentFile->fileName().endsWith( "tar.gz" ) ) {
            mDebug() << "Can only handle tar.gz files";
        } else {
            mDebug() << "Cannot extract archive: tar executable not found in PATH.";
        }
    }
}

template<class T>
void NewstuffModelPrivate::readValue( const QDomNode &node, const QString &key, T* target )
{
    QDomNodeList matches = node.toElement().elementsByTagName( key );
    if ( matches.size() == 1 ) {
        *target = matches.at( 0 ).toElement().text();
    }
}

NewstuffModel::NewstuffModel( QObject *parent ) :
    QAbstractListModel( parent ), d( new NewstuffModelPrivate( this ) )
{
    QHash<int,QByteArray> roles = roleNames();
    roles[Name] = "name";
    roles[Author] = "author";
    roles[Licence] = "licence";
    roles[Summary] = "summary";
    roles[Version] = "version";
    roles[ReleaseDate] = "releasedate";
    roles[Preview] = "preview";
    roles[Payload] = "payload";
    setRoleNames( roles );
}

NewstuffModel::~NewstuffModel()
{
    delete d;
}

int NewstuffModel::rowCount ( const QModelIndex &parent ) const
{
    if ( !parent.isValid() ) {
        return d->m_items.size();
    }

    return 0;
}

QVariant NewstuffModel::data ( const QModelIndex &index, int role ) const
{
    if ( index.isValid() && index.row() >= 0 && index.row() < d->m_items.size() ) {
        switch ( role ) {
        case Qt::DisplayRole: return d->m_items.at( index.row() ).m_name;
        case Name: return d->m_items.at( index.row() ).m_name;
        case Author: return d->m_items.at( index.row() ).m_author;
        case Licence: return d->m_items.at( index.row() ).m_licence;
        case Summary: return d->m_items.at( index.row() ).m_summary;
        case Version: return d->m_items.at( index.row() ).m_version;
        case ReleaseDate: return d->m_items.at( index.row() ).m_releaseDate;
        case Preview: return d->m_items.at( index.row() ).m_preview;
        case Payload: return d->m_items.at( index.row() ).m_payload;
        }
    }

    return QVariant();
}

int NewstuffModel::count()
{
    return rowCount();
}

void NewstuffModel::setProvider( const QString &downloadUrl )
{
    if ( downloadUrl == d->m_provider ) {
        return;
    }

    if ( !d->m_networkAccessManager) {
        d->m_networkAccessManager = new QNetworkAccessManager( this );
        connect( d->m_networkAccessManager, SIGNAL( finished( QNetworkReply * ) ),
                 this, SLOT( handleProviderData( QNetworkReply * ) ) );
    }

    d->m_provider = downloadUrl;
    emit providerChanged();
    d->m_networkAccessManager->get( QNetworkRequest( QUrl( downloadUrl ) ) );
}

QString NewstuffModel::provider() const
{
    return d->m_provider;
}

void NewstuffModel::setTargetDirectory( const QString &targetDirectory )
{
    if ( targetDirectory != d->m_targetDirectory ) {
        d->m_targetDirectory = targetDirectory;
        emit targetDirectoryChanged();
    }
}

QString NewstuffModel::targetDirectory() const
{
    return d->m_targetDirectory;
}


void NewstuffModel::install( int index )
{
    if ( index < 0 || index == d->m_currentIndex || index >= d->m_items.size() ) {
        return;
    }

    d->m_currentIndex = index;
    if ( !d->m_currentFile ) {
        QFileInfo const file = d->m_items.at( index ).m_payload.path();
        d->m_currentFile = new QTemporaryFile( QDir::tempPath() + "/marble-XXXXXX-" + file.fileName() );
    }

    if ( d->m_currentFile->open() ) {
        QUrl const payload = d->m_items.at( index ).m_payload;
        d->m_currentReply = d->m_networkAccessManager->get( QNetworkRequest( payload ) );
        connect( d->m_currentReply, SIGNAL( readyRead() ), this, SLOT( retrieveData() ) );
        connect( d->m_currentReply, SIGNAL( readChannelFinished() ), this, SLOT( retrieveData() ) );
        connect( d->m_currentReply, SIGNAL( downloadProgress( qint64, qint64 ) ),
                 this, SLOT( updateProgress( qint64, qint64 ) ) );
        /** @todo: handle download errors */
    } else {
        mDebug() << "Failed to write to " << d->m_currentFile->fileName();
    }
}

void NewstuffModel::updateProgress( qint64 bytesReceived, qint64 bytesTotal )
{
    qreal const progress = qBound<qreal>( 0.0, 0.9 * bytesReceived / qreal( bytesTotal ), 1.0 );
    emit installationProgressed( d->m_currentIndex, progress );
}

void NewstuffModel::retrieveData()
{
    if ( d->m_currentReply && d->m_currentReply->isReadable() ) {
        d->m_currentFile->write( d->m_currentReply->readAll() );
        if ( d->m_currentReply->isFinished() ) {
            d->m_currentReply->deleteLater();
            d->m_currentReply = 0;
            d->m_currentFile->flush();
            d->installMap();
        }
    }
}

void NewstuffModel::mapInstalled( int exitStatus )
{
    d->m_unpackProcess->deleteLater();
    d->m_unpackProcess = 0;
    d->m_currentFile->deleteLater();
    d->m_currentFile = 0;

    emit installationProgressed( d->m_currentIndex, 1.0 );
    if ( exitStatus == 0 ) {
        emit installationFinished( d->m_currentIndex );
    } else {
        mDebug() << "Process exit status " << exitStatus << " indicates an error.";
        emit installationFailed( d->m_currentIndex , QString( "Unable to unpack file. Process exited with status code %1." ).arg( exitStatus ) );
    }

    d->m_currentIndex = -1;
}

}

#include "NewstuffModel.moc"
