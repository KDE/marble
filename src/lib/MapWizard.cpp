//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "MapWizard.h"
#include "ui_MapWizard.h"

#include "MarbleGlobal.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "ServerLayout.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneIcon.h"
#include "GeoSceneZoom.h"
#include "GeoSceneMap.h"
#include "GeoSceneLayer.h"
#include "GeoSceneTiled.h"
#include "GeoSceneSettings.h"
#include "GeoSceneProperty.h"
#include "GeoSceneGeodata.h"
#include "GeoSceneLegend.h"
#include "GeoSceneSection.h"
#include "GeoSceneItem.h"
#include "GeoSceneVector.h"
#include "GeoWriter.h"
#include "DgmlElementDictionary.h"
#include "MarbleWidget.h"
#include "MarbleNavigator.h"

#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QSharedPointer>
#include <QtCore/QTimer>
#include <QtCore/QTemporaryFile>
#include <QtGui/QPixmap>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QImageReader>
#include <QtGui/QDialogButtonBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtXml/QDomElement>
#include <QtXml/QXmlStreamWriter>

namespace Marble
{

class MapWizardPrivate
{
public:
    MapWizardPrivate()
        : m_serverCapabilitiesValid( false )
    {}

    void pageEntered( int id );

    Ui::MapWizard uiWidget;

    QString mapTheme;

    QNetworkAccessManager xmlAccessManager;
    QNetworkAccessManager legendAccessManager;
    QNetworkAccessManager levelZeroAccessManager;
    QStringList wmsServerList;
    QMap<QString, QString> wmsFetchedMaps;
    QStringList staticUrlServerList;
    bool m_serverCapabilitiesValid;

    enum mapType
    {
        NoMap,
        StaticImageMap,
        WmsMap,
        StaticUrlMap
    };

    mapType mapProviderType;
    QByteArray levelZero;
    QImage previewImage;

    QString format;

    QString wmsProjection;
    QStringList wmsLegends;

    QString sourceImage;

    QString dgmlOutput;
    QString legendHtml;
};

class PreviewDialog : public QDialog
{
public:
    PreviewDialog( QWidget* parent, QString mapThemeId );
    virtual void closeEvent(QCloseEvent* e );
private:
    bool deleteTheme( QString directory );
    QString m_mapThemeId;
};

PreviewDialog::PreviewDialog( QWidget* parent, QString mapThemeId ) : QDialog( parent ), m_mapThemeId( mapThemeId )
{
    QGridLayout *layout = new QGridLayout();
    MarbleWidget *widget = new MarbleWidget();
    MarbleNavigator *navigator = new MarbleNavigator();
    
    connect( navigator, SIGNAL( goHome() ), widget, SLOT( goHome() ) );
    connect( navigator, SIGNAL( moveUp() ), widget, SLOT( moveUp() ) );
    connect( navigator, SIGNAL( moveDown() ), widget, SLOT( moveDown() ) );
    connect( navigator, SIGNAL( moveLeft() ), widget, SLOT( moveLeft() ) );
    connect( navigator, SIGNAL( moveRight() ), widget, SLOT( moveRight() ) );
    connect( navigator, SIGNAL( zoomIn() ), widget, SLOT( zoomIn() ) );
    connect( navigator, SIGNAL( zoomOut() ), widget, SLOT( zoomOut() ) );
    connect( navigator, SIGNAL( zoomChanged(int) ), widget, SLOT( zoomView(int) ) );
    
    widget->setMapThemeId( m_mapThemeId );
    widget->zoomView( 1000 );
    
    layout->addWidget( navigator, 1, 1 );
    layout->addWidget( widget, 1, 2 );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    
    this->setLayout( layout );
    this->setMinimumSize( 640, 480 );
    this->setWindowTitle( tr( "Preview Map" ) );
}

void PreviewDialog::closeEvent(QCloseEvent* e)
{
    QString dgmlPath = MarbleDirs::localPath() + "/maps/" + m_mapThemeId;
    QString directory = dgmlPath.left( dgmlPath.lastIndexOf("/") );
    this->deleteTheme( directory );
    QDialog::closeEvent( e );
}

bool PreviewDialog::deleteTheme( QString directory )
{
    QDir dir(directory);
    bool result = true;
 
    if (dir.exists(directory)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = deleteTheme(info.absoluteFilePath());
            } else {
                result = QFile::remove(info.absoluteFilePath());
            }
 
            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(directory);
    }
    
    return result;
}

