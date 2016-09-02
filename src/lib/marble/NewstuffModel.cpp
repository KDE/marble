//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#include "NewstuffModel.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleZipReader.h"

#include <QUrl>
#include <QVector>
#include <QTemporaryFile>
#include <QDir>
#include <QFuture>
#include <QPair>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QProcessEnvironment>
#include <QMutexLocker>
#include <QIcon>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDomDocument>

namespace Marble
{

class NewstuffItem
{
public:
    QString m_category;
    QString m_name;
    QString m_author;
    QString m_license;
    QString m_summary;
    QString m_version;
    QString m_releaseDate;
    QUrl m_previewUrl;
    QIcon m_preview;
    QUrl m_payloadUrl;
    QDomNode m_registryNode;
    qint64 m_payloadSize;
    qint64 m_downloadedSize;

    NewstuffItem();

    QString installedVersion() const;
    QString installedReleaseDate() const;
    bool isUpgradable() const;
    QStringList installedFiles() const;

    static bool deeperThan( const QString &one, const QString &two );
};

class FetchPreviewJob;

class NewstuffModelPrivate
{
public:
    enum NodeAction {
        Append,
        Replace
    };

    enum UserAction {
        Install,
        Uninstall
    };

    typedef QPair<int, UserAction> Action;

    NewstuffModel* m_parent;

    QVector<NewstuffItem> m_items;

    QNetworkAccessManager m_networkAccessManager;

    QString m_provider;

    QMap<QNetworkReply *, FetchPreviewJob *> m_networkJobs;

    QNetworkReply* m_currentReply;

    QTemporaryFile* m_currentFile;

    QString m_targetDirectory;

    QString m_registryFile;

    NewstuffModel::IdTag m_idTag;

    QDomDocument m_registryDocument;

    QDomElement m_root;

    Action m_currentAction;

    QProcess* m_unpackProcess;

    QMutex m_mutex;

    QList<Action> m_actionQueue;

    QHash<int, QByteArray> m_roleNames;

    explicit NewstuffModelPrivate( NewstuffModel* parent );

    QIcon preview( int index );
    void setPreview( int index, const QIcon &previewIcon );

    void handleProviderData( QNetworkReply* reply );

    static bool canExecute( const QString &executable );

    void installMap();

    void updateModel();

    void saveRegistry();

    void uninstall( int index );

    static void changeNode( QDomNode &node, QDomDocument &domDocument, const QString &key, const QString &value, NodeAction action );

    void readInstalledFiles( QStringList* target, const QDomNode &node );

    void processQueue();

    static NewstuffItem importNode( const QDomNode &node );

    bool isTransitioning( int index ) const;

    void unzip();

    void updateRegistry(const QStringList &files);

    template<class T>
    static void readValue( const QDomNode &node, const QString &key, T* target );
};

class FetchPreviewJob
{
public:
    FetchPreviewJob( NewstuffModelPrivate *modelPrivate, int index );

