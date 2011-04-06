//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Utku Aydın <utkuaydin34@gmail.com>
//

#include "MapWizard.h"
#include "ui_MapWizard.h"

#include "global.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtCore/QTemporaryFile>
#include <QtGui/QPixmap>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QImageReader>
#include <QtGui/QDialogButtonBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtXml/QDomElement>

namespace Marble
{

class MapWizardPrivate
{
public:
    Ui::MapWizard uiWidget;
    QNetworkAccessManager xmlAccessManager;
    QNetworkAccessManager legendAccessManager;
    QNetworkAccessManager levelZeroAccessManager;
    QStringList wmsServerList;

    enum mapType
    {
        NoMap,
        StaticImageMap,
        WmsMap,
        StaticUrlMap
    };

    mapType mapProviderType;
    QString mapTitle;
    QString mapTheme;
    QString mapDescription;
    QString mapPreview;
    QByteArray levelZero;

    QString wmsProtocol;
    QString wmsHost;
    QString wmsPath;
    QString wmsQuery;
    QString wmsFormat;
    QString wmsProjection;
    QStringList wmsLegends;

    QString staticUrlProtocol;
    QString staticUrlHost;
    QString staticUrlPath;
    QString staticUrlQuery;

    QString sourceImage;
    QString previewExtension;
    QString sourceExtension;