void MapWizardPrivate::pageEntered( int id )
{
    if ( id == 1 ) {
        m_serverCapabilitiesValid = false;
    } else if ( id == 2 || id == 4 ) {
        levelZero.clear();
        uiWidget.comboBoxStaticUrlServer->clear();
        uiWidget.comboBoxStaticUrlServer->addItems( staticUrlServerList );
        uiWidget.comboBoxStaticUrlServer->addItem( "http://" );
    } else if ( id == 5 ) {
        if ( mapProviderType == MapWizardPrivate::StaticImageMap ) {
            previewImage = QImage( uiWidget.lineEditSource->text() ).scaled( 136, 136, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
        } else {
            previewImage = QImage::fromData( levelZero ).scaled( 136, 136, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
        }
        uiWidget.labelPreview->setPixmap( QPixmap::fromImage( previewImage ) );
    } else if ( id == 7 ) {
        uiWidget.labelThumbnail->setPixmap( QPixmap::fromImage( previewImage ) );
    }
}

MapWizard::MapWizard( QWidget* parent ) : QWizard( parent ), d( new MapWizardPrivate )
{
    d->uiWidget.setupUi( this );
    
    connect( this, SIGNAL( currentIdChanged( int ) ), this, SLOT( pageEntered( int ) ) );

    connect( &( d->xmlAccessManager ), SIGNAL( finished( QNetworkReply* ) ), this, SLOT( parseServerCapabilities( QNetworkReply* ) ) );
    connect( &( d->legendAccessManager ), SIGNAL( finished( QNetworkReply* ) ), this, SLOT( createWmsLegend( QNetworkReply* ) ) );
    connect( &( d->levelZeroAccessManager ), SIGNAL( finished( QNetworkReply* ) ), this, SLOT( createLevelZero( QNetworkReply* ) ) );

    connect( d->uiWidget.pushButtonSource, SIGNAL( clicked( bool ) ), this, SLOT( querySourceImage() ) );
    connect( d->uiWidget.pushButtonPreview, SIGNAL( clicked( bool ) ), this, SLOT( queryPreviewImage() ) );
    connect( d->uiWidget.pushButtonLegend_2, SIGNAL( clicked( bool ) ), this, SLOT( queryLegendImage() ) );

    connect( d->uiWidget.comboBoxWmsServer, SIGNAL( currentIndexChanged( QString ) ), d->uiWidget.lineEditWmsUrl, SLOT( setText( QString ) ) );
    connect( d->uiWidget.listWidgetWmsMaps, SIGNAL( itemSelectionChanged() ), this, SLOT( autoFillDetails() ) );
    
    connect( d->uiWidget.lineEditTitle, SIGNAL( textChanged( QString ) ), d->uiWidget.labelSumMName, SLOT( setText( QString ) ) );
    connect( d->uiWidget.lineEditTheme, SIGNAL( textChanged( QString ) ), d->uiWidget.labelSumMTheme, SLOT( setText( QString ) ) );
    
    connect( d->uiWidget.pushButtonPreviewMap, SIGNAL( clicked( bool ) ), this, SLOT( showPreview() ) );
    
}

MapWizard::~MapWizard()
{
    delete d;
}

void MapWizard::queryServerCapabilities()
{
    QUrl url( d->uiWidget.lineEditWmsUrl->text() );
    url.addQueryItem( "service", "WMS" );
    url.addQueryItem( "request", "GetCapabilities" );

    QNetworkRequest request;
    request.setUrl( url );

    d->xmlAccessManager.get( request );
}

void MapWizard::parseServerCapabilities( QNetworkReply* reply )
{
    button( MapWizard::NextButton )->setEnabled( true );

    QString result( reply->readAll() );
    QDomDocument xml;
    if( !xml.setContent( result ) )
    {
        QMessageBox::critical( this, tr( "Error while parsing" ), tr( "Wizard cannot parse server's response" ) );
        return;
    }

    if( xml.documentElement().firstChildElement().tagName().isNull() )
    {
        QMessageBox::critical( this, tr( "Error while parsing" ), tr( "Server is not a Web Map Server." ) );
        return;
    }

    QDomElement firstLayer = xml.documentElement().firstChildElement( "Capability" ).firstChildElement( "Layer" );
    QDomNodeList layers = firstLayer.elementsByTagName( "Layer" );

    d->wmsProjection = firstLayer.firstChildElement( "SRS" ).text();
    d->uiWidget.listWidgetWmsMaps->clear();

    for( int i = 0; i < layers.size(); ++i )
    {
        QString theme = layers.at( i ).firstChildElement( "Name" ).text();
        QString title = layers.at( i ).firstChildElement( "Title" ).text();
        QDomElement legendUrl = layers.at( i ).firstChildElement( "Style" ).firstChildElement( "LegendURL" );
        d->wmsFetchedMaps[ theme ] = title;

        d->wmsLegends.clear();
        if( legendUrl.isNull() ) {
            d->wmsLegends.append( QString() );
        } else {
            d->wmsLegends.append( legendUrl.firstChildElement( "OnlineResource" ).attribute( "xlink:href" ) );
        }
    }
    
    d->uiWidget.listWidgetWmsMaps->addItems( d->wmsFetchedMaps.values() );

    QDomElement format = xml.documentElement().firstChildElement( "Capability" ).firstChildElement( "Request" )
                         .firstChildElement( "GetMap" ).firstChildElement( "Format" );

    d->format = format.text().right( format.text().length() - format.text().indexOf( '/' ) - 1 ).toLower();

    if( d->format == "jpeg" ) {
        d->format = "jpg";
    }

    if( !d->wmsFetchedMaps.isEmpty() && !d->wmsServerList.contains( d->uiWidget.lineEditWmsUrl->text() ) ) {
        d->wmsServerList.append( d->uiWidget.lineEditWmsUrl->text() );
        setWmsServers( d->wmsServerList );
    }

    d->m_serverCapabilitiesValid = true;
    next();
}

void MapWizard::createWmsLegend( QNetworkReply* reply )
{
    QByteArray result( reply->readAll() );
    QDir map( QString( "%1/maps/earth/%2" ).arg( MarbleDirs::localPath() ).arg( d->mapTheme ) );
    if( !map.exists( "legend" ) ) {
        map.mkdir( "legend" );
    }

    QFile image( QString( "%1/legend/legend.png" ).arg( map.absolutePath() ) );
    image.open( QIODevice::ReadWrite );
    image.write( result );
    image.close();

    createLegendHtml();
    createLegendFile();
}

void MapWizard::setWmsServers( const QStringList& uris )
{
    d->wmsServerList = uris;

    d->uiWidget.comboBoxWmsServer->clear();
    d->uiWidget.comboBoxWmsServer->addItems( d->wmsServerList );
    d->uiWidget.comboBoxWmsServer->addItem( tr( "Custom" ), "http://" );
}

QStringList MapWizard::wmsServers() const
{
    return d->wmsServerList;
}

QStringList MapWizard::staticUrlServers() const
{
    return d->staticUrlServerList;
}

void MapWizard::setStaticUrlServers( const QStringList& uris )
{
    d->staticUrlServerList = uris;
}

void MapWizard::autoFillDetails()
{
    QString selected = d->uiWidget.listWidgetWmsMaps->currentItem()->text();
    d->uiWidget.lineEditTitle->setText( selected );
    d->uiWidget.lineEditTheme->setText( d->wmsFetchedMaps.key( selected ) );
}

bool MapWizard::createFiles( const GeoSceneDocument* document )
{
    // Create directories
    QDir maps( MarbleDirs::localPath() + "/maps/earth/" );
    if( !maps.exists( document->head()->theme() ) )
    {
        maps.mkdir( document->head()->theme() );

        if( d->mapProviderType == MapWizardPrivate::StaticImageMap )
        {
            // Source image
            QFile sourceImage( d->sourceImage );
            d->format = d->sourceImage.right( d->sourceImage.length() - d->sourceImage.lastIndexOf( '.' ) - 1 ).toLower();
            sourceImage.copy( QString( "%1/%2/%2.%3" ).arg( maps.absolutePath() )
                                                      .arg( document->head()->theme() )
                                                      .arg( d->format ) );
        }

        else if( d->mapProviderType == MapWizardPrivate::WmsMap )
        {
            maps.mkdir( QString( "%1/0/" ).arg( document->head()->theme() ) );
            maps.mkdir( QString( "%1/0/0" ).arg( document->head()->theme() ) );
            const QString path = QString( "%1/%2/0/0/0.%3" ).arg( maps.absolutePath() )
                                                            .arg( document->head()->theme() )
                                                            .arg( d->format );
            QFile baseTile( path );
            baseTile.open( QFile::WriteOnly );
            baseTile.write( d->levelZero );
        }

        else if( d->mapProviderType == MapWizardPrivate::StaticUrlMap )
        {
            maps.mkdir( QString( "%1/0/" ).arg( document->head()->theme() ) );
            maps.mkdir( QString( "%1/0/0" ).arg( document->head()->theme() ) );
            const QString path = QString( "%1/%2/0/0/0.%3" ).arg( maps.absolutePath() )
                                                            .arg( document->head()->theme() )
                                                            .arg( d->format );
            QFile baseTile( path );
            baseTile.open( QFile::WriteOnly );
            baseTile.write( d->levelZero );
        }

        // Preview image
        QString pixmapPath = QString( "%1/%2/%3" ).arg( maps.absolutePath() )
                                                  .arg( document->head()->theme() )
                                                  .arg( document->head()->icon()->pixmap() );
        d->previewImage.save( pixmapPath );

        // DGML
        QFile file( QString( "%1/%2/%2.dgml" ).arg( maps.absolutePath() )
                                              .arg( document->head()->theme() ) );
        file.open( QIODevice::ReadWrite );
        GeoWriter geoWriter;
        geoWriter.setDocumentType( dgml::dgmlTag_nameSpace20 );
        geoWriter.write( &file, document );
        file.close();

        return true;
    }

    else
        return false;
}

void MapWizard::createLegendHtml( QString image )
{
    QString htmlOutput;
    QXmlStreamWriter stream( &htmlOutput );
    stream.writeStartDocument();
    stream.writeStartElement( "html" );
    stream.writeStartElement( "head" );

    stream.writeTextElement( "title", "Marble: Legend" );
    stream.writeStartElement( "link" );
    stream.writeAttribute( "href", "legend.css" );
    stream.writeAttribute( "rel", "stylesheet" );
    stream.writeAttribute( "type", "text/css" );
    stream.writeEndElement();

    stream.writeStartElement( "body" );
    stream.writeStartElement( "img" );
    stream.writeAttribute( "src", image );
    stream.writeEndElement();
    stream.writeComment( " ##customLegendEntries:all## " );
    stream.writeEndElement();
    stream.writeEndElement();
    d->legendHtml = htmlOutput;
}

void MapWizard::createLegendFile()
{
    QDir map( QString( "%1/maps/earth/%2" ).arg( MarbleDirs::localPath() ).arg( d->mapTheme ) );
    
    QFile html( QString( "%1/legend.html" ).arg( map.absolutePath() ) );
    html.open( QIODevice::ReadWrite );
    html.write( d->legendHtml.toAscii().data() );
    html.close();
}

void MapWizard::downloadLegend( const QString& url )
{
    QUrl downloadUrl( url );
    QNetworkRequest request( downloadUrl );
    d->legendAccessManager.get( request );
}

void MapWizard::downloadLevelZero()
{
    if( d->mapProviderType == MapWizardPrivate::WmsMap )
    {
        QString selected = d->uiWidget.listWidgetWmsMaps->currentItem()->text();
        
        QUrl downloadUrl( d->uiWidget.lineEditWmsUrl->text() );
        downloadUrl.addQueryItem( "request", "GetMap" );
        downloadUrl.addQueryItem( "version", "1.1.1" );
        downloadUrl.addQueryItem( "layers", d->wmsFetchedMaps.key( selected ) );
        downloadUrl.addQueryItem( "srs", "EPSG:4326" );
        downloadUrl.addQueryItem( "width", "400" );
        downloadUrl.addQueryItem( "height", "200" );
        downloadUrl.addQueryItem( "bbox", "-180,-90,180,90" );
        downloadUrl.addQueryItem( "format", "image/jpeg" );
        downloadUrl.addQueryItem( "styles", "" );
        
        QNetworkRequest request( downloadUrl );
        d->levelZeroAccessManager.get( request );
    }
    
    else if( d->mapProviderType == MapWizardPrivate::StaticUrlMap )
    {
        QString server = d->uiWidget.comboBoxStaticUrlServer->currentText();
        QUrl downloadUrl;

        server.replace( server.indexOf( "{x}" ), 3,  QString::number( 0 ) );
        server.replace( server.indexOf( "{y}" ), 3,  QString::number( 0 ) );
        server.replace( server.indexOf( "{zoomLevel}" ), 11,  QString::number( 0 ) );
        downloadUrl.setUrl( server );

        QNetworkRequest request( downloadUrl );
        d->levelZeroAccessManager.get( request );
    }
}

void MapWizard::createLevelZero( QNetworkReply* reply )
{
    button( MapWizard::NextButton )->setEnabled( true );

    d->levelZero = reply->readAll();
    QImage testImage = QImage::fromData( d->levelZero );

    if ( d->levelZero.isNull() ) {
        QMessageBox::information( this,
                                    tr( "Base Tile" ),
                                    tr( "The base tile could not be downloaded." ) );
        return;
    }

    if ( testImage.isNull() ) {
        QMessageBox::information( this,
                                    tr( "Base Tile" ),
                                    tr( "The base tile could not be downloaded successfully. The server replied:\n\n%1" ).arg( QString( d->levelZero ) ) );
        d->levelZero.clear();
        return;
    }

    QBuffer testBuffer( &d->levelZero );
    d->format = QImageReader( &testBuffer ).format();

    if ( d->mapProviderType == MapWizardPrivate::StaticUrlMap ) {
        const QString url = d->uiWidget.comboBoxStaticUrlServer->currentText();
        d->staticUrlServerList.removeAll( url );
        d->staticUrlServerList.prepend( url );
    }

    next();
}

void MapWizard::createLegend()
{
    QDir map( QString( "%1/maps/earth/%2" ).arg( MarbleDirs::localPath() ).arg( d->mapTheme ) );
    if( !map.exists( "legend" ) ) {
        map.mkdir( "legend" );
    }

    QFile image;
    image.setFileName( d->uiWidget.lineEditLegend_2->text() );
    image.copy( QString( "%1/legend/legend.png" ).arg( map.absolutePath() ) );

    createLegendHtml();
    createLegendFile();
}

void MapWizard::querySourceImage()
{
    d->uiWidget.lineEditSource->setText( QFileDialog::getOpenFileName() );
}

void MapWizard::queryPreviewImage()
{
    QString fileName = QFileDialog::getOpenFileName();
    d->previewImage = QImage( fileName );

    QPixmap preview = QPixmap::fromImage( d->previewImage );
    d->uiWidget.labelThumbnail->setPixmap( preview );
    d->uiWidget.labelThumbnail->resize( preview.width(), preview.height() );
}

void MapWizard::queryLegendImage()
{
    QString fileName = QFileDialog::getOpenFileName();
    d->uiWidget.lineEditLegend_2->setText( fileName );
    createLegendHtml( d->uiWidget.lineEditLegend_2->text() );
    d->uiWidget.textBrowserLegend->setHtml( d->legendHtml );
}

QString MapWizard::createArchive( QWidget *parent, QString mapId )
{
    QStringList splitMapId( mapId.split(QLatin1Char('/')) );
    QString body = splitMapId[0];
    QString theme = splitMapId[1];
    QDir themeDir;

    QStringList tarArgs;
    tarArgs.append( "--create" );
    tarArgs.append( "--gzip" );
    tarArgs.append( "--file" );
    tarArgs.append( QString( "%1/%2.tar.gz" ).arg( QDir::tempPath() ).arg( theme ) );
    tarArgs.append( "--directory" );

    if( QFile::exists( QString( "%1/maps/%2" ).arg( MarbleDirs::localPath() ).arg( mapId ) ) )
    {
        tarArgs.append( QString( "%1/maps/" ).arg( MarbleDirs::localPath() ) );
        themeDir.cd( QString( "%1/maps/%2/%3" ).arg( MarbleDirs::localPath() ).arg( body ).arg( theme ) );
    }
    
    else if( QFile::exists( QString( "%1/maps/%2" ).arg( MarbleDirs::systemPath() ).arg( mapId ) ) )
    {
        tarArgs.append( QString( "%1/maps/" ).arg( MarbleDirs::systemPath() ) );
        themeDir.cd( QString( "%1/maps/%2/%3" ).arg( MarbleDirs::systemPath() ).arg( body ).arg( theme ) );
    }
    
    if( QFile::exists( QString( "%1/%2.dgml" ).arg( themeDir.absolutePath() ).arg( theme ) ) )
    {
        tarArgs.append( QString( "%1/%2/%2.dgml" ).arg( body ).arg( theme ) );
    }
    
    if( QFile::exists( QString( "%1/legend.html" ).arg( themeDir.absolutePath() ) ) )
    {
        tarArgs.append( QString( "%1/%2/legend.html" ).arg( body ).arg( theme ) );
    }
    
    if( QFile::exists( QString( "%1/legend" ).arg( themeDir.absolutePath() ) ) )
    {
        tarArgs.append( QString( "%1/%2/legend" ).arg( body ).arg( theme ) );
    }
    
    if( QFile::exists( QString( "%1/0/000000" ).arg( themeDir.absolutePath() ) ) )
    {
        tarArgs.append( QString( "%1/%2/0/000000" ).arg( body ).arg( theme ) );
    }
    
    QStringList previewFilters;
    previewFilters << "preview.*";
    QStringList preview = themeDir.entryList( previewFilters );
    if( !preview.isEmpty() ) {
        tarArgs.append( QString( "%1/%2/%3" ).arg( body ).arg( theme ).arg( preview[0] ) );
    }
    
    QStringList sourceImgFilters;
    sourceImgFilters << QString( "%1.jpg" ).arg( theme ) << QString( "%1.png" ).arg( theme ) << QString( "%1.jpeg" ).arg( theme );
    QStringList sourceImg = themeDir.entryList( sourceImgFilters );
    if( !sourceImg.isEmpty() ) {
        tarArgs.append( QString( "%1/%2/%3" ).arg( body ).arg( theme ).arg( sourceImg[0] ) );
    }
    
    QProcess archiver;
    switch( archiver.execute( "tar", tarArgs ) )
    {
    case -2:
        QMessageBox::critical( parent, tr( "Archiving failed" ), tr( "Archiving process cannot be started." ) );
        break;
    case -1:
        QMessageBox::critical( parent, tr( "Archiving failed" ), tr( "Archiving process crashed." ) );
        break;
    case 0:
        mDebug() << "Archived the theme successfully.";
        break;
    }
    archiver.waitForFinished();
    return QString( "%1/%2.tar.gz" ).arg( QDir::tempPath() ).arg( theme ); 
}

void MapWizard::deleteArchive( QString mapId )
{
    QStringList splitMapId( mapId.split(QLatin1Char('/')) );
    QString theme = splitMapId[1];
    QFile::remove( QString( "%1/%2.tar.gz" ).arg( QDir::tempPath() ).arg( theme ) );
}

bool MapWizard::validateCurrentPage()
{
    if ( currentId() == 1 && !d->m_serverCapabilitiesValid ) {
        queryServerCapabilities();
        button( MapWizard::NextButton )->setEnabled( false );
        return false;
    }

    if ( ( currentId() == 2 || currentId() == 4 ) && d->levelZero.isNull() ) {
        downloadLevelZero();
        button( MapWizard::NextButton )->setEnabled( false );
        return false;
    }

    if ( currentId() == 3 ) {
        d->sourceImage = d->uiWidget.lineEditSource->text();
        if ( d->sourceImage.isEmpty() ) {
            QMessageBox::information( this,
                                      tr( "Source Image" ),
                                      tr( "Please specify a source image." ) );
            d->uiWidget.lineEditSource->setFocus();
            return false;
        }

        if ( !QFileInfo( d->sourceImage ).exists() ) {
            QMessageBox::information( this,
                                      tr( "Source Image" ),
                                      tr( "The source image you specified does not exist. Please specify a different one." ) );
            d->uiWidget.lineEditSource->setFocus();
            d->uiWidget.lineEditSource->selectAll();
            return false;
        }

        if ( QImage( d->sourceImage ).isNull() ) {
            QMessageBox::information( this,
                                      tr( "Source Image" ),
                                      tr( "The source image you specified does not seem to be an image. Please specify a different image file." ) );
            d->uiWidget.lineEditSource->setFocus();
            d->uiWidget.lineEditSource->selectAll();
            return false;
        }
    }

    if ( currentId() == 5 ) {
        if ( d->uiWidget.lineEditTitle->text().isEmpty() ) {
            QMessageBox::information( this, tr( "Map Title" ), tr( "Please specify a map title." ) );
            d->uiWidget.lineEditTitle->setFocus();
            return false;
        }

        d->mapTheme = d->uiWidget.lineEditTheme->text();
        if ( d->mapTheme.isEmpty() ) {
            QMessageBox::information( this, tr( "Map Name" ), tr( "Please specify a map name." ) );
            d->uiWidget.lineEditTheme->setFocus();
            return false;
        }

        const QDir destinationDir( QString( "%1/maps/earth/%2" ).arg( MarbleDirs::localPath() ).arg( d->mapTheme ) );
        if ( destinationDir.exists() ) {
            QMessageBox::information( this,
                                    tr( "Map Name" ),
                                    tr( "Please specify another map name, since there is already a map named \"%1\"." ).arg( d->mapTheme ) );
            d->uiWidget.lineEditTheme->setFocus();
            d->uiWidget.lineEditTheme->selectAll();
            return false;
        }

        if ( d->previewImage.isNull() ) {
            QMessageBox::information( this, tr( "Preview Image" ), tr( "Please specify a preview image." ) );
            d->uiWidget.pushButtonPreview->setFocus();
            return false;
        }
    }

    return QWizard::validateCurrentPage();
}

int MapWizard::nextId() const
{
    switch( currentId() )
    {
    case 0:
        if( d->uiWidget.radioButtonWms->isChecked() ) {
            d->mapProviderType = MapWizardPrivate::WmsMap;
            return 1;
        } else if( d->uiWidget.radioButtonBitmap->isChecked() ) {
            d->mapProviderType = MapWizardPrivate::StaticImageMap;
            return 3;
        } else if( d->uiWidget.radioButtonStaticUrl->isChecked() ) {
            d->mapProviderType = MapWizardPrivate::StaticUrlMap;
            return 4;
        }
        break;

    case 2: // WMS
        return 5;
        break;

    case 3: // Static Image
        return 5;
        break;

    case 7: // Finish
        return -1;
        break;

    default:
        break;

    }
    
    return currentId() + 1;
}

GeoSceneDocument* MapWizard::createDocument()
{
    GeoSceneDocument *document = new GeoSceneDocument;
       
    GeoSceneHead *head = document->head();
    head->setName( d->uiWidget.lineEditTitle->text() );
    head->setTheme( d->uiWidget.lineEditTheme->text() );
    head->setTarget( "earth" );
    head->setDescription( d->uiWidget.textEditDesc->document()->toHtml() );
    head->setVisible( true );
        
    GeoSceneIcon *icon = head->icon();
    icon->setPixmap( QString( "preview.png" ) );
    
    GeoSceneZoom *zoom = head->zoom();
    zoom->setMinimum( 900 );
    zoom->setMaximum( 3500 );
    zoom->setDiscrete( false );
    
    GeoSceneTiled *texture = new GeoSceneTiled( "map" );
    texture->setExpire( 31536000 );
    texture->setSourceDir( "earth/" + document->head()->theme() ); 
    if( d->mapProviderType == MapWizardPrivate::WmsMap )
    {
        texture->setFileFormat( d->format );
        QString layer = d->wmsFetchedMaps.key( d->uiWidget.listWidgetWmsMaps->currentItem()->text() );
        QUrl downloadUrl = QUrl( d->uiWidget.lineEditWmsUrl->text() );
        downloadUrl.addQueryItem( "layers", layer );
        texture->addDownloadUrl( downloadUrl );
        texture->setMaximumTileLevel( 20 );
        texture->setLevelZeroRows( 1 );
        texture->setLevelZeroColumns( 1 );
        texture->setServerLayout( new WmsServerLayout( texture ) );
        texture->setProjection( GeoSceneTiled::Equirectangular );
    }
    
    else if( d->mapProviderType == MapWizardPrivate::StaticUrlMap )
    {
        texture->setFileFormat( d->format );
        QUrl downloadUrl = QUrl( d->uiWidget.comboBoxStaticUrlServer->currentText() );
        texture->addDownloadPolicy( DownloadBrowse, 20 );
        texture->addDownloadPolicy( DownloadBulk, 2 );
        texture->addDownloadUrl( downloadUrl );
        texture->setMaximumTileLevel( 20 );
        texture->setLevelZeroRows( 1 );
        texture->setLevelZeroColumns( 1 );
        texture->setServerLayout( new CustomServerLayout( texture ) );
        texture->setProjection( GeoSceneTiled::Mercator );
    }
    
    else if( d->mapProviderType == MapWizardPrivate::StaticImageMap )
    {
        QString image = d->uiWidget.lineEditSource->text();
        d->format = image.right( image.length() - image.lastIndexOf( '.' ) - 1 ).toLower();
        texture->setFileFormat( d->format.toUpper() );
        texture->setInstallMap( document->head()->theme() + '.' + d->format );
        texture->setServerLayout( new MarbleServerLayout( texture ) );
        texture->setProjection( GeoSceneTiled::Equirectangular );
        int imageWidth = QImage( image ).width();
        int tileSize = c_defaultTileSize;
        
        float approxMaxTileLevel = log( imageWidth / ( 2.0 * tileSize ) ) / log( 2.0 );
        int  maxTileLevel = 0;
        if ( approxMaxTileLevel == int( approxMaxTileLevel ) ) {
            maxTileLevel = static_cast<int>( approxMaxTileLevel );
        } else {
            maxTileLevel = static_cast<int>( approxMaxTileLevel + 1 );
        }
        texture->setMaximumTileLevel( maxTileLevel );
    }
    
    GeoSceneLayer *layer = new GeoSceneLayer( d->uiWidget.lineEditTheme->text() );
    layer->setBackend( "texture" );
    layer->addDataset( texture );
    
    GeoSceneLayer* secondLayer = new GeoSceneLayer( "standardplaces" );    
    secondLayer->setBackend( "geodata" );
  
    GeoSceneGeodata* cityplacemarks = new GeoSceneGeodata( "cityplacemarks" );
    cityplacemarks->setSourceFile( "cityplacemarks.kml" );
    secondLayer->addDataset( cityplacemarks );
    
    GeoSceneGeodata* baseplacemarks = new GeoSceneGeodata( "baseplacemarks" );
    baseplacemarks->setSourceFile( "baseplacemarks.kml" );
    secondLayer->addDataset( baseplacemarks );
    
    GeoSceneGeodata* elevplacemarks = new GeoSceneGeodata( "elevplacemarks" );
    elevplacemarks->setSourceFile( "elevplacemarks.kml" );
    secondLayer->addDataset( elevplacemarks );
    
    GeoSceneGeodata* observatoryplacemarks = new GeoSceneGeodata( "observatoryplacemarks" );
    observatoryplacemarks->setSourceFile( "observatoryplacemarks.kml" );
    secondLayer->addDataset( observatoryplacemarks );
    
    GeoSceneGeodata* otherplacemarks = new GeoSceneGeodata( "otherplacemarks" );
    otherplacemarks->setSourceFile( "otherplacemarks.kml" );
    secondLayer->addDataset( otherplacemarks );
    
    GeoSceneGeodata* boundaryplacemarks = new GeoSceneGeodata( "boundaryplacemarks" );
    boundaryplacemarks->setSourceFile( "boundaryplacemarks.kml" );
    secondLayer->addDataset( boundaryplacemarks );
    
    GeoSceneMap *map = document->map();
    map->addLayer( layer );
    map->addLayer( secondLayer );
    
    GeoSceneSettings *settings = document->settings();
    GeoSceneLegend *legend = document->legend();
    
    if( d->uiWidget.checkBoxCoord->checkState() == Qt::Checked )
    {
        GeoSceneProperty *coorGrid = new GeoSceneProperty( "coordinate-grid" );
        coorGrid->setDefaultValue( true );
        coorGrid->setAvailable( true );
        settings->addProperty( coorGrid );
        
        GeoSceneSection *coorSection = new GeoSceneSection( "coordinate-grid" );
        coorSection->setHeading( "Coordinate Grid" );
        coorSection->setCheckable( true );
        coorSection->setConnectTo( "coordinate-grid" );
        coorSection->setSpacing( 12 );
        legend->addSection( coorSection );
    }
    
    if( d->uiWidget.checkBoxInterest->checkState() == Qt::Checked )
    {
        GeoSceneProperty *poiProperty = new GeoSceneProperty( "otherplaces" );
        poiProperty->setDefaultValue( true );
        poiProperty->setAvailable( true );
        settings->addProperty( poiProperty );
        
        GeoSceneSection *poiSection = new GeoSceneSection( "otherplaces" );
        poiSection->setHeading( "Places of Interest" );
        poiSection->setCheckable( true );
        poiSection->setConnectTo( "otherplaces" );
        poiSection->setSpacing( 12 );
        
        GeoSceneItem *geoPole = new GeoSceneItem( "geographic-pole" );
        GeoSceneIcon *geoPoleIcon = geoPole->icon();
        geoPole->setText( tr("Geographic Pole") );
        geoPoleIcon->setPixmap( "bitmaps/pole_1.png" );    
        poiSection->addItem( geoPole );
        
        GeoSceneItem *magPole = new GeoSceneItem( "magnetic-pole" );
        GeoSceneIcon *magPoleIcon = magPole->icon();
        magPole->setText( tr("Magnetic Pole") );
        magPoleIcon->setPixmap( "bitmaps/pole_2.png" );    
        poiSection->addItem( magPole );
        
        GeoSceneItem *airport = new GeoSceneItem( "airport" );
        GeoSceneIcon *airportIcon = airport->icon();
        airport->setText( tr("Airport") );
        airportIcon->setPixmap( "bitmaps/airport.png" );    
        poiSection->addItem( airport );
        
        GeoSceneItem *shipwreck = new GeoSceneItem( "shipwreck" );
        GeoSceneIcon *shipwreckIcon = shipwreck->icon();
        shipwreck->setText( tr("Shipwreck") );
        shipwreckIcon->setPixmap( "bitmaps/shipwreck.png" );    
        poiSection->addItem( shipwreck );
        
        GeoSceneItem *observatory = new GeoSceneItem( "observatory" );
        GeoSceneIcon *observatoryIcon = observatory->icon();
        observatory->setText( tr("Observatory") );
        observatoryIcon->setPixmap( "bitmaps/observatory.png" );    
        poiSection->addItem( observatory );
        
        legend->addSection( poiSection );
    }
    
    if( d->uiWidget.checkBoxTer->checkState() == Qt::Checked )
    {
        GeoSceneProperty *terrainProperty = new GeoSceneProperty( "terrain" );
        terrainProperty->setDefaultValue( true );
        terrainProperty->setAvailable( true );
        settings->addProperty( terrainProperty );     
        
        GeoSceneSection *terrainSection = new GeoSceneSection( "terrain" );
        terrainSection->setHeading( "Terrain" );
        terrainSection->setCheckable( true );
        terrainSection->setConnectTo( "terrain" );
        terrainSection->setSpacing( 12 );    
        
        GeoSceneItem *mountain = new GeoSceneItem( "mountain" );
        GeoSceneIcon *mountainIcon = mountain->icon();
        mountain->setText( tr("Mountain") );
        mountainIcon->setPixmap( "bitmaps/mountain_1.png" );    
        terrainSection->addItem( mountain );
        
        GeoSceneItem *volcano = new GeoSceneItem( "volcano" );
        GeoSceneIcon *volcanoIcon = volcano->icon();
        volcano->setText( tr("Volcano") );
        volcanoIcon->setPixmap( "bitmaps/volcano_1.png" );    
        terrainSection->addItem( volcano );   
        
        legend->addSection( terrainSection );
        
    }
    
    if( d->uiWidget.checkBoxPop->checkState() == Qt::Checked )
    {
        GeoSceneProperty *placesProperty = new GeoSceneProperty( "places" );
        placesProperty->setDefaultValue( true );
        placesProperty->setAvailable( true );
        settings->addProperty( placesProperty );
        
        GeoSceneProperty *citiesProperty = new GeoSceneProperty( "cities" );
        citiesProperty->setDefaultValue( true );
        citiesProperty->setAvailable( true );
        settings->addProperty( citiesProperty );
    }
    
    if( d->uiWidget.checkBoxBorder->checkState() == Qt::Checked )
    {
        GeoSceneSection *bordersSection = new GeoSceneSection( "borders" );
        bordersSection->setHeading( "Boundaries" );
        bordersSection->setCheckable( true );
        bordersSection->setConnectTo( "borders" );
        bordersSection->setSpacing( 12 );
        
        GeoSceneItem *internationalBoundary = new GeoSceneItem( "international-boundary" );
        GeoSceneIcon *internationalBoundaryIcon = internationalBoundary->icon();
        internationalBoundary->setText( tr("International") );
        internationalBoundaryIcon->setPixmap( "bitmaps/border_1.png" );    
        bordersSection->addItem( internationalBoundary ); 
        
        GeoSceneItem *stateBoundary = new GeoSceneItem( "state" );
        GeoSceneIcon *stateBoundaryIcon = stateBoundary->icon();
        stateBoundary->setText( tr("State") );
        stateBoundaryIcon->setPixmap( "bitmaps/border_2.png" );
        bordersSection->addItem( stateBoundary );
        
        GeoSceneProperty *bordersProperty = new GeoSceneProperty( "borders" );
        bordersProperty->setDefaultValue( false );
        bordersProperty->setAvailable( true );
        settings->addProperty( bordersProperty );
        
        GeoSceneProperty *intBoundariesProperty = new GeoSceneProperty( "international-boundaries" );
        intBoundariesProperty->setDefaultValue( false );
        intBoundariesProperty->setAvailable( true );
        settings->addProperty( intBoundariesProperty );
        
        GeoSceneProperty *stateBounderiesProperty = new GeoSceneProperty( "state-boundaries" );
        stateBounderiesProperty->setDefaultValue( false );
        stateBounderiesProperty->setAvailable( true );
        settings->addProperty( stateBounderiesProperty );
        
        legend->addSection( bordersSection );
        
        GeoSceneLayer* mwdbii = new GeoSceneLayer( "mwdbii" );
        mwdbii->setBackend( "vector" );
        mwdbii->setRole( "polyline" ); 
        
        GeoSceneVector* vector = new GeoSceneVector( "pdiffborder" );
        vector->setFeature( "border" );
        vector->setFileFormat( "PNT" );
        vector->setSourceFile( "earth/mwdbii/PDIFFBORDER.PNT" );
        vector->pen().setColor( "#ffe300" );
        mwdbii->addDataset( vector );
        map->addLayer( mwdbii );
    }
    
    GeoSceneProperty *overviewmap = new GeoSceneProperty( "overviewmap" );
    overviewmap->setDefaultValue( true );
    overviewmap->setAvailable( true );
    settings->addProperty( overviewmap );
    
    GeoSceneProperty *compass = new GeoSceneProperty( "compass" );
    compass->setDefaultValue( true );
    compass->setAvailable( true );
    settings->addProperty( compass );
    
    GeoSceneProperty *scalebar = new GeoSceneProperty( "scalebar" );
    scalebar->setDefaultValue( true );
    scalebar->setAvailable( true );
    settings->addProperty( scalebar );
    
    return document;
}

void MapWizard::accept()
{
    Q_ASSERT( d->mapProviderType != MapWizardPrivate::NoMap );

    Q_ASSERT( d->format == d->format.toLower() );
    Q_ASSERT( !d->mapTheme.isEmpty() );

    if ( d->mapProviderType == MapWizardPrivate::StaticImageMap )
    {
        d->sourceImage = d->uiWidget.lineEditSource->text();
        Q_ASSERT( !d->sourceImage.isEmpty() );
        Q_ASSERT( QFile( d->sourceImage ).exists() );
    }
    else if ( d->mapProviderType == MapWizardPrivate::WmsMap )
    {
        Q_ASSERT( !d->wmsFetchedMaps.isEmpty() );
        Q_ASSERT( !d->levelZero.isNull() );
        Q_ASSERT( !QImage::fromData( d->levelZero ).isNull() );
    }
    else if ( d->mapProviderType == MapWizardPrivate::StaticUrlMap )
    {
        Q_ASSERT( !d->levelZero.isNull() );
        Q_ASSERT( !QImage::fromData( d->levelZero ).isNull() );
    }

    QSharedPointer<GeoSceneDocument> document( createDocument() );
    Q_ASSERT( !document->head()->description().isEmpty() );
    Q_ASSERT( !document->head()->name().isEmpty() );

    if( createFiles( document.data() ) )
    {
        if( d->mapProviderType == MapWizardPrivate::WmsMap )
        {
            if( d->wmsLegends.isEmpty() && d->wmsLegends.at( d->uiWidget.listWidgetWmsMaps->currentRow() ).isEmpty() )
            {
                downloadLegend( d->wmsLegends.at( d->uiWidget.listWidgetWmsMaps->currentRow() ) );
            }
        } else if( d->mapProviderType == MapWizardPrivate::StaticImageMap || d->mapProviderType == MapWizardPrivate::StaticUrlMap ) {
            createLegend();
        }

        QDialog::accept();
        d->uiWidget.lineEditTitle->clear();
        d->uiWidget.lineEditTheme->clear();
        d->uiWidget.textEditDesc->clear();
        d->uiWidget.labelPreview->clear();
        d->uiWidget.lineEditSource->clear();
        d->dgmlOutput.clear();
        QTimer::singleShot( 0, this, SLOT( restart() ) );
    }

    else
    {
        QMessageBox::critical( this, tr( "Problem while creating files" ), tr( "Check if a theme with the same name exists." ) );
        return;
    }
}

void MapWizard::showPreview()
{
    QSharedPointer<GeoSceneDocument> document( createDocument() );
    
    if( createFiles( document.data() ) )
    {
        if( d->mapProviderType == MapWizardPrivate::WmsMap )
        {
            if( d->wmsLegends.isEmpty() && d->wmsLegends.at( d->uiWidget.listWidgetWmsMaps->currentRow() ).isEmpty() )
            {
                downloadLegend( d->wmsLegends.at( d->uiWidget.listWidgetWmsMaps->currentRow() ) );
            }
        } else if( d->mapProviderType == MapWizardPrivate::StaticImageMap || d->mapProviderType == MapWizardPrivate::StaticUrlMap ) {
            createLegend();
        }
    }
    
    PreviewDialog *previewDialog = new PreviewDialog( this, document.data()->head()->mapThemeId() );
    previewDialog->exec();
}


}

#include "MapWizard.moc"