    void run( const QByteArray &data );

private:
    NewstuffModelPrivate *const m_modelPrivate;
    const int m_index;
};

NewstuffItem::NewstuffItem() : m_payloadSize( -2 ), m_downloadedSize( 0 )
{
    // nothing to do
}

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

FetchPreviewJob::FetchPreviewJob( NewstuffModelPrivate *modelPrivate, int index ) :
    m_modelPrivate( modelPrivate ),
    m_index( index )
{
}

void FetchPreviewJob::run( const QByteArray &data )
{
    const QImage image = QImage::fromData( data );

    if ( image.isNull() )
        return;

    const QPixmap pixmap = QPixmap::fromImage( image );
    const QIcon previewIcon( pixmap );
    m_modelPrivate->setPreview( m_index, previewIcon );
}

NewstuffModelPrivate::NewstuffModelPrivate( NewstuffModel* parent ) : m_parent( parent ),
    m_networkAccessManager( 0 ), m_currentReply( 0 ), m_currentFile( 0 ),
    m_idTag( NewstuffModel::PayloadTag ), m_currentAction( -1, Install ), m_unpackProcess( 0 )
{
    // nothing to do
}

QIcon NewstuffModelPrivate::preview( int index )
{
    if ( m_items.at( index ).m_preview.isNull() ) {
        QPixmap dummyPixmap( 136, 136 );
        dummyPixmap.fill( Qt::transparent );
        setPreview( index, QIcon( dummyPixmap ) );
        QNetworkReply *reply = m_networkAccessManager.get( QNetworkRequest( m_items.at( index ).m_previewUrl ) );
        m_networkJobs.insert( reply, new FetchPreviewJob( this, index ) );
    }

    Q_ASSERT( !m_items.at( index ).m_preview.isNull() );

    return m_items.at( index ).m_preview;
}

void NewstuffModelPrivate::setPreview( int index, const QIcon &previewIcon )
{
    NewstuffItem &item = m_items[index];
    item.m_preview = previewIcon;
    const QModelIndex affected = m_parent->index( index );
    emit m_parent->dataChanged( affected, affected );
}

void NewstuffModelPrivate::handleProviderData(QNetworkReply *reply)
{
    if ( reply->operation() == QNetworkAccessManager::HeadOperation ) {
        const QVariant redirectionAttribute = reply->attribute( QNetworkRequest::RedirectionTargetAttribute );
        if ( !redirectionAttribute.isNull() ) {
            for ( int i=0; i<m_items.size(); ++i ) {
                NewstuffItem &item = m_items[i];
                if ( item.m_payloadUrl == reply->url() ) {
                    item.m_payloadUrl = redirectionAttribute.toUrl();
                }
            }
            m_networkAccessManager.head( QNetworkRequest( redirectionAttribute.toUrl() ) );
            return;
        }

        QVariant const size = reply->header( QNetworkRequest::ContentLengthHeader );
        if ( size.isValid() ) {
            qint64 length = size.value<qint64>();
            for ( int i=0; i<m_items.size(); ++i ) {
                NewstuffItem &item = m_items[i];
                if ( item.m_payloadUrl == reply->url() ) {
                    item.m_payloadSize = length;
                    QModelIndex const affected = m_parent->index( i );
                    emit m_parent->dataChanged( affected, affected );
                }
            }
        }
        return;
    }

    FetchPreviewJob *const job = m_networkJobs.take( reply );

    // check if we are redirected
    const QVariant redirectionAttribute = reply->attribute( QNetworkRequest::RedirectionTargetAttribute );
    if ( !redirectionAttribute.isNull() ) {
        QNetworkReply *redirectReply = m_networkAccessManager.get( QNetworkRequest( QUrl( redirectionAttribute.toUrl() ) ) );
        if ( job ) {
            m_networkJobs.insert( redirectReply, job );
        }
        return;
    }

    if ( job ) {
        job->run( reply->readAll() );
        delete job;
        return;
    }

    QDomDocument xml;
    if ( !xml.setContent( reply->readAll() ) ) {
        mDebug() << "Cannot parse newstuff xml data ";
        return;
    }

    m_items.clear();

    QDomElement root = xml.documentElement();
    QDomNodeList items = root.elementsByTagName( "stuff" );
    for (int i=0 ; i < items.length(); ++i ) {
        m_items << importNode( items.item( i ) );
    }

    updateModel();
}

bool NewstuffModelPrivate::canExecute( const QString &executable )
{
    QString path = QProcessEnvironment::systemEnvironment().value(QStringLiteral("PATH"), QStringLiteral("/usr/local/bin:/usr/bin:/bin"));
    foreach( const QString &dir, path.split( QLatin1Char( ':' ) ) ) {
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
    } else if ( m_currentFile->fileName().endsWith( QLatin1String( "zip" ) ) ) {
        unzip();
    }
    else if ( m_currentFile->fileName().endsWith( QLatin1String( "tar.gz" ) ) && canExecute( "tar" ) ) {
        m_unpackProcess = new QProcess;
        QObject::connect( m_unpackProcess, SIGNAL(finished(int)),
                          m_parent, SLOT(contentsListed(int)) );
        QStringList arguments = QStringList() << "-t" << "-z" << "-f" << m_currentFile->fileName();
        m_unpackProcess->setWorkingDirectory( m_targetDirectory );
        m_unpackProcess->start( "tar", arguments );
    } else {
        if ( !m_currentFile->fileName().endsWith( QLatin1String( "tar.gz" ) ) ) {
            mDebug() << "Can only handle tar.gz files";
        } else {
            mDebug() << "Cannot extract archive: tar executable not found in PATH.";
        }
    }
}

void NewstuffModelPrivate::unzip()
{
    MarbleZipReader zipReader(m_currentFile->fileName());
    QStringList files;
    foreach(const MarbleZipReader::FileInfo &fileInfo, zipReader.fileInfoList()) {
        files << fileInfo.filePath;
    }
    updateRegistry(files);
    zipReader.extractAll(m_targetDirectory);
    m_parent->mapInstalled(0);
}

void NewstuffModelPrivate::updateModel()
{
    QDomNodeList items = m_root.elementsByTagName( "stuff" );
    for (int i=0 ; i < items.length(); ++i ) {
        QString const key = m_idTag == NewstuffModel::PayloadTag ? "payload" : "name";
        QDomNodeList matches = items.item( i ).toElement().elementsByTagName( key );
        if ( matches.size() == 1 ) {
            QString const value = matches.at( 0 ).toElement().text();
            bool found = false;
            for ( int j=0; j<m_items.size() && !found; ++j ) {
                NewstuffItem &item = m_items[j];
                if ( m_idTag == NewstuffModel::PayloadTag && item.m_payloadUrl.toString() == value ) {
                    item.m_registryNode = items.item( i );
                    found = true;
                }
                if ( m_idTag == NewstuffModel::NameTag && item.m_name == value ) {
                    item.m_registryNode = items.item( i );
                    found = true;
                }
            }

            if ( !found ) {
                // Not found in newstuff or newstuff not there yet
                NewstuffItem item = importNode( items.item( i ) );
                if ( m_idTag == NewstuffModel::PayloadTag ) {
                    item.m_registryNode = items.item( i );
                } else if ( m_idTag == NewstuffModel::NameTag ) {
                    item.m_registryNode = items.item( i );
                }
                m_items << item;
            }
        }
    }

    m_parent->beginResetModel();
    m_parent->endResetModel();
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
        if (file.endsWith(QLatin1Char('/'))) {
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
        *target = T(matches.at( 0 ).toElement().text());
    } else {
        for ( int i=0; i<matches.size(); ++i ) {
            if ( matches.at( i ).attributes().contains(QStringLiteral("lang")) &&
                 matches.at( i ).attributes().namedItem(QStringLiteral("lang")).toAttr().value() == QLatin1String("en")) {
                *target = T(matches.at( i ).toElement().text());
                return;
            }
        }
    }
}

NewstuffModel::NewstuffModel( QObject *parent ) :
    QAbstractListModel( parent ), d( new NewstuffModelPrivate( this ) )
{
    setTargetDirectory(MarbleDirs::localPath() + QLatin1String("/maps"));
    // no default registry file

    connect( &d->m_networkAccessManager, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(handleProviderData(QNetworkReply*)) );

    QHash<int,QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[Name] = "name";
    roles[Author] = "author";
    roles[License] = "license";
    roles[Summary] = "summary";
    roles[Version] = "version";
    roles[ReleaseDate] = "releasedate";
    roles[Preview] = "preview";
    roles[Payload] = "payload";
    roles[InstalledVersion] = "installedversion";
    roles[InstalledReleaseDate] = "installedreleasedate";
    roles[InstalledFiles] = "installedfiles";
    roles[IsInstalled] = "installed";
    roles[IsUpgradable] = "upgradable";
    roles[Category] = "category";
    roles[IsTransitioning] = "transitioning";
    roles[PayloadSize] = "size";
    roles[DownloadedSize] = "downloaded";
    d->m_roleNames = roles;
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
        case Qt::DecorationRole: return d->preview( index.row() );
        case Name: return d->m_items.at( index.row() ).m_name;
        case Author: return d->m_items.at( index.row() ).m_author;
        case License: return d->m_items.at( index.row() ).m_license;
        case Summary: return d->m_items.at( index.row() ).m_summary;
        case Version: return d->m_items.at( index.row() ).m_version;
        case ReleaseDate: return d->m_items.at( index.row() ).m_releaseDate;
        case Preview: return d->m_items.at( index.row() ).m_previewUrl;
        case Payload: return d->m_items.at( index.row() ).m_payloadUrl;
        case InstalledVersion: return d->m_items.at( index.row() ).installedVersion();
        case InstalledReleaseDate: return d->m_items.at( index.row() ).installedReleaseDate();
        case InstalledFiles: return d->m_items.at( index.row() ).installedFiles();
        case IsInstalled: return !d->m_items.at( index.row() ).m_registryNode.isNull();
        case IsUpgradable: return d->m_items.at( index.row() ).isUpgradable();
        case Category: return d->m_items.at( index.row() ).m_category;
        case IsTransitioning: return d->isTransitioning( index.row() );
        case PayloadSize: {
            qint64 const size = d->m_items.at( index.row() ).m_payloadSize;
            QUrl const url = d->m_items.at( index.row() ).m_payloadUrl;
            if ( size < -1 && !url.isEmpty() ) {
                d->m_items[index.row()].m_payloadSize = -1; // prevent several head requests for the same item
                d->m_networkAccessManager.head( QNetworkRequest( url ) );
            }

            return qMax<qint64>( -1, size );
        }
        case DownloadedSize: return d->m_items.at( index.row() ).m_downloadedSize;
        }
    }

    return QVariant();
}