    QString dgmlOutput;
};

MapWizard::MapWizard( QWidget* parent ) : QWizard( parent ), d( new MapWizardPrivate )
{
    d->uiWidget.setupUi( this );
    QListIterator<QByteArray> i( QImageReader::supportedImageFormats() );

    while( i.hasNext() ) {
        d->uiWidget.comboBoxStaticUrlFormat->addItem( QString( i.next() ) );
    }

    connect( &( d->xmlAccessManager ), SIGNAL( finished( QNetworkReply* ) ), this, SLOT( parseServerCapabilities( QNetworkReply* ) ) );
    connect( &( d->legendAccessManager ), SIGNAL( finished( QNetworkReply* ) ), this, SLOT( createWmsLegend( QNetworkReply* ) ) );
    connect( &( d->levelZeroAccessManager ), SIGNAL( finished( QNetworkReply* ) ), this, SLOT( createLevelZero( QNetworkReply* ) ) );

    connect( d->uiWidget.pushButtonSource, SIGNAL( clicked( bool ) ), this, SLOT( querySourceImage() ) );
    connect( d->uiWidget.pushButtonPreview, SIGNAL( clicked( bool ) ), this, SLOT( queryPreviewImage() ) );
    connect( d->uiWidget.pushButtonLegend, SIGNAL( clicked( bool ) ), this, SLOT( queryLegendImage() ) );
    connect( d->uiWidget.pushButtonStaticUrlLegend, SIGNAL( clicked( bool ) ), this, SLOT( queryStaticUrlLegendImage() ) );
    connect( d->uiWidget.pushButtonWmsInspect, SIGNAL( clicked( bool ) ), this, SLOT( queryServerCapabilities() ) );
    connect( d->uiWidget.pushButtonStaticUrlTest, SIGNAL( clicked( bool ) ), this, SLOT( downloadLevelZero() ) );
    connect( d->uiWidget.pushButtonSuggest, SIGNAL( clicked( bool ) ), this, SLOT( downloadLevelZero() ) );

    connect( d->uiWidget.comboBoxWmsServer, SIGNAL( currentIndexChanged( QString ) ), d->uiWidget.lineEditWmsUrl, SLOT( setText( QString ) ) );
    connect( d->uiWidget.comboBoxWmsMap, SIGNAL( currentIndexChanged( QString ) ), this, SLOT( autoFillDetails() ) );

    connect( d->uiWidget.lineEditTitle, SIGNAL( textChanged( QString ) ), d->uiWidget.labelSumMName, SLOT( setText( QString ) ) );
    connect( d->uiWidget.lineEditTheme, SIGNAL( textChanged( QString ) ), d->uiWidget.labelSumMTheme, SLOT( setText( QString ) ) );
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

bool MapWizard::parseServerCapabilities( QNetworkReply* reply )
{
    QString result( reply->readAll() );
    QDomDocument xml;
    if( !xml.setContent( result ) )
    {
        QMessageBox::critical( this, tr( "Error while parsing" ), tr( "Wizard can't parse server's response" ) );
        return false;
    }

    if( xml.documentElement().firstChildElement().tagName().isNull() )
    {
        QMessageBox::critical( this, tr( "Error while parsing" ), tr( "Server is not a Web Map Server." ) );
        return false;
    }

    QDomElement firstLayer = xml.documentElement().firstChildElement( "Capability" ).firstChildElement( "Layer" );
    QDomNodeList layers = firstLayer.elementsByTagName( "Layer" );

    d->wmsProjection = firstLayer.firstChildElement( "SRS" ).text();
    d->uiWidget.comboBoxWmsMap->clear();

    for( int i = 0; i < layers.size(); ++i )
    {
        QString theme = layers.at( i ).firstChildElement( "Name" ).text();
        QString title = layers.at( i ).firstChildElement( "Title" ).text();
        QDomElement legendUrl = layers.at( i ).firstChildElement( "Style" ).firstChildElement( "LegendURL" );
        d->uiWidget.comboBoxWmsMap->addItem( title, theme );

        d->wmsLegends.clear();
        if( legendUrl.isNull() ) {
            d->wmsLegends.append( QString() );
        } else {
            d->wmsLegends.append( legendUrl.firstChildElement( "OnlineResource" ).attribute( "xlink:href" ) );
        }
    }

    QDomElement format = xml.documentElement().firstChildElement( "Capability" ).firstChildElement( "Request" )
                         .firstChildElement( "GetMap" ).firstChildElement( "Format" );

    d->wmsFormat = format.text().right( format.text().length() - format.text().indexOf( '/' ) - 1 );

    if( d->wmsFormat == "jpeg" ) {
        d->wmsFormat = "jpg";
    }

    if( !d->uiWidget.comboBoxWmsMap->currentText().isEmpty() && !d->wmsServerList.contains( d->uiWidget.lineEditWmsUrl->text() ) ) {
        d->wmsServerList.append( d->uiWidget.lineEditWmsUrl->text() );
    }

    return true;
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
}

void MapWizard::setWmsServers( const QStringList& uris )
{
    d->wmsServerList = uris;

    d->uiWidget.comboBoxWmsServer->clear();
    d->uiWidget.comboBoxWmsServer->addItems( d->wmsServerList );
}

QStringList MapWizard::wmsServers() const
{
    return d->wmsServerList;
}

void MapWizard::autoFillDetails()
{
    int selected = d->uiWidget.comboBoxWmsMap->currentIndex();
    d->uiWidget.lineEditTitle->setText( d->uiWidget.comboBoxWmsMap->currentText() );
    d->uiWidget.lineEditTheme->setText( d->uiWidget.comboBoxWmsMap->itemData( selected ).toString() );
}

void MapWizard::createDgml()
{
    QXmlStreamWriter stream( &( d->dgmlOutput ) ) ;
    stream.setAutoFormatting( true );

    stream.writeStartDocument();
    stream.writeStartElement( "dgml" );
    stream.writeAttribute( "xmlns", "http://edu.kde.org/marble/dgml/2.0" );
    stream.writeStartElement( "document" );

    stream.writeStartElement( "head" );
    stream.writeTextElement( "name", d->mapTitle );
    stream.writeTextElement( "target", "earth" );
    stream.writeTextElement( "theme", d->mapTheme );
    stream.writeTextElement( "visible", "true" );

    stream.writeStartElement( "icon" );
    stream.writeAttribute( "pixmap", QString( "preview.%1" ).arg( d->previewExtension ) );
    stream.writeEndElement();

    stream.writeStartElement( "description" );
    stream.writeCDATA( d->mapDescription );
    stream.writeEndElement();

    stream.writeStartElement( "zoom" );
    stream.writeTextElement( "discrete", "false" );
    stream.writeTextElement( "minimum", "900" );
    stream.writeTextElement( "maximum", "3500" );
    stream.writeEndElement(); // zoom
    stream.writeEndElement(); // head

    stream.writeStartElement( "map" );
    stream.writeStartElement( "canvas" );
    stream.writeEndElement(); //canvas
    stream.writeStartElement( "target" );
    stream.writeEndElement(); // target

    stream.writeStartElement( "layer" );
    stream.writeAttribute( "name", d->mapTheme );
    stream.writeAttribute( "backend", "texture" );
    stream.writeStartElement( "texture" );
    stream.writeAttribute( "name", "map" );

    if( d->mapProviderType == MapWizardPrivate::WmsMap ) {
        stream.writeAttribute( "expire", "31536000" );
    }

    stream.writeStartElement( "sourcedir" );

    if( d->mapProviderType == MapWizardPrivate::WmsMap ) {
        stream.writeAttribute( "format", d->wmsFormat );
    } else if( d->mapProviderType == MapWizardPrivate::StaticImageMap ) {
        stream.writeAttribute( "format", d->sourceExtension );
    } else if( d->mapProviderType == MapWizardPrivate::StaticUrlMap ) {
        stream.writeAttribute( "format", d->uiWidget.comboBoxStaticUrlFormat->currentText() );
    }

    stream.writeCharacters( QString( "earth/%1" ).arg( d->mapTheme ) );
    stream.writeEndElement(); // sourcedir

    if( d->mapProviderType == MapWizardPrivate::WmsMap )
    {
        stream.writeStartElement( "storageLayout" );
        stream.writeAttribute( "levelZeroColumns", "1" );
        stream.writeAttribute( "levelZeroRows", "1" );
        stream.writeAttribute( "maximumTileLevel", "20" );
        stream.writeAttribute( "mode", "WebMapService" );
        stream.writeEndElement(); // storageLayout

        stream.writeStartElement( "projection" );
        if( d->wmsProjection == "EPSG:4326" ) {
            stream.writeAttribute( "name", "Equirectangular" );
        } else if( d->wmsProjection == "EPSG:900913" || d->wmsProjection == "EPSG:3785" ) {
            stream.writeAttribute( "name", "Mercator" );
        }
        stream.writeEndElement(); // projection

        stream.writeStartElement( "downloadUrl" );
        stream.writeAttribute( "protocol", d->wmsProtocol );
        stream.writeAttribute( "host", d->wmsHost );
        stream.writeAttribute( "path", d->wmsPath );
        stream.writeAttribute( "query", d->wmsQuery );
        stream.writeEndElement(); // projection
    }

    else if( d->mapProviderType == MapWizardPrivate::StaticUrlMap )
    {
        stream.writeStartElement( "storageLayout" );
        stream.writeAttribute( "levelZeroColumns", "1" );
        stream.writeAttribute( "levelZeroRows", "1" );
        stream.writeAttribute( "maximumTileLevel", "20" );
        stream.writeAttribute( "mode", "Custom" );
        stream.writeEndElement(); // storageLayout

        stream.writeStartElement( "projection" );
        stream.writeAttribute( "name", "Mercator" );
        stream.writeEndElement(); // projection

        stream.writeStartElement( "downloadUrl" );
        stream.writeAttribute( "protocol", d->staticUrlProtocol );
        stream.writeAttribute( "host", d->staticUrlHost );
        stream.writeAttribute( "path", d->staticUrlPath );
        stream.writeEndElement(); // projection

        stream.writeStartElement( "downloadPolicy" );
        stream.writeAttribute( "usage", "Browse" );
        stream.writeAttribute( "maximumConnections", "20" );
        stream.writeEndElement(); // projection

        stream.writeStartElement( "downloadPolicy" );
        stream.writeAttribute( "usage", "Bulk" );
        stream.writeAttribute( "maximumConnections", "2" );
        stream.writeEndElement(); // projection
    }

    else if( d->mapProviderType == MapWizardPrivate::StaticImageMap ) {
        stream.writeTextElement( "installmap", QString( "%1.%2" ).arg( d->mapTheme ).arg( d->sourceExtension ) );
    }

    stream.writeEndElement(); // texture
    stream.writeEndElement(); // layer
    stream.writeEndElement(); // map

    stream.writeStartElement( "legend" );
    stream.writeEndElement(); // legend

    stream.writeEndElement(); // document
    stream.writeEndElement(); // dgml
    stream.writeEndDocument();
}

bool MapWizard::createFiles()
{
    // Create directories
    QDir maps( MarbleDirs::localPath() + "/maps/earth/" );
    if( !maps.exists( d->mapTheme ) )
    {
        maps.mkdir( d->mapTheme );

        if( d->mapProviderType == MapWizardPrivate::StaticImageMap )
        {
            // Source image
            QFile sourceImage( d->sourceImage );
            d->sourceExtension = d->sourceImage.right( d->sourceImage.length() - d->sourceImage.lastIndexOf( '.' ) - 1 );
            sourceImage.copy( QString( "%1/%2/%2.%3" ).arg( maps.absolutePath() ).arg( d->mapTheme ).arg( d->sourceExtension ) );
        }

        else if( d->mapProviderType == MapWizardPrivate::WmsMap )
        {
            maps.mkdir( QString( "%1/0/" ).arg( d->mapTheme ) );
            maps.mkdir( QString( "%1/0/0" ).arg( d->mapTheme ) );
            QImage baseTile = QImage::fromData( d->levelZero, d->wmsFormat.toAscii().data() );
            baseTile.save( QString( "%1/%2/0/0/0.%3" ).arg( maps.absolutePath() ).arg( d->mapTheme ).arg( d->wmsFormat ) );
        }

        else if( d->mapProviderType == MapWizardPrivate::StaticUrlMap )
        {
            maps.mkdir( QString( "%1/0/" ).arg( d->mapTheme ) );
            maps.mkdir( QString( "%1/0/0" ).arg( d->mapTheme ) );
            QImage baseTile( 256, 256, QImage::Format_RGB32 );
            baseTile.save( QString( "%1/%2/0/0/0.%3" ).arg( maps.absolutePath() ).arg( d->mapTheme ).arg( d->uiWidget.comboBoxStaticUrlFormat->currentText() ) );
        }

        // Preview image
        d->previewExtension = d->mapPreview.right( d->mapPreview.length() - d->mapPreview.lastIndexOf( '.' ) - 1 ) ;
        QFile previewImage( d->mapPreview );
        previewImage.copy( QString( "%1/%2/preview.%3" ).arg( maps.absolutePath() ).arg( d->mapTheme ).arg( d->previewExtension ) );

        // DGML
        QFile file( QString( "%1/%2/%2.dgml" ).arg( maps.absolutePath() ).arg( d->mapTheme ) );
        file.open( QIODevice::ReadWrite );
        file.write( d->dgmlOutput.toAscii().data() );
        file.close();

        return true;
    }

    else
        return false;
}

void MapWizard::createLegendHtml()
{
    QDir map( QString( "%1/maps/earth/%2" ).arg( MarbleDirs::localPath() ).arg( d->mapTheme ) );

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
    stream.writeAttribute( "src", "./legend/legend.png" );
    stream.writeEndElement();
    stream.writeEndElement();
    stream.writeEndElement();

    QFile html( QString( "%1/legend.html" ).arg( map.absolutePath() ) );
    html.open( QIODevice::ReadWrite );
    html.write( htmlOutput.toAscii().data() );
    html.close();
}

void MapWizard::downloadLegend( const QString url )
{
    QUrl downloadUrl( url );
    QNetworkRequest request( downloadUrl );
    d->legendAccessManager.get( request );
}

void MapWizard::downloadLevelZero()
{
    if( d->mapProviderType == MapWizardPrivate::WmsMap )
    {
        int selected = d->uiWidget.comboBoxWmsMap->currentIndex();
        
        QUrl downloadUrl( d->uiWidget.lineEditWmsUrl->text() );
        downloadUrl.addQueryItem( "request", "GetMap" );
        downloadUrl.addQueryItem( "layers", d->uiWidget.comboBoxWmsMap->itemData( selected ).toString() );
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
        QString server = d->uiWidget.lineEditStaticUrlServer->text();
        QString format = d->uiWidget.comboBoxStaticUrlFormat->currentText();
        QUrl downloadUrl;

        if( server.indexOf( "{x}" ) != -1 && server.indexOf( "{y}" ) != -1 )
        {
            server.replace( server.indexOf( "{x}" ), 3,  QString::number( 0 ) );
            server.replace( server.indexOf( "{y}" ), 3,  QString::number( 0 ) );
            server.replace( server.indexOf( "{zoomLevel}" ), 11,  QString::number( 0 ) );
            downloadUrl.setUrl( server );
        } else {
            downloadUrl.setUrl( QString( "%1/0/0/0.%2" ).arg( server ).arg( format ) );
        }

        QNetworkRequest request( downloadUrl );
        d->levelZeroAccessManager.get( request );
    }
    
    else if( d->mapProviderType == MapWizardPrivate::StaticImageMap ){
        suggestPreviewImage();
    }
}

void MapWizard::createLevelZero( QNetworkReply* reply )
{
    if( d->mapProviderType == MapWizardPrivate::WmsMap || d->mapProviderType == MapWizardPrivate::StaticUrlMap )
    {
        QByteArray result = reply->readAll();
        d->levelZero = result;
    }
    
    if( d->mapProviderType == MapWizardPrivate::StaticUrlMap )
    {
        QPixmap levelZero;
        levelZero.loadFromData( d->levelZero );
	if ( !d->levelZero.isNull() ) {
	    levelZero = levelZero.scaled( d->uiWidget.labelStaticUrlTest->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation );
	    d->uiWidget.labelStaticUrlTest->setPixmap( levelZero );
	}
    }
    
    suggestPreviewImage();
}

void MapWizard::createLegend()
{
    QDir map( QString( "%1/maps/earth/%2" ).arg( MarbleDirs::localPath() ).arg( d->mapTheme ) );
    if( !map.exists( "legend" ) ) {
        map.mkdir( "legend" );
    }

    QFile image;

    if( d->mapProviderType == MapWizardPrivate::StaticImageMap ) {
        image.setFileName( d->uiWidget.lineEditLegend->text() );
    }

    if( d->mapProviderType == MapWizardPrivate::StaticUrlMap ) {
        image.setFileName( d->uiWidget.lineEditStaticUrlLegend->text() );
    }

    image.copy( QString( "%1/legend/legend.png" ).arg( map.absolutePath() ) );

    createLegendHtml();
}

void MapWizard::querySourceImage()
{
    d->uiWidget.lineEditSource->setText( QFileDialog::getOpenFileName() );
}

void MapWizard::queryPreviewImage()
{
    QString fileName = QFileDialog::getOpenFileName();
    d->uiWidget.lineEditPreview->setText( fileName );

    QPixmap preview( fileName );
    d->uiWidget.labelThumbnail->setPixmap( QPixmap( fileName ) );
    d->uiWidget.labelThumbnail->resize( preview.width(), preview.height() );
}

void MapWizard::queryLegendImage()
{
    QString fileName = QFileDialog::getOpenFileName();
    d->uiWidget.lineEditLegend->setText( fileName );
}

void MapWizard::queryStaticUrlLegendImage()
{
    QString fileName = QFileDialog::getOpenFileName();
    d->uiWidget.lineEditStaticUrlLegend->setText( fileName );
}

QString MapWizard::createArchive( QString mapId )
{
    QStringList splitMapId( mapId.split("/") );
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
        QMessageBox::critical( this, tr( "Archiving failed" ), tr( "Archiving process cannot be started." ) );
        break;
    case -1:
        QMessageBox::critical( this, tr( "Archiving failed" ), tr( "Archiving process crashed." ) );
        break;
    case 0:
        mDebug() << "Archived the theme sucessfully.";
        break;
    }
    archiver.waitForFinished();
    return QString( "%1/%2.tar.gz" ).arg( QDir::tempPath() ).arg( theme ); 
}

void MapWizard::deleteArchive( QString mapId )
{
    QStringList splitMapId( mapId.split("/") );
    QString theme = splitMapId[1];
    QFile::remove( QString( "%1/%2.tar.gz" ).arg( QDir::tempPath() ).arg( theme ) );
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
            return 2;
        } else if( d->uiWidget.radioButtonStaticUrl->isChecked() ) {
            d->mapProviderType = MapWizardPrivate::StaticUrlMap;
            return 3;
        }
        break;

