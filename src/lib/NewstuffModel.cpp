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
#include <QtCore/QFuture>
#include <QtCore/QFutureWatcher>
#include <QtCore/QtConcurrentRun>
#include <QtCore/QProcessEnvironment>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>

namespace Marble
{

class NewstuffItem
{
public:
    QString m_category;
    QString m_name;
    QString m_author;
    QString m_licence;
    QString m_summary;
    QString m_version;
    QString m_releaseDate;
    QUrl m_preview;
    QUrl m_payload;
    QDomNode m_registryNode;

    QString installedVersion() const;
    QString installedReleaseDate() const;
    bool isUpgradable() const;
    QStringList installedFiles() const;

    static bool deeperThan( const QString &one, const QString &two );
};

class NewstuffModelPrivate
{
public:
    enum NodeAction {
        Append,
        Replace
    };

    NewstuffModel* m_parent;

    QVector<NewstuffItem> m_items;

    QNetworkAccessManager* m_networkAccessManager;

    QString m_provider;

    QNetworkReply* m_currentReply;

    QTemporaryFile* m_currentFile;

    QString m_targetDirectory;

    QString m_registryFile;

    NewstuffModel::IdTag m_idTag;

    QDomDocument m_registryDocument;

    QDomElement m_root;

    int m_currentIndex;

    QProcess* m_unpackProcess;

    NewstuffModelPrivate( NewstuffModel* parent );

    void handleProviderData( QNetworkReply* reply );

    bool canExecute( const QString &executable ) const;

    void installMap();

    void updateModel();

    void saveRegistry();

    void uninstall( int index );

    void changeNode( QDomNode &node, QDomDocument &domDocument, const QString &key, const QString &value, NodeAction action );

    void readInstalledFiles( QStringList* target, const QDomNode &node );