QHash<int, QByteArray> NewstuffModel::roleNames() const
{
    return d->m_roleNames;
}


int NewstuffModel::count() const
{
    return rowCount();
}

void NewstuffModel::setProvider( const QString &downloadUrl )
{
    if ( downloadUrl == d->m_provider ) {
        return;
    }

    d->m_provider = downloadUrl;
    emit providerChanged();
    d->m_networkAccessManager.get( QNetworkRequest( QUrl( downloadUrl ) ) );
}

QString NewstuffModel::provider() const
{
    return d->m_provider;
}

void NewstuffModel::setTargetDirectory( const QString &targetDirectory )
{
    if ( targetDirectory != d->m_targetDirectory ) {
        QFileInfo targetDir( targetDirectory );
        if ( !targetDir.exists() ) {
            if ( !QDir::root().mkpath( targetDir.absoluteFilePath() ) ) {
                qDebug() << "Failed to create directory " << targetDirectory << ", newstuff installation might fail.";
            }
        }

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
    if (registryFile.startsWith(QLatin1Char('~')) && registryFile.length() > 1) {
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
    if ( index < 0 || index >= d->m_items.size() ) {
        return;
    }

    NewstuffModelPrivate::Action action( index, NewstuffModelPrivate::Install );
    { // <-- do not remove, mutex locker scope
        QMutexLocker locker( &d->m_mutex );
        if ( d->m_actionQueue.contains( action ) ) {
            return;
        }
        d->m_actionQueue << action;
    }

    d->processQueue();
}

void NewstuffModel::uninstall( int idx )
{
    if ( idx < 0 || idx >= d->m_items.size() ) {
        return;
    }

    if ( d->m_items[idx].m_registryNode.isNull() ) {
        emit uninstallationFinished( idx );
    }

    NewstuffModelPrivate::Action action( idx, NewstuffModelPrivate::Uninstall );
    { // <-- do not remove, mutex locker scope
        QMutexLocker locker( &d->m_mutex );
        if ( d->m_actionQueue.contains( action ) ) {
            return;
        }
        d->m_actionQueue << action;
    }

    d->processQueue();
}

void NewstuffModel::cancel( int index )
{
    if ( !d->isTransitioning( index ) ) {
        return;
    }

    { // <-- do not remove, mutex locker scope
        QMutexLocker locker( &d->m_mutex );
        if ( d->m_currentAction.first == index ) {
            if ( d->m_currentAction.second == NewstuffModelPrivate::Install ) {
                if ( d->m_currentReply ) {
                    d->m_currentReply->abort();
                    d->m_currentReply->deleteLater();
                    d->m_currentReply = 0;
                }

                if ( d->m_unpackProcess ) {
                    d->m_unpackProcess->terminate();
                    d->m_unpackProcess->deleteLater();
                    d->m_unpackProcess = 0;
                }

                if ( d->m_currentFile ) {
                    d->m_currentFile->deleteLater();
                    d->m_currentFile = 0;
                }

                d->m_items[d->m_currentAction.first].m_downloadedSize = 0;

                emit installationFailed( d->m_currentAction.first, tr( "Installation aborted by user." ) );
                d->m_currentAction = NewstuffModelPrivate::Action( -1, NewstuffModelPrivate::Install );
            } else {
                // Shall we interrupt this?
            }
        } else {
            if ( d->m_currentAction.second == NewstuffModelPrivate::Install ) {
                NewstuffModelPrivate::Action install( index, NewstuffModelPrivate::Install );
                d->m_actionQueue.removeAll( install );
                emit installationFailed( index, tr( "Installation aborted by user." ) );
            } else {
                NewstuffModelPrivate::Action uninstall( index, NewstuffModelPrivate::Uninstall );
                d->m_actionQueue.removeAll( uninstall );
                emit uninstallationFinished( index ); // do we need failed here?
            }
        }
    }

    d->processQueue();
}

void NewstuffModel::updateProgress( qint64 bytesReceived, qint64 bytesTotal )
{
    qreal const progress = qBound<qreal>( 0.0, 0.9 * bytesReceived / qreal( bytesTotal ), 1.0 );
    emit installationProgressed( d->m_currentAction.first, progress );
    NewstuffItem &item = d->m_items[d->m_currentAction.first];
    item.m_payloadSize = bytesTotal;
    if ( qreal(bytesReceived-item.m_downloadedSize)/bytesTotal >= 0.01 || progress >= 0.9 ) {
        // Only consider download progress of 1% and more as a data change
        item.m_downloadedSize = bytesReceived;
        QModelIndex const affected = index( d->m_currentAction.first );
        emit dataChanged( affected, affected );
    }
}

void NewstuffModel::retrieveData()
{
    if ( d->m_currentReply && d->m_currentReply->isReadable() ) {
        // check if we are redirected
        const QVariant redirectionAttribute = d->m_currentReply->attribute( QNetworkRequest::RedirectionTargetAttribute );
        if ( !redirectionAttribute.isNull() ) {
            d->m_currentReply = d->m_networkAccessManager.get( QNetworkRequest( redirectionAttribute.toUrl() ) );
            QObject::connect( d->m_currentReply, SIGNAL(readyRead()), this, SLOT(retrieveData()) );
            QObject::connect( d->m_currentReply, SIGNAL(readChannelFinished()), this, SLOT(retrieveData()) );
            QObject::connect( d->m_currentReply, SIGNAL(downloadProgress(qint64,qint64)),
                              this, SLOT(updateProgress(qint64,qint64)) );
        } else {
            d->m_currentFile->write( d->m_currentReply->readAll() );
            if ( d->m_currentReply->isFinished() ) {
                d->m_currentReply->deleteLater();
                d->m_currentReply = 0;
                d->m_currentFile->flush();
                d->installMap();
            }
        }
    }
}

void NewstuffModel::mapInstalled( int exitStatus )
{
    if ( d->m_unpackProcess ) {
        d->m_unpackProcess->deleteLater();
        d->m_unpackProcess = 0;
    }

    if ( d->m_currentFile ) {
        d->m_currentFile->deleteLater();
        d->m_currentFile = 0;
    }

    emit installationProgressed( d->m_currentAction.first, 1.0 );
    d->m_items[d->m_currentAction.first].m_downloadedSize = 0;
    if ( exitStatus == 0 ) {
        emit installationFinished( d->m_currentAction.first );
    } else {
        mDebug() << "Process exit status " << exitStatus << " indicates an error.";
        emit installationFailed( d->m_currentAction.first , QString( "Unable to unpack file. Process exited with status code %1." ).arg( exitStatus ) );
    }
    QModelIndex const affected = index( d->m_currentAction.first );

    { // <-- do not remove, mutex locker scope
        QMutexLocker locker( &d->m_mutex );
        d->m_currentAction = NewstuffModelPrivate::Action( -1, NewstuffModelPrivate::Install );
    }
    emit dataChanged( affected, affected );
    d->processQueue();
}

void NewstuffModel::mapUninstalled()
{
    QModelIndex const affected = index( d->m_currentAction.first );
    emit uninstallationFinished( d->m_currentAction.first );

    { // <-- do not remove, mutex locker scope
        QMutexLocker locker( &d->m_mutex );
        d->m_currentAction = NewstuffModelPrivate::Action( -1, NewstuffModelPrivate::Install );
    }
    emit dataChanged( affected, affected );
    d->processQueue();
}

void NewstuffModel::contentsListed( int exitStatus )
{
    if ( exitStatus == 0 ) {
        QStringList const files = QString(d->m_unpackProcess->readAllStandardOutput()).split(QLatin1Char('\n'), QString::SkipEmptyParts);
        d->updateRegistry(files);

        QObject::disconnect( d->m_unpackProcess, SIGNAL(finished(int)),
                             this, SLOT(contentsListed(int)) );
        QObject::connect( d->m_unpackProcess, SIGNAL(finished(int)),
                          this, SLOT(mapInstalled(int)) );
        QStringList arguments = QStringList() << "-x" << "-z" << "-f" << d->m_currentFile->fileName();
        d->m_unpackProcess->start( "tar", arguments );
    } else {
        mDebug() << "Process exit status " << exitStatus << " indicates an error.";
        emit installationFailed( d->m_currentAction.first , QString( "Unable to list file contents. Process exited with status code %1." ).arg( exitStatus ) );

        { // <-- do not remove, mutex locker scope
            QMutexLocker locker( &d->m_mutex );
            d->m_currentAction = NewstuffModelPrivate::Action( -1, NewstuffModelPrivate::Install );
        }
        d->processQueue();
    }
}

void NewstuffModelPrivate::updateRegistry(const QStringList &files)
{
    emit m_parent->installationProgressed( m_currentAction.first, 0.92 );
    if ( !m_registryFile.isEmpty() ) {
        NewstuffItem &item = m_items[m_currentAction.first];
        QDomNode node = item.m_registryNode;
        NewstuffModelPrivate::NodeAction action = node.isNull() ? NewstuffModelPrivate::Append : NewstuffModelPrivate::Replace;
        if ( node.isNull() ) {
            node = m_root.appendChild( m_registryDocument.createElement( "stuff" ) );
        }

        node.toElement().setAttribute( "category", m_items[m_currentAction.first].m_category );
        changeNode( node, m_registryDocument, "name", item.m_name, action );
        changeNode( node, m_registryDocument, "providerid", m_provider, action );
        changeNode( node, m_registryDocument, "author", item.m_author, action );
        changeNode( node, m_registryDocument, "homepage", QString(), action );
        changeNode( node, m_registryDocument, "licence", item.m_license, action );
        changeNode( node, m_registryDocument, "version", item.m_version, action );
        QString const itemId = m_idTag == NewstuffModel::PayloadTag ? item.m_payloadUrl.toString() : item.m_name;
        changeNode( node, m_registryDocument, "id", itemId, action );
        changeNode( node, m_registryDocument, "releasedate", item.m_releaseDate, action );
        changeNode( node, m_registryDocument, "summary", item.m_summary, action );
        changeNode( node, m_registryDocument, "changelog", QString(), action );
        changeNode( node, m_registryDocument, "preview", item.m_previewUrl.toString(), action );
        changeNode( node, m_registryDocument, "previewBig", item.m_previewUrl.toString(), action );
        changeNode( node, m_registryDocument, "payload", item.m_payloadUrl.toString(), action );
        changeNode( node, m_registryDocument, "status", "installed", action );
        m_items[m_currentAction.first].m_registryNode = node;

        bool hasChildren = true;
        while ( hasChildren ) {
            /** @todo FIXME: fileList does not contain all elements opposed to what docs say */
            QDomNodeList fileList = node.toElement().elementsByTagName( "installedfile" );
            hasChildren = !fileList.isEmpty();
            for ( int i=0; i<fileList.count(); ++i ) {
                node.removeChild( fileList.at( i ) );
            }
        }

        foreach( const QString &file, files ) {
            QDomNode fileNode = node.appendChild( m_registryDocument.createElement( "installedfile" ) );
            fileNode.appendChild(m_registryDocument.createTextNode(m_targetDirectory + QLatin1Char('/') + file));
        }

        saveRegistry();
    }
}

void NewstuffModelPrivate::processQueue()
{
    if ( m_actionQueue.empty() || m_currentAction.first >= 0 ) {
        return;
    }

    { // <-- do not remove, mutex locker scope
        QMutexLocker locker( &m_mutex );
        m_currentAction = m_actionQueue.takeFirst();
    }
    if ( m_currentAction.second == Install ) {
        if ( !m_currentFile ) {
            QFileInfo const file = m_items.at( m_currentAction.first ).m_payloadUrl.path();
            m_currentFile = new QTemporaryFile(QDir::tempPath() + QLatin1String("/marble-XXXXXX-") + file.fileName());
        }

        if ( m_currentFile->open() ) {
            QUrl const payload = m_items.at( m_currentAction.first ).m_payloadUrl;
            m_currentReply = m_networkAccessManager.get( QNetworkRequest( payload ) );
            QObject::connect( m_currentReply, SIGNAL(readyRead()), m_parent, SLOT(retrieveData()) );
            QObject::connect( m_currentReply, SIGNAL(readChannelFinished()), m_parent, SLOT(retrieveData()) );
            QObject::connect( m_currentReply, SIGNAL(downloadProgress(qint64,qint64)),
                              m_parent, SLOT(updateProgress(qint64,qint64)) );
            /** @todo: handle download errors */
        } else {
            mDebug() << "Failed to write to " << m_currentFile->fileName();
        }
    } else {
        // Run in a separate thread to keep the ui responsive
        QFutureWatcher<void>* watcher = new QFutureWatcher<void>( m_parent );
        QObject::connect( watcher, SIGNAL(finished()), m_parent, SLOT(mapUninstalled()) );
        QObject::connect( watcher, SIGNAL(finished()), watcher, SLOT(deleteLater()) );

        QFuture<void> future = QtConcurrent::run( this, &NewstuffModelPrivate::uninstall, m_currentAction.first );
        watcher->setFuture( future );
    }
}

NewstuffItem NewstuffModelPrivate::importNode(const QDomNode &node)
{
    NewstuffItem item;
    item.m_category = node.attributes().namedItem(QStringLiteral("category")).toAttr().value();
    readValue<QString>( node, "name", &item.m_name );
    readValue<QString>( node, "author", &item.m_author );
    readValue<QString>( node, "licence", &item.m_license );
    readValue<QString>( node, "summary", &item.m_summary );
    readValue<QString>( node, "version", &item.m_version );
    readValue<QString>( node, "releasedate", &item.m_releaseDate );
    readValue<QUrl>( node, "preview", &item.m_previewUrl );
    readValue<QUrl>( node, "payload", &item.m_payloadUrl );
    return item;
}

bool NewstuffModelPrivate::isTransitioning( int index ) const
{
    if ( m_currentAction.first == index ) {
        return true;
    }

    foreach( const Action &action, m_actionQueue ) {
        if ( action.first == index ) {
            return true;
        }
    }

    return false;
}

}

#include "moc_NewstuffModel.cpp"
