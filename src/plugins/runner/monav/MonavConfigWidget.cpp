//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MonavConfigWidget.h"

#include "MonavMapsModel.h"
#include "MonavPlugin.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"

#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QSignalMapper>
#include <QtGui/QStringListModel>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QShowEvent>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>

namespace Marble
{

class MonavStuffEntry
{
public:
    void setPayload( const QString &payload );

    QString payload() const;

    void setName( const QString &name );

    QString name() const;

    bool isValid() const;

    QString continent() const;

    QString state() const;

    QString region() const;

    QString transport() const;

private:
    QString m_payload;

    QString m_name;

    QString m_continent;

    QString m_state;

    QString m_region;

    QString m_transport;
};

class MonavConfigWidgetPrivate
{
public:
    MonavConfigWidget* m_parent;

    MonavPlugin* m_plugin;

    QNetworkAccessManager* m_networkAccessManager;

    QNetworkReply* m_currentReply;

    QProcess* m_unpackProcess;

    QSortFilterProxyModel* m_filteredModel;

    MonavMapsModel* m_mapsModel;

    bool m_initialized;

    QSignalMapper m_removeMapSignalMapper;

    QSignalMapper m_upgradeMapSignalMapper;

    QVector<MonavStuffEntry> m_remoteMaps;

    QMap<QString, QString> m_remoteVersions;

    QString m_currentDownload;

    QFile m_currentFile;

    QString m_transport;

    MonavConfigWidgetPrivate( MonavConfigWidget* parent, MonavPlugin* plugin );

    void parseNewStuff( const QByteArray &data );

    bool updateContinents( QComboBox* comboBox );

    bool updateStates( const QString &continent, QComboBox* comboBox );

    bool updateRegions( const QString &continent, const QString &state, QComboBox* comboBox );

    MonavStuffEntry map( const QString &continent, const QString &state, const QString &region ) const;

    void install();

    void installMap();

    void updateInstalledMapsView();

    void updateInstalledMapsViewButtons();

    void updateTransportPreference();

    bool canExecute( const QString &executable ) const;