    template<class T>
    void readValue( const QDomNode &node, const QString &key, T* target );
};

QString NewstuffItem::installedVersion() const
{
    QDomNodeList const nodes = m_registryNode.toElement().elementsByTagName( "version" );
    if ( nodes.size() == 1 ) {
        return nodes.at( 0 ).toElement().text();
    }

    return QString();
}

QString NewstuffItem::installedReleaseDate() const
{
    QDomNodeList const nodes = m_registryNode.toElement().elementsByTagName( "releasedate" );
    if ( nodes.size() == 1 ) {
        return nodes.at( 0 ).toElement().text();
    }

    return QString();
}

bool NewstuffItem::isUpgradable() const
{
    bool installedOk, remoteOk;
    double const installed = installedVersion().toDouble( &installedOk );
    double const remote= m_version.toDouble( &remoteOk );
    return installedOk && remoteOk && remote > installed;
}

QStringList NewstuffItem::installedFiles() const
{
    QStringList result;
    QDomNodeList const nodes = m_registryNode.toElement().elementsByTagName( "installedfile" );
    for ( int i=0; i<nodes.count(); ++i ) {
        result << nodes.at( i ).toElement().text();
    }
    return result;
}

bool NewstuffItem::deeperThan(const QString &one, const QString &two)
{
    return one.length() > two.length();
}

NewstuffModelPrivate::NewstuffModelPrivate( NewstuffModel* parent ) : m_parent( parent ),
    m_networkAccessManager( 0 ), m_currentReply( 0 ), m_currentFile( 0 ),
    m_idTag( NewstuffModel::PayloadTag ), m_currentIndex( -1 ), m_unpackProcess( 0 )
{
    m_targetDirectory = QDir::home().filePath( ".local/share/marble/maps" );
    // no default registry file
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
        item.m_category = node.attributes().namedItem( "category" ).toAttr().value();
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

    updateModel();
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
                 m_parent, SLOT( contentsListed( int ) ) );
        QStringList arguments = QStringList() << "-t" << "-z" << "-f" << m_currentFile->fileName();
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

void NewstuffModelPrivate::updateModel()
{
    QDomNodeList items = m_root.elementsByTagName( "stuff" );
    for ( unsigned int i = 0; i < items.length(); ++i ) {
        QString const key = m_idTag == NewstuffModel::PayloadTag ? "payload" : "name";
        QDomNodeList matches = items.item( i ).toElement().elementsByTagName( key );
        if ( matches.size() == 1 ) {
            QString const value = matches.at( 0 ).toElement().text();
            for ( int j=0; j<m_items.size(); ++j ) {
                NewstuffItem &item = m_items[j];
                if ( m_idTag == NewstuffModel::PayloadTag && item.m_payload == value ) {
                    item.m_registryNode = items.item( i );
                    break;
                }
                if ( m_idTag == NewstuffModel::NameTag && item.m_name == value ) {
                    item.m_registryNode = items.item( i );
                    break;
                }
            }
        }
    }

    m_parent->reset();
}

void NewstuffModelPrivate::saveRegistry()
{
    QFile output( m_registryFile );
    if ( !output.open( QFile::WriteOnly ) ) {
        mDebug() << "Cannot open " << m_registryFile << " for writing";
    } else {
        QTextStream outStream( &output );
        outStream << m_registryDocument.toString( 2 );
        outStream.flush();
        output.close();
    }
}

void NewstuffModelPrivate::uninstall( int index )
{
    // Delete all files first, then directories (deeper ones first)

    QStringList directories;
    QStringList const files = m_items[index].installedFiles();
    foreach( const QString &file, files ) {
        if ( file.endsWith( '/' ) ) {
            directories << file;
        } else {
            QFile::remove( file );
        }
    }

    qSort( directories.begin(), directories.end(), NewstuffItem::deeperThan );
    foreach( const QString &dir, directories ) {
        QDir::root().rmdir( dir );
    }

    m_items[index].m_registryNode.parentNode().removeChild( m_items[index].m_registryNode );
    m_items[index].m_registryNode.clear();
    saveRegistry();
}

void NewstuffModelPrivate::changeNode( QDomNode &node, QDomDocument &domDocument, const QString &key, const QString &value, NodeAction action )
{
    if ( action == Append ) {
        QDomNode newNode = node.appendChild( domDocument.createElement( key ) );
        newNode.appendChild( domDocument.createTextNode( value ) );
    } else {
        QDomNode oldNode = node.namedItem( key );
        if ( !oldNode.isNull() ) {
            oldNode.removeChild( oldNode.firstChild() );
            oldNode.appendChild( domDocument.createTextNode( value ) );
        }
    }
}

template<class T>
void NewstuffModelPrivate::readValue( const QDomNode &node, const QString &key, T* target )
{
    QDomNodeList matches = node.toElement().elementsByTagName( key );
    if ( matches.size() == 1 ) {
        *target = matches.at( 0 ).toElement().text();
    } else {
        for ( int i=0; i<matches.size(); ++i ) {
            if ( matches.at( i ).attributes().contains( "lang" ) &&
                 matches.at( i ).attributes().namedItem( "lang").toAttr().value() == "en" ) {
                *target = matches.at( i ).toElement().text();
                return;
            }
        }
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
    roles[InstalledVersion] = "installedversion";
    roles[InstalledReleaseDate] = "installedreleasedate";
    roles[IsInstalled] = "installed";
    roles[IsUpgradable] = "upgradable";
    roles[Category] = "category";
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
        case InstalledVersion: return d->m_items.at( index.row() ).installedVersion();
        case InstalledReleaseDate: return d->m_items.at( index.row() ).installedReleaseDate();
        case IsInstalled: return !d->m_items.at( index.row() ).m_registryNode.isNull();
        case IsUpgradable: return d->m_items.at( index.row() ).isUpgradable();
        case Category: return d->m_items.at( index.row() ).m_category;
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

void NewstuffModel::setRegistryFile( const QString &filename, IdTag idTag )
{
    QString registryFile = filename;
    if ( registryFile.startsWith( '~' ) && registryFile.length() > 1 ) {
        registryFile = QDir::homePath() + registryFile.mid( 1 );
    }

    if ( d->m_registryFile != registryFile ) {
        d->m_registryFile = registryFile;
        d->m_idTag = idTag;
        emit registryFileChanged();

        QFileInfo inputFile( registryFile );
        if ( !inputFile.exists() ) {
            QDir::root().mkpath( inputFile.absolutePath() );
            d->m_registryDocument = QDomDocument( "khotnewstuff3" );
            QDomProcessingInstruction header = d->m_registryDocument.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"utf-8\"" );
            d->m_registryDocument.appendChild( header );
            d->m_root = d->m_registryDocument.createElement( "hotnewstuffregistry" );
            d->m_registryDocument.appendChild( d->m_root );
        } else {
            QFile input( registryFile );
            if ( !input.open( QFile::ReadOnly ) ) {
                mDebug() << "Cannot open newstuff registry " << registryFile;
                return;
            }

            if ( !d->m_registryDocument.setContent( &input ) ) {
                mDebug() << "Cannot parse newstuff registry " << registryFile;
                return;
            }
            input.close();
            d->m_root = d->m_registryDocument.documentElement();
        }

        d->updateModel();
    }
}

QString NewstuffModel::registryFile() const
{
    return d->m_registryFile;
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

void NewstuffModel::uninstall( int idx )
{
    if ( idx < 0 || idx >= d->m_items.size() || d->m_items[idx].m_registryNode.isNull() ) {
        return;
    }

    // Run in a separate thread to keep the ui responsive
    d->m_currentIndex = idx;
    QFutureWatcher<void>* watcher = new QFutureWatcher<void>( this );
    connect( watcher, SIGNAL( finished() ), this, SLOT( mapUninstalled() ) );
    connect( watcher, SIGNAL( finished() ), watcher, SLOT( deleteLater() ) );

    QFuture<void> future = QtConcurrent::run( d, &NewstuffModelPrivate::uninstall, idx );
    watcher->setFuture( future );
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
        QModelIndex const affected = index( d->m_currentIndex );
        emit dataChanged( affected, affected );
    } else {
        mDebug() << "Process exit status " << exitStatus << " indicates an error.";
        emit installationFailed( d->m_currentIndex , QString( "Unable to unpack file. Process exited with status code %1." ).arg( exitStatus ) );
    }

    d->m_currentIndex = -1;
}

void NewstuffModel::mapUninstalled()
{
    QModelIndex const affected = index( d->m_currentIndex );
    emit dataChanged( affected, affected );
    emit uninstallationFinished( d->m_currentIndex );
    d->m_currentIndex = -1;
}

void NewstuffModel::contentsListed( int exitStatus )
{
    emit installationProgressed( d->m_currentIndex, 0.92 );
    if ( exitStatus == 0 ) {
        if ( !d->m_registryFile.isEmpty() ) {
            NewstuffItem &item = d->m_items[d->m_currentIndex];
            QDomNode node = item.m_registryNode;
            NewstuffModelPrivate::NodeAction action = node.isNull() ? NewstuffModelPrivate::Append : NewstuffModelPrivate::Replace;
            if ( node.isNull() ) {
                node = d->m_root.appendChild( d->m_registryDocument.createElement( "stuff" ) );
            }

            node.toElement().setAttribute( "category", d->m_items[d->m_currentIndex].m_category );
            d->changeNode( node, d->m_registryDocument, "name", item.m_name, action );
            d->changeNode( node, d->m_registryDocument, "providerid", d->m_provider, action );
            d->changeNode( node, d->m_registryDocument, "author", item.m_author, action );
            d->changeNode( node, d->m_registryDocument, "homepage", QString(), action );
            d->changeNode( node, d->m_registryDocument, "licence", item.m_licence, action );
            d->changeNode( node, d->m_registryDocument, "version", item.m_version, action );
            QString const itemId = d->m_idTag == PayloadTag ? item.m_payload.toString() : item.m_name;
            d->changeNode( node, d->m_registryDocument, "id", itemId, action );
            d->changeNode( node, d->m_registryDocument, "releasedate", item.m_releaseDate, action );
            d->changeNode( node, d->m_registryDocument, "summary", item.m_summary, action );
            d->changeNode( node, d->m_registryDocument, "changelog", QString(), action );
            d->changeNode( node, d->m_registryDocument, "preview", item.m_preview.toString(), action );
            d->changeNode( node, d->m_registryDocument, "previewBig", item.m_preview.toString(), action );
            d->changeNode( node, d->m_registryDocument, "payload", item.m_payload.toString(), action );
            d->changeNode( node, d->m_registryDocument, "status", "installed", action );
            d->m_items[d->m_currentIndex].m_registryNode = node;

            bool hasChildren = true;
            while ( hasChildren ) {
                /** @todo FIXME: fileList does not contain all elements opposed to what docs say */
                QDomNodeList fileList = node.toElement().elementsByTagName( "installedfile" );
                hasChildren = !fileList.isEmpty();
                for ( int i=0; i<fileList.count(); ++i ) {
                    node.removeChild( fileList.at( i ) );
                }
            }

            QStringList const files = QString( d->m_unpackProcess->readAllStandardOutput() ).split( '\n', QString::SkipEmptyParts );
            foreach( const QString &file, files ) {
                QDomNode fileNode = node.appendChild( d->m_registryDocument.createElement( "installedfile" ) );
                fileNode.appendChild( d->m_registryDocument.createTextNode( d->m_targetDirectory + "/" + file ) );
            }

            d->saveRegistry();
        }

        QObject::disconnect( d->m_unpackProcess, SIGNAL( finished( int ) ),
                 this, SLOT( contentsListed( int ) ) );
        QObject::connect( d->m_unpackProcess, SIGNAL( finished( int ) ),
                 this, SLOT( mapInstalled( int ) ) );
        QStringList arguments = QStringList() << "-x" << "-z" << "-f" << d->m_currentFile->fileName();
        d->m_unpackProcess->start( "tar", arguments );
    } else {
        mDebug() << "Process exit status " << exitStatus << " indicates an error.";
        emit installationFailed( d->m_currentIndex , QString( "Unable to list file contents. Process exited with status code %1." ).arg( exitStatus ) );
    }
}

}

#include "NewstuffModel.moc"