    case 1:
        return 4;
        break;

    case 2:
        return 4;
        break;

    case 5:
        return -1;
        break;

    default:
        break;

    }
    
    return currentId() + 1;
}

void MapWizard::suggestPreviewImage()
{   
    if( currentId() == 4 )
    {
        QPointer<QDialog> dialog = new QDialog;
        QPixmap pixmap;
        if( d->mapProviderType == MapWizardPrivate::StaticImageMap ) {
            pixmap = QPixmap( d->uiWidget.lineEditSource->text() );
        } else if ( d->mapProviderType == MapWizardPrivate::WmsMap || d->mapProviderType == MapWizardPrivate::StaticUrlMap ){
            pixmap.loadFromData( d->levelZero );
        }
        
        QLabel *previewImage = new QLabel();
        previewImage->setPixmap( pixmap.scaled( 136, 136, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
        
        QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
        connect( buttonBox, SIGNAL( accepted() ), dialog, SLOT( accept() ) );
        connect( buttonBox, SIGNAL( rejected() ), dialog, SLOT( reject() ) );
        
        QGridLayout *grid = new QGridLayout();
        grid->addWidget( previewImage );
        grid->addWidget( buttonBox );
        
        dialog->setLayout( grid );
        if( dialog->exec() )
        {
            
            QTemporaryFile tempFile;
            tempFile.open();
            
            if( d->mapProviderType == MapWizardPrivate::StaticImageMap )
            {
                QImage tempPreview = pixmap.toImage();
                tempPreview.save( tempFile.fileName() + ".png" );
                d->uiWidget.lineEditPreview->setText( tempFile.fileName() + ".png" );
            }
            
            else if( d->mapProviderType == MapWizardPrivate::StaticUrlMap )
            {
                QImage tempPreview;
                tempPreview = QImage::fromData( d->levelZero );
                tempPreview.save( tempFile.fileName() + "."  + d->uiWidget.comboBoxStaticUrlFormat->currentText() );
                d->uiWidget.lineEditPreview->setText( tempFile.fileName() + "." + d->uiWidget.comboBoxStaticUrlFormat->currentText() );
            }
            
            else if ( d->mapProviderType == MapWizardPrivate::WmsMap )
            {
                QImage tempPreview;
                tempPreview = QImage::fromData( d->levelZero );
                tempPreview.save( tempFile.fileName() + "." + d->wmsFormat );
                d->uiWidget.lineEditPreview->setText( tempFile.fileName() + "." + d->wmsFormat );
            }
            
            d->uiWidget.labelThumbnail->setPixmap( pixmap.scaled( 136, 136, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
        }
        delete dialog;
    }
}

void MapWizard::accept()
{
    d->mapTitle = d->uiWidget.lineEditTitle->text();
    d->mapTheme = d->uiWidget.lineEditTheme->text();
    d->mapDescription = d->uiWidget.textEditDesc->document()->toHtml();
    d->sourceImage = d->uiWidget.lineEditSource->text();
    d->mapPreview = d->uiWidget.lineEditPreview->text();
    d->sourceExtension = d->sourceImage.right( d->sourceImage.length() - d->sourceImage.lastIndexOf( '.' ) - 1 );
    d->previewExtension = d->mapPreview.right( d->mapPreview.length() - d->mapPreview.lastIndexOf( '.' ) - 1 );

    QString wmsUrl = d->uiWidget.lineEditWmsUrl->text();

    if( d->uiWidget.radioButtonWms->isChecked() )
    {
        QUrl wmsUrl( d->uiWidget.lineEditWmsUrl->text() );
        d->wmsProtocol = wmsUrl.toString().left( wmsUrl.toString().indexOf( ':' ) );
        d->wmsHost =  QString( wmsUrl.encodedHost() );
        d->wmsPath =  QString( wmsUrl.encodedPath() );
        d->wmsQuery = QString( "layers=%1&%2" ).arg( d->uiWidget.comboBoxWmsMap->itemData( d->uiWidget.comboBoxWmsMap->currentIndex() ).toString() )
                      .arg( QString( wmsUrl.encodedQuery() ) );
    }

    else if( d->uiWidget.radioButtonStaticUrl->isChecked() )
    {
        QUrl staticImageUrl( d->uiWidget.lineEditStaticUrlServer->text() );
        d->staticUrlProtocol = staticImageUrl.toString().left( staticImageUrl.toString().indexOf( ':' ) );
        d->staticUrlHost =  QString( staticImageUrl.encodedHost() );
        d->staticUrlPath =  QUrl::fromPercentEncoding( staticImageUrl.encodedPath() );
    }

    Q_ASSERT( d->mapProviderType != MapWizardPrivate::NoMap );

    if( d->mapProviderType == MapWizardPrivate::StaticImageMap && d->uiWidget.lineEditSource->text().isEmpty() )
    {
        QMessageBox::information( this, tr( "Problem with map information" ), tr( "Please specify a source image." ) );
        return;
    }

    if( d->mapProviderType == MapWizardPrivate::WmsMap && d->uiWidget.comboBoxWmsMap->currentText().isEmpty() )
    {
        QMessageBox::information( this, tr( "Problem with map information" ), tr( "Please choose a map." ) );
        return;
    }

    if( !d->mapTitle.isEmpty() && !d->mapDescription.isEmpty() && !d->mapDescription.isEmpty() && !d->mapPreview.isEmpty() )
    {
        if( d->mapProviderType == MapWizardPrivate::StaticImageMap && !QFile( d->sourceImage ).exists() )
        {
            QMessageBox::critical( this, tr( "File not found" ), tr( "Source image is not found." ) );
            return;
        }

        if( !QFile( d->mapPreview ).exists() )
        {
            QMessageBox::critical( this, tr( "File not found" ), tr( "Preview image is not found." ) );
            return;
        }

        createDgml();

        if( createFiles() )
        {
            if( d->mapProviderType == MapWizardPrivate::WmsMap )
            {
                if( d->wmsLegends.isEmpty() && d->wmsLegends.at( d->uiWidget.comboBoxWmsMap->currentIndex() ).isEmpty() )
                    downloadLegend( d->wmsLegends.at( d->uiWidget.comboBoxWmsMap->currentIndex() ) );
            } else if( d->mapProviderType == MapWizardPrivate::StaticImageMap || d->mapProviderType == MapWizardPrivate::StaticUrlMap ) {
                createLegend();
            }

            QDialog::accept();
            d->uiWidget.lineEditTitle->clear();
            d->uiWidget.lineEditTheme->clear();
            d->uiWidget.textEditDesc->clear();
            d->uiWidget.lineEditPreview->clear();
            d->uiWidget.lineEditSource->clear();
            d->dgmlOutput = QString();
            QTimer::singleShot( 0, this, SLOT( restart() ) );
        }

        else
        {
            QMessageBox::critical( this, tr( "Problem while creating files" ), tr( "Check if a theme with the same name exists." ) );
            return;
        }
    }

    else
    {
        QMessageBox::information( this, tr( "Empty fields" ), tr( "Sorry, some required information is missing. Please fill in all fields." ) );
        return;
    }
}

}

#include "MapWizard.moc"