    void setBusy( bool busy, const QString &message = QString() ) const;

private:
    bool fillComboBox( QStringList items, QComboBox* comboBox ) const;
};

void MonavStuffEntry::setPayload( const QString &payload )
{
    m_payload = payload;
}

QString MonavStuffEntry::payload() const
{
    return m_payload;
}

void MonavStuffEntry::setName( const QString &name )
{
    m_name = name;
    QStringList parsed = name.split( "/" );
    int size = parsed.size();
    m_continent = size > 0 ? parsed.at( 0 ).trimmed() : QString();
    m_state = size > 1 ? parsed.at( 1 ).trimmed() : QString();
    m_region = size > 2 ? parsed.at( 2 ).trimmed() : QString();
    m_transport = "Motorcar"; // No i18n

    if ( size > 1 ) {
        QString last = parsed.last().trimmed();
        QRegExp regexp("([^(]+)\\(([^)]+)\\)");
        if ( regexp.indexIn( last ) >= 0 ) {
            QStringList matches = regexp.capturedTexts();
            if ( matches.size() == 3 ) {
                m_transport = matches.at( 2 ).trimmed();
                if ( size > 2 ) {
                    m_region = matches.at( 1 ).trimmed();
                } else {
                    m_state = matches.at( 1 ).trimmed();
                }
            }
        }
    }
}

QString MonavStuffEntry::name() const
{
    return m_name;
}

QString MonavStuffEntry::continent() const
{
    return m_continent;
}

QString MonavStuffEntry::state() const
{
    return m_state;
}

QString MonavStuffEntry::region() const
{
    return m_region;
}

QString MonavStuffEntry::transport() const
{
    return m_transport;
}

bool MonavStuffEntry::isValid() const
{
    return !m_continent.isEmpty() && !m_state.isEmpty() && m_payload.startsWith( "http://" );
}

MonavConfigWidgetPrivate::MonavConfigWidgetPrivate( MonavConfigWidget* parent, MonavPlugin* plugin ) :
        m_parent( parent ), m_plugin( plugin ), m_networkAccessManager( 0 ),
        m_currentReply( 0 ), m_unpackProcess( 0 ), m_filteredModel( new QSortFilterProxyModel( parent) ),
        m_mapsModel( 0 ), m_initialized( false )
{
    m_filteredModel->setFilterKeyColumn( 1 );
}

void MonavConfigWidgetPrivate::parseNewStuff( const QByteArray &data )
{
    QDomDocument xml;
    if ( !xml.setContent( data ) ) {
        mDebug() << "Cannot parse xml file " << data;
        return;
    }

    QDomElement root = xml.documentElement();
    QDomNodeList items = root.elementsByTagName( "stuff" );
    for ( unsigned int i = 0; i < items.length(); ++i ) {
        MonavStuffEntry item;
        QDomNode node = items.item( i );

        QDomNodeList names = node.toElement().elementsByTagName( "name" );
        if ( names.size() == 1 ) {
            item.setName( names.at( 0 ).toElement().text() );
        }

        QString releaseDate;
        QDomNodeList dates = node.toElement().elementsByTagName( "releasedate" );
        if ( dates.size() == 1 ) {
            releaseDate = dates.at( 0 ).toElement().text();
        }

        QString filename;
        QDomNodeList payloads = node.toElement().elementsByTagName( "payload" );
        if ( payloads.size() == 1 ) {
            QString payload = payloads.at( 0 ).toElement().text();
            filename = payload.mid( 1 + payload.lastIndexOf( "/" ) );
            item.setPayload( payload );
        }

        if ( item.isValid() ) {
            m_remoteMaps.push_back( item );
            if ( !filename.isEmpty() && !releaseDate.isEmpty() ) {
                m_remoteVersions[filename] = releaseDate;
            }
        }
    }

    m_mapsModel->setInstallableVersions( m_remoteVersions );
    updateInstalledMapsViewButtons();
}

bool MonavConfigWidgetPrivate::fillComboBox( QStringList items, QComboBox* comboBox ) const
{
    comboBox->clear();
    qSort( items );
    comboBox->addItems( items );
    return !items.isEmpty();
}

bool MonavConfigWidgetPrivate::updateContinents( QComboBox* comboBox )
{
    QSet<QString> continents;
    foreach( const MonavStuffEntry &map, m_remoteMaps ) {
        Q_ASSERT( map.isValid() );
        continents << map.continent();
    }

    return fillComboBox( continents.toList(), comboBox );
}

bool MonavConfigWidgetPrivate::updateStates( const QString &continent, QComboBox* comboBox )
{
    QSet<QString> states;
    foreach( const MonavStuffEntry &map, m_remoteMaps ) {
        Q_ASSERT( map.isValid() );
        if ( map.continent() == continent ) {
            states << map.state();
        }
    }

    return fillComboBox( states.toList(), comboBox );
}

bool MonavConfigWidgetPrivate::updateRegions( const QString &continent, const QString &state, QComboBox* comboBox )
{
    comboBox->clear();
    QMap<QString,QString> regions;
    foreach( const MonavStuffEntry &map, m_remoteMaps ) {
        Q_ASSERT( map.isValid() );
        if ( map.continent() == continent && map.state() == state ) {
            QString item = "%1 - %2";
            if ( map.region().isEmpty() ) {
                item = item.arg( map.state() );
                comboBox->addItem( item.arg( map.transport() ), map.payload() );
            } else {
                item = item.arg( map.region(), map.transport() );
                regions.insert( item, map.payload() );
            }
        }
    }

    QMapIterator<QString, QString> iter( regions );
    while ( iter.hasNext() ) {
        iter.next();
        comboBox->addItem( iter.key(), iter.value() );
    }

    return true;
}

MonavStuffEntry MonavConfigWidgetPrivate::map( const QString &continent, const QString &state, const QString &region ) const
{
    foreach( const MonavStuffEntry &entry, m_remoteMaps ) {
        if ( continent == entry.continent() && state == entry.state() && region == entry.region() ) {
            return entry;
        }
    }

    return MonavStuffEntry();
}

MonavConfigWidget::MonavConfigWidget( MonavPlugin* plugin ) :
        d( new MonavConfigWidgetPrivate( this, plugin ) )
{
    setupUi( this );
    d->setBusy( false );
    m_installedMapsListView->setModel( d->m_mapsModel );
    m_configureMapsListView->setModel( d->m_filteredModel );
    m_configureMapsListView->resizeColumnsToContents();

    updateComboBoxes();

    connect( m_continentComboBox, SIGNAL( currentIndexChanged( int ) ),
             this, SLOT( updateStates() ) );
    connect( m_transportTypeComboBox, SIGNAL( currentIndexChanged( QString ) ),
             this, SLOT( updateTransportTypeFilter( QString ) ) );
    connect( m_stateComboBox, SIGNAL( currentIndexChanged( int ) ),
             this, SLOT( updateRegions() ) );
    connect( m_installButton, SIGNAL( clicked() ), this, SLOT( downloadMap() ) );
    connect( m_cancelButton, SIGNAL( clicked() ), this, SLOT( cancelOperation() ) );
    connect( &d->m_removeMapSignalMapper, SIGNAL( mapped( int ) ),
             this, SLOT( removeMap( int ) ) );
    connect( &d->m_upgradeMapSignalMapper, SIGNAL( mapped( int ) ),
             this, SLOT( upgradeMap( int ) ) );
}

MonavConfigWidget::~MonavConfigWidget()
{
    delete d;
}

void MonavConfigWidget::loadSettings( const QHash<QString, QVariant> &settings )
{
    d->m_transport = settings["transport"].toString();
    d->updateTransportPreference();
}

void MonavConfigWidgetPrivate::updateTransportPreference()
{
    if ( m_parent->m_transportTypeComboBox && m_mapsModel ) {
        m_parent->m_transportTypeComboBox->blockSignals( true );
        m_parent->m_transportTypeComboBox->clear();
        QSet<QString> transportTypes;
        for( int i=0; i<m_mapsModel->rowCount(); ++i ) {
            QModelIndex index = m_mapsModel->index( i, 1 );
            transportTypes << m_mapsModel->data( index ).toString();
        }
        m_parent->m_transportTypeComboBox->addItems( transportTypes.toList() );
        m_parent->m_transportTypeComboBox->blockSignals( false );

        if ( !m_transport.isEmpty() && m_parent->m_transportTypeComboBox ) {
            for ( int i=1; i<m_parent->m_transportTypeComboBox->count(); ++i ) {
                if ( m_parent->m_transportTypeComboBox->itemText( i ) == m_transport ) {
                    m_parent->m_transportTypeComboBox->setCurrentIndex( i );
                    return;
                }
            }
        }
    }
}

QHash<QString, QVariant> MonavConfigWidget::settings() const
{
    QHash<QString, QVariant> settings;
    settings["transport"] = d->m_transport;
    return settings;
}

void MonavConfigWidget::retrieveMapList( QNetworkReply *reply )
{
    if ( reply->isReadable() && d->m_currentDownload.isEmpty() ) {
        disconnect( d->m_networkAccessManager, SIGNAL( finished( QNetworkReply * ) ),
                     this, SLOT( retrieveMapList( QNetworkReply * ) ) );
        d->parseNewStuff( reply->readAll() );
        updateComboBoxes();
    }
}

void MonavConfigWidget::retrieveData()
{
    if ( d->m_currentReply && d->m_currentReply->isReadable() && !d->m_currentDownload.isEmpty() ) {
        d->m_currentFile.write( d->m_currentReply->readAll() );
        if ( d->m_currentReply->isFinished() ) {
            d->m_currentReply->deleteLater();
            d->m_currentReply = 0;
            d->m_currentFile.close();
            d->installMap();
            d->m_currentDownload.clear();
        }
    }
}

void MonavConfigWidget::updateComboBoxes()
{
    d->updateContinents( m_continentComboBox );
    updateStates();
    updateRegions();
}

void MonavConfigWidget::updateStates()
{
    bool const haveContinents = m_continentComboBox->currentIndex() >= 0;
    if ( haveContinents ) {
        QString const continent = m_continentComboBox->currentText();
        if ( d->updateStates( continent, m_stateComboBox ) ) {
            updateRegions();
        }
    }
}

void MonavConfigWidget::updateRegions()
{
    bool haveRegions = false;
    if ( m_continentComboBox->currentIndex() >= 0 ) {
        if ( m_stateComboBox->currentIndex() >= 0 ) {
            QString const continent = m_continentComboBox->currentText();
            QString const state = m_stateComboBox->currentText();
            haveRegions = d->updateRegions( continent, state, m_regionComboBox );
        }
    }

    m_regionLabel->setVisible( haveRegions );
    m_regionComboBox->setVisible( haveRegions );
}

void MonavConfigWidget::downloadMap()
{
    if ( d->m_currentDownload.isEmpty() && !d->m_currentFile.isOpen() ) {
        d->m_currentDownload = m_regionComboBox->itemData( m_regionComboBox->currentIndex() ).toString();
        d->install();
    }
}

void MonavConfigWidget::cancelOperation()
{
    if ( !d->m_currentDownload.isEmpty() || d->m_currentFile.isOpen() ) {
        d->m_currentReply->abort();
        d->m_currentReply->deleteLater();
        d->m_currentReply = 0;
        d->m_currentDownload.clear();
        d->setBusy( false );
        d->m_currentFile.close();
    }
}

void MonavConfigWidgetPrivate::install()
{
    if ( !m_currentDownload.isEmpty() ) {
        int const index = m_currentDownload.lastIndexOf( "/");
        QString localFile = MarbleDirs::localPath() + "/maps" + m_currentDownload.mid( index );
        m_currentFile.setFileName( localFile );
        if ( m_currentFile.open( QFile::WriteOnly ) ) {
            QFileInfo file( m_currentFile );
            QString message = QObject::tr( "Downloading %1" ).arg( file.fileName() );
            setBusy( true, message );
            m_currentReply = m_networkAccessManager->get( QNetworkRequest( m_currentDownload ) );
            QObject::connect( m_currentReply, SIGNAL( readyRead() ),
                         m_parent, SLOT( retrieveData() ) );
            QObject::connect( m_currentReply, SIGNAL( readChannelFinished() ),
                         m_parent, SLOT( retrieveData() ) );
            QObject::connect( m_currentReply, SIGNAL( downloadProgress( qint64, qint64 ) ),
                     m_parent, SLOT( updateProgressBar( qint64, qint64 ) ) );
        } else {
            mDebug() << "Failed to write to " << localFile;
        }
    }
}

void MonavConfigWidgetPrivate::installMap()
{
    if ( m_unpackProcess ) {
        m_unpackProcess->close();
        delete m_unpackProcess;
        m_unpackProcess = 0;
        m_parent->m_installButton->setEnabled( true );
    } else if ( m_currentFile.fileName().endsWith( "tar.gz" ) && canExecute( "tar" ) ) {
        QFileInfo file( m_currentFile );
        QString message = QObject::tr( "Installing %1" ).arg( file.fileName() );
        setBusy( true, message );
        m_parent->m_progressBar->setMaximum( 0 );
        if ( file.exists() && file.isReadable() ) {
            m_unpackProcess = new QProcess;
            QObject::connect( m_unpackProcess, SIGNAL( finished( int ) ),
                     m_parent, SLOT( mapInstalled( int ) ) );
            QStringList arguments = QStringList() << "-x" << "-z" << "-f" << file.fileName();
            m_unpackProcess->setWorkingDirectory( file.dir().absolutePath() );
            m_unpackProcess->start( "tar", arguments );
        }
    } else {
        if ( !m_currentFile.fileName().endsWith( "tar.gz" ) ) {
            mDebug() << "Can only handle tar.gz files";
        } else {
            mDebug() << "Cannot extract archive: tar executable not found in PATH.";
        }
    }
}

void MonavConfigWidget::updateProgressBar( qint64 bytesReceived, qint64 bytesTotal )
{
    // Coarse MB resolution for the text to get it short,
    // finer Kb resolution for the progress values to see changes easily
    m_progressBar->setMaximum( bytesTotal / 1024 );
    m_progressBar->setValue( bytesReceived / 1024 );
    QString progress = "%1/%2 MB";
    m_progressBar->setFormat( progress.arg( bytesReceived / 1024 / 1024 ).arg( bytesTotal / 1024 / 1024 ) );
}

bool MonavConfigWidgetPrivate::canExecute( const QString &executable ) const
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

void MonavConfigWidget::mapInstalled( int exitStatus )
{
    d->m_unpackProcess = 0;
    d->m_currentFile.remove();
    d->setBusy( false );

    if ( exitStatus == 0 ) {
        d->m_plugin->reloadMaps();
        d->updateInstalledMapsView();
        monavTabWidget->setCurrentIndex( 0 );
    } else {
        mDebug() << "Error when unpacking archive, process exited with status code " << exitStatus;
    }
}

void MonavConfigWidget::showEvent ( QShowEvent * event )
{
    // Lazy initialization
    RunnerPlugin::ConfigWidget::showEvent( event );
    if ( !event->spontaneous() && !d->m_initialized ) {
        d->m_initialized = true;
        d->updateInstalledMapsView();
        d->m_networkAccessManager = new QNetworkAccessManager( this );
        connect( d->m_networkAccessManager, SIGNAL( finished( QNetworkReply * ) ),
                     this, SLOT( retrieveMapList( QNetworkReply * ) ) );
        QUrl url = QUrl( "http://files.kde.org/marble/newstuff/maps-monav.xml" );
        d->m_networkAccessManager->get( QNetworkRequest( url ) );
    }
}

void MonavConfigWidgetPrivate::updateInstalledMapsView()
{
    m_mapsModel = m_plugin->installedMapsModel();
    m_mapsModel->setInstallableVersions( m_remoteVersions );
    m_filteredModel->setSourceModel( m_mapsModel );
    m_parent->m_installedMapsListView->setModel( m_mapsModel );

    m_parent->m_configureMapsListView->setColumnHidden( 1, true );
    m_parent->m_installedMapsListView->setColumnHidden( 2, true );
    m_parent->m_configureMapsListView->setColumnHidden( 3, true );
    m_parent->m_configureMapsListView->setColumnHidden( 4, true );
    m_parent->m_installedMapsListView->setColumnHidden( 5, true );

    m_parent->m_configureMapsListView->horizontalHeader()->setVisible( true );
    m_parent->m_installedMapsListView->horizontalHeader()->setVisible( true );
    m_parent->m_configureMapsListView->resizeColumnsToContents();
    m_parent->m_installedMapsListView->resizeColumnsToContents();

    updateTransportPreference();
    updateInstalledMapsViewButtons();
}

void MonavConfigWidgetPrivate::updateInstalledMapsViewButtons()
{
    m_removeMapSignalMapper.removeMappings( m_parent );
    m_upgradeMapSignalMapper.removeMappings( m_parent );
    for( int i=0; i<m_mapsModel->rowCount(); ++i ) {
        {
            QPushButton* button = new QPushButton( QIcon( ":/system-software-update.png" ), "" );
            button->setAutoFillBackground( true );
            QModelIndex index = m_mapsModel->index( i, 3 );
            m_parent->m_installedMapsListView->setIndexWidget( index, button );
            m_upgradeMapSignalMapper.setMapping( button, index.row() );
            QObject::connect( button, SIGNAL( clicked() ), &m_upgradeMapSignalMapper, SLOT( map() ) );
            bool upgradable = m_mapsModel->data( index ).toBool();
            QString canUpgradeText = QObject::tr( "An update is available. Click to install it." );
            QString isLatestText = QObject::tr( "No update available. You are running the latest version." );
            button->setToolTip( upgradable ? canUpgradeText : isLatestText );
            button->setEnabled( upgradable );
        }
        {
            QPushButton* button = new QPushButton( QIcon( ":/edit-delete.png" ), "" );
            button->setAutoFillBackground( true );
            QModelIndex index = m_mapsModel->index( i, 4 );
            m_parent->m_installedMapsListView->setIndexWidget( index, button );
            m_removeMapSignalMapper.setMapping( button, index.row() );
            QObject::connect( button, SIGNAL( clicked() ), &m_removeMapSignalMapper, SLOT( map() ) );
            bool const writable = m_mapsModel->data( index ).toBool();
            button->setEnabled( writable );
        }
    }
    m_parent->m_installedMapsListView->resizeColumnsToContents();
}

void MonavConfigWidget::updateTransportTypeFilter( const QString &filter )
{
    d->m_filteredModel->setFilterFixedString( filter );
    d->m_transport = filter;
    m_configureMapsListView->resizeColumnsToContents();
}

void MonavConfigWidget::removeMap( int index )
{
    QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
    QString text = tr( "Are you sure you want to delete this map from the system?" );
    if ( QMessageBox::question( this, tr( "Remove Map" ), text, buttons, QMessageBox::No ) == QMessageBox::Yes ) {
        d->m_mapsModel->deleteMapFiles( index );
        d->m_plugin->reloadMaps();
        d->updateInstalledMapsView();
    }
}

void MonavConfigWidget::upgradeMap( int index )
{
    QString payload = d->m_mapsModel->payload( index );
    if ( !payload.isEmpty() ) {
        foreach( const MonavStuffEntry &entry, d->m_remoteMaps ) {
            if ( entry.payload().endsWith( payload ) ) {
                d->m_currentDownload = entry.payload();
                d->install();
                return;
            }
        }
    }
}

void MonavConfigWidgetPrivate::setBusy( bool busy, const QString &message ) const
{
    if ( busy ) {
        m_parent->m_stackedWidget->removeWidget( m_parent->m_settingsPage );
        m_parent->m_stackedWidget->addWidget( m_parent->m_progressPage );
    } else {
        m_parent->m_stackedWidget->removeWidget( m_parent->m_progressPage );
        m_parent->m_stackedWidget->addWidget( m_parent->m_settingsPage );
    }

    QString const defaultMessage = QObject::tr( "Nothing to do." );
    m_parent->m_progressLabel->setText( message.isEmpty() ? defaultMessage : message );
}

}

#include "MonavConfigWidget.moc"
