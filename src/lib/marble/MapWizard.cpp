// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
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
#include "GeoSceneTileDataset.h"
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
#include "OwsServiceManager.h"

#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QSharedPointer>
#include <QTimer>
#include <QPixmap>
#include <QMessageBox>
#include <QFileDialog>
#include <QImageReader>
#include <QDialogButtonBox>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDomElement>
#include <QXmlStreamWriter>
#include <QUrlQuery>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

namespace Marble
{

int const layerIdRole = 1001;

enum wizardPage
{
    WelcomePage,
    WmsSelectionPage,
    LayerSelectionPage,
    GlobalSourceImagePage,
    XYZUrlPage,
    ThemeInfoPage,
    LegendPage,
    SummaryPage
};

class MapWizardPrivate
{
public:
    MapWizardPrivate()
        : m_serverCapabilitiesValid( false ),
          m_levelZeroTileValid( false ),
          m_previewImageValid( false ),
          m_legendImageValid( false ),
          mapProviderType(),
          model(new QStandardItemModel()),
          sortModel(new QSortFilterProxyModel())

    {
        sortModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        sortModel->setSourceModel(model);
    }

    void pageEntered( int id );

    Ui::MapWizard uiWidget;

    QString mapTheme;

    OwsServiceManager owsManager;

    QStringList wmsServerList;
    QStringList staticUrlServerList;
    bool m_serverCapabilitiesValid;
    bool m_levelZeroTileValid;
    bool m_previewImageValid;
    bool m_legendImageValid;

    enum mapType
    {
        NoMap,
        StaticImageMap,
        WmsMap,
        StaticUrlMap
    };

    mapType mapProviderType;
    QByteArray levelZero;
    QByteArray preview;
    QByteArray legend;
    QImage levelZeroTile;
    QImage previewImage;
    QImage legendImage;

    QString format;

    QStringList wmsLegends;

    QString sourceImage;

    QStandardItemModel * model;
    QSortFilterProxyModel * sortModel;
};

class PreviewDialog : public QDialog
{
    Q_OBJECT
public:
    PreviewDialog( QWidget* parent, const QString& mapThemeId );
    void closeEvent(QCloseEvent* e ) override;
private:
    bool deleteTheme( const QString& directory );
    QString m_mapThemeId;
};

PreviewDialog::PreviewDialog( QWidget* parent, const QString& mapThemeId ) : QDialog( parent ), m_mapThemeId( mapThemeId )
{
    QGridLayout *layout = new QGridLayout();
    MarbleWidget *widget = new MarbleWidget();
    MarbleNavigator *navigator = new MarbleNavigator();
    
    connect( navigator, SIGNAL(goHome()), widget, SLOT(goHome()) );
    connect( navigator, SIGNAL(moveUp()), widget, SLOT(moveUp()) );
    connect( navigator, SIGNAL(moveDown()), widget, SLOT(moveDown()) );
    connect( navigator, SIGNAL(moveLeft()), widget, SLOT(moveLeft()) );
    connect( navigator, SIGNAL(moveRight()), widget, SLOT(moveRight()) );
    connect( navigator, SIGNAL(zoomIn()), widget, SLOT(zoomIn()) );
    connect( navigator, SIGNAL(zoomOut()), widget, SLOT(zoomOut()) );
    connect( navigator, SIGNAL(zoomChanged(int)), widget, SLOT(setZoom(int)) );
    
    widget->setMapThemeId( m_mapThemeId );
    widget->setZoom( 1000 );
    
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
    const QString dgmlPath = MarbleDirs::localPath() + QLatin1String("/maps/") + m_mapThemeId;
    const QString directory = dgmlPath.left(dgmlPath.lastIndexOf(QLatin1Char('/')));
    this->deleteTheme( directory );
    QDialog::closeEvent( e );
}

bool PreviewDialog::deleteTheme( const QString &directory )
{
    QDir dir(directory);
    bool result = true;
 
    if (dir.exists(directory)) {
        for(const QFileInfo& info: dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
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
    if ( id == WmsSelectionPage ) {
        m_serverCapabilitiesValid = false;
        uiWidget.lineEditWmsUrl->setFocus();
    } else if ( id == LayerSelectionPage || id == GlobalSourceImagePage ) {
        m_legendImageValid = false;
        m_previewImageValid = false;
        levelZero.clear();
        uiWidget.comboBoxStaticUrlServer->clear();
        uiWidget.comboBoxStaticUrlServer->addItems( staticUrlServerList );
        uiWidget.comboBoxStaticUrlServer->addItem( "http://" );
    } else if ( id == ThemeInfoPage ) {
        m_legendImageValid = false;
        if ( mapProviderType == MapWizardPrivate::StaticImageMap ) {
            previewImage = QImage( uiWidget.lineEditSource->text() ).scaled( 136, 136, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
            uiWidget.labelPreview->setPixmap( QPixmap::fromImage( previewImage ) );
        }
    } else if ( id == LegendPage ) {
        m_levelZeroTileValid = false;
    }
}

MapWizard::MapWizard( QWidget* parent ) : QWizard( parent ), d( new MapWizardPrivate )
{
    d->uiWidget.setupUi( this );
    
    connect( this, SIGNAL(currentIdChanged(int)), this, SLOT(pageEntered(int)) );

    connect( &d->owsManager, &OwsServiceManager::wmsCapabilitiesReady,
             this, &MapWizard::processCapabilitiesResults);
    connect( &d->owsManager, &OwsServiceManager::imageRequestResultReady,
             this, &MapWizard::processImageResults);

    connect( d->uiWidget.pushButtonSource, &QAbstractButton::clicked, this, &MapWizard::querySourceImage );
    connect( d->uiWidget.pushButtonPreview, &QAbstractButton::clicked, this, &MapWizard::queryPreviewImage );
    connect( d->uiWidget.pushButtonLegend_2, &QAbstractButton::clicked, this, &MapWizard::queryLegendImage );

    connect( d->uiWidget.comboBoxWmsServer, SIGNAL(activated(QString)), this, SLOT(setLineEditWms(QString)) );
    connect( d->uiWidget.listViewWmsLayers, &QListView::pressed, this, &MapWizard::processSelectedLayerInformation );

    connect( d->uiWidget.lineEditTitle, &QLineEdit::textChanged, d->uiWidget.labelSumMName, &QLabel::setText );
    connect( d->uiWidget.lineEditTheme, &QLineEdit::textChanged, d->uiWidget.labelSumMTheme, &QLabel::setText );
    
    connect( d->uiWidget.pushButtonPreviewMap, &QAbstractButton::clicked, this, &MapWizard::showPreview );
    connect( d->uiWidget.lineEditWmsSearch, &QLineEdit::textChanged, this, &MapWizard::updateSearchFilter );

    d->uiWidget.checkBoxWmsMultipleSelections->setVisible(false);
    d->uiWidget.comboBoxWmsBackdrop->setVisible(false);
    d->uiWidget.labelWmsBackdrop->setVisible(false);

    d->uiWidget.listViewWmsLayers->setModel(d->sortModel);

    d->uiWidget.progressBarWmsCapabilities->setVisible(false);
    setLayerButtonsVisible(true);
}

MapWizard::~MapWizard()
{
    delete d;
}

void MapWizard::processCapabilitiesResults()
{
    d->uiWidget.progressBarWmsCapabilities->setVisible(false);

    button( MapWizard::NextButton )->setEnabled( true );

    WmsCapabilities capabilities = d->owsManager.wmsCapabilities();
    if (capabilities.capabilitiesStatus() == WmsCapabilitiesReplyUnreadable)
    {
        QMessageBox::critical( this, tr( "Error while parsing" ), tr( "Wizard cannot parse server's response" ) );
        return;
    }
    if(capabilities.capabilitiesStatus() == WmsCapabilitiesNoWmsServer)
    {
        QMessageBox::critical( this, tr( "Error while parsing" ), tr( "Server is not a Web Map Server." ) );
        return;
    }    
    d->uiWidget.labelWmsTitle->setText(QString("<b>%1</b>").arg(d->owsManager.wmsCapabilities().title() ) );
    d->uiWidget.labelWmsTitle->setToolTip(QString("<small>%1</small>").arg(d->owsManager.wmsCapabilities().abstract() ) );
    d->model->clear();

    for (auto layer : capabilities.layers()) {
        if (!layer.isEmpty()) {
                QStandardItem * item = new QStandardItem(d->owsManager.wmsCapabilities().title(layer));
                item->setData(layer, layerIdRole);
                item->setToolTip(d->owsManager.wmsCapabilities().abstract(layer));
                d->model->appendRow(item);
        }
    }

    // default to the first layer
    d->uiWidget.listViewWmsLayers->setCurrentIndex( d->sortModel->index(0,0) );
    d->uiWidget.lineEditWmsSearch->setText(QString());

    d->uiWidget.comboBoxWmsFormat->clear();
    d->uiWidget.comboBoxWmsFormat->addItems(capabilities.formats());

    // default to png or jpeg
    d->uiWidget.comboBoxWmsFormat->setCurrentText("png");
    if (d->uiWidget.comboBoxWmsFormat->currentText() != "png") {
        d->uiWidget.comboBoxWmsFormat->setCurrentText("jpeg");
    }
    QString serviceInfo;
    serviceInfo += d->owsManager.wmsCapabilities().abstract();

    d->uiWidget.textEditWmsServiceInfo->setText(serviceInfo);
    d->uiWidget.tabServiceInfo->setEnabled(!serviceInfo.isEmpty());

    if (d->uiWidget.listViewWmsLayers->model()->rowCount() > 0) {
        processSelectedLayerInformation();
    }

    d->m_serverCapabilitiesValid = true;

    if (!d->wmsServerList.contains(d->uiWidget.lineEditWmsUrl->text())) {
        d->wmsServerList.append( d->uiWidget.lineEditWmsUrl->text() );
    }
    setWmsServers( d->wmsServerList );


    next();

    setSearchFieldVisible(d->model->rowCount() > 20);

}

void MapWizard::processSelectedLayerInformation()
{
    QString selected = d->sortModel->data(d->uiWidget.listViewWmsLayers->currentIndex(), layerIdRole).toString();
    WmsCapabilities capabilities = d->owsManager.wmsCapabilities();
    d->uiWidget.comboBoxWmsMaps->clear();
    QMap<QString, QString> epsgToText;
    epsgToText["epsg:3857"] = tr("Web Mercator (epsg:3857)");
    epsgToText["epsg:4326"] = tr("Equirectangular (epsg:4326)");
    QStringList projectionTextList;
    for (auto projection : capabilities.projections(selected)) {
        projectionTextList << epsgToText[projection];
    }
    d->uiWidget.comboBoxWmsMaps->addItems(projectionTextList);

    // default to Web Mercator
    d->uiWidget.comboBoxWmsMaps->setCurrentText(tr("Web Mercator (epsg:3857)"));
/*
    bool projectionSelectionVisible = d->uiWidget.comboBoxWmsMaps->count() > 0;
    d->uiWidget.comboBoxWmsMaps->setVisible(projectionSelectionVisible);
    d->uiWidget.labelWmsTileProjection->setVisible(projectionSelectionVisible);
    d->uiWidget.comboBoxWmsMaps->setEnabled(d->uiWidget.comboBoxWmsMaps->count() > 1);
*/
    d->uiWidget.lineEditTitle->setText( d->owsManager.wmsCapabilities().title(selected) );

    // Remove all invalid characters from the theme-String
    // that will make the TileLoader malfunction.
    QString themeString = selected;
    themeString.remove(QRegExp("[:\"\\\\/]"));
    d->uiWidget.lineEditTheme->setText( themeString );
    QRegularExpression rx("^[^:\"\\\\/]*$");
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    d->uiWidget.lineEditTheme->setValidator(validator);

    QString description;
    description += d->owsManager.wmsCapabilities().abstract(selected);
    description += QString("<br><br><i>Contact:</i> %1").arg( d->owsManager.wmsCapabilities().contactInformation());
    description += QString("<br><br><i>Fees:</i> %1").arg( d->owsManager.wmsCapabilities().fees());
    d->uiWidget.textEditDesc->setText(description);

    QString layerInfo;
    layerInfo += d->owsManager.wmsCapabilities().abstract(selected);
    d->uiWidget.tabLayerInfo->setEnabled(!layerInfo.isEmpty());
    d->uiWidget.textEditWmsLayerInfo->setText(layerInfo);
}

void MapWizard::processImageResults()
{
    setLayerButtonsVisible(true);
    QString imageType;
    if (d->owsManager.imageRequestResult().resultType() == PreviewImage) {
        d->m_previewImageValid = true;
        imageType = tr("Preview Image");
    }
    if (d->owsManager.imageRequestResult().resultType() == LevelZeroTile) {
        d->m_levelZeroTileValid = true;
        imageType = tr("Base Tile");
    }
    if (d->owsManager.imageRequestResult().resultType() == LevelZeroTile) {
        d->m_legendImageValid = true;
        imageType = tr("Legend Image");
    }

    button( MapWizard::NextButton )->setEnabled( true );

    if (d->owsManager.imageRequestResult().imageStatus() == WmsImageFailed ) {
        QMessageBox::information( this,
                                    tr( "%1" ).arg(imageType),
                                    tr( "The %1 could not be downloaded." ).arg(imageType) );
        if (imageType == PreviewImage) d->m_previewImageValid = false;
        if (imageType == LevelZeroTile) d->m_levelZeroTileValid = false;
        if (imageType == LegendImage) d->m_legendImageValid = false;
    }
    else if  (d->owsManager.imageRequestResult().imageStatus() == WmsImageFailedServerMessage ) {
        QMessageBox::information( this,
                                    tr( "%1" ).arg(imageType),
                                    tr( "The %1 could not be downloaded successfully. The server replied:\n\n%2" ).arg( imageType, QString( d->owsManager.resultRaw() ) ) );
        if (imageType == PreviewImage) d->m_previewImageValid = false;
        if (imageType == LevelZeroTile) d->m_levelZeroTileValid = false;
        if (imageType == LegendImage) d->m_legendImageValid = false;
    }
    else {
        if (d->owsManager.imageRequestResult().resultType() == PreviewImage) {
            d->previewImage = d->owsManager.resultImage().scaled( 100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
            QPixmap previewPixmap = QPixmap::fromImage( d->previewImage );
            d->uiWidget.labelThumbnail->setPixmap( previewPixmap );
            d->uiWidget.labelThumbnail->resize( QSize(100, 100) );
            d->uiWidget.labelPreview->setPixmap( previewPixmap );
            d->uiWidget.labelPreview->resize( QSize(100, 100) );
        }
        if (d->owsManager.imageRequestResult().resultType() == LevelZeroTile) {
            d->levelZeroTile = d->owsManager.resultImage();
            d->levelZero = d->owsManager.resultRaw();
        }
        if (d->owsManager.imageRequestResult().resultType() == LegendImage) {
            d->legendImage = d->owsManager.resultImage();
            d->legend = d->owsManager.resultRaw();
            QPixmap legendPixmap = QPixmap::fromImage( d->legendImage );
            d->uiWidget.labelLegendImage->resize(legendPixmap.size());
            d->uiWidget.labelLegendImage->setPixmap( legendPixmap );
        }
        next();
    }
}

void MapWizard::createWmsLegend()
{
    QDir map(MarbleDirs::localPath() + QLatin1String("/maps/earth/") + d->mapTheme);
    if( !map.exists( "legend" ) ) {
        map.mkdir( "legend" );
    }

    QFile image(map.absolutePath() + QLatin1String("/legend/legend.png"));
    image.open( QIODevice::ReadWrite );
    image.write( d->legend );
    image.close();

    const QString legendHtml = createLegendHtml();
    createLegendFile( legendHtml );
}

void MapWizard::setWmsServers( const QStringList& uris )
{
    d->wmsServerList = uris;

    d->uiWidget.comboBoxWmsServer->clear();
    d->uiWidget.comboBoxWmsServer->addItems( d->wmsServerList );
    d->uiWidget.comboBoxWmsServer->addItem( tr( "Custom" ), "http://" );
    d->uiWidget.comboBoxWmsServer->setCurrentText( tr( "Custom" ) );
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


void MapWizard::setLineEditWms(const QString &text)
{
    if (text == tr("Custom")) {
        d->uiWidget.lineEditWmsUrl->setText(QString());
    }
    else {
        d->uiWidget.lineEditWmsUrl->setText(text);
    }
}

void MapWizard::setLayerButtonsVisible(bool visible)
{
    d->uiWidget.checkBoxWmsMultipleSelections->setVisible(false);
    d->uiWidget.comboBoxWmsBackdrop->setVisible(false);
    d->uiWidget.comboBoxWmsFormat->setVisible(visible);
    d->uiWidget.comboBoxWmsMaps->setVisible(visible);
    d->uiWidget.labelWmsTileFormat->setVisible(visible);
    d->uiWidget.labelWmsTileProjection->setVisible(visible);
    d->uiWidget.progressBarWmsLayers->setVisible(!visible);
}

void MapWizard::setSearchFieldVisible(bool visible)
{
    d->uiWidget.labelWmsSearch->setVisible(visible);
    d->uiWidget.lineEditWmsSearch->setText(QString());
    d->uiWidget.lineEditWmsSearch->setVisible(visible);
    d->uiWidget.lineEditWmsSearch->setFocus();
}

bool MapWizard::createFiles( const GeoSceneDocument* document )
{
    // Create directories
    QDir maps(MarbleDirs::localPath() + QLatin1String("/maps/earth/"));
    if( !maps.exists( document->head()->theme() ) )
    {
        maps.mkdir( document->head()->theme() );

        if( d->mapProviderType == MapWizardPrivate::StaticImageMap )
        {
            // Source image
            QFile sourceImage( d->sourceImage );
            d->format = d->sourceImage.right(d->sourceImage.length() - d->sourceImage.lastIndexOf(QLatin1Char('.')) - 1).toLower();
            sourceImage.copy( QString( "%1/%2/%2.%3" ).arg( maps.absolutePath(),
                                                            document->head()->theme(),
                                                            d->format ) );
        }

        else if( d->mapProviderType == MapWizardPrivate::WmsMap )
        {
            maps.mkdir( QString( "%1/0/" ).arg( document->head()->theme() ) );
            maps.mkdir( QString( "%1/0/0" ).arg( document->head()->theme() ) );
            const QString path = QString( "%1/%2/0/0/0.%3" ).arg( maps.absolutePath(),
                                                                  document->head()->theme(),
                                                                  d->owsManager.resultFormat() );
            QFile baseTile( path );
            baseTile.open( QFile::WriteOnly );
            baseTile.write( d->levelZero );
        }

        else if( d->mapProviderType == MapWizardPrivate::StaticUrlMap )
        {
            maps.mkdir( QString( "%1/0/" ).arg( document->head()->theme() ) );
            maps.mkdir( QString( "%1/0/0" ).arg( document->head()->theme() ) );
            const QString path = QString( "%1/%2/0/0/0.%3" ).arg( maps.absolutePath(),
                                                                  document->head()->theme(),
                                                                  d->format );
            QFile baseTile( path );
            baseTile.open( QFile::WriteOnly );
            baseTile.write( d->levelZero );
        }

        // Preview image
        QString pixmapPath = QString( "%1/%2/%3" ).arg( maps.absolutePath(),
                                                        document->head()->theme(),
                                                        document->head()->icon()->pixmap() );
        d->previewImage.save( pixmapPath );

        // DGML
        QFile file( QString( "%1/%2/%2.dgml" ).arg( maps.absolutePath(),
                                                    document->head()->theme() ) );
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

QString MapWizard::createLegendHtml( const QString& image )
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

    return htmlOutput;
}

void MapWizard::createLegendFile( const QString& legendHtml )
{
    QDir map(MarbleDirs::localPath() + QLatin1String("/maps/earth/") + d->mapTheme);

    QFile html(map.absolutePath() + QLatin1String("/legend.html"));
    html.open( QIODevice::ReadWrite );
    html.write( legendHtml.toLatin1().data() );
    html.close();
}

void MapWizard::createLegend()
{
    QDir map(MarbleDirs::localPath() + QLatin1String("/maps/earth/") + d->mapTheme);
    if( !map.exists( "legend" ) ) {
        map.mkdir( "legend" );
    }

    QFile image;
    image.setFileName( d->uiWidget.lineEditLegend_2->text() );
    image.copy(map.absolutePath() + QLatin1String("/legend/legend.png"));

    const QString legendHtml = createLegendHtml();
    createLegendFile( legendHtml );
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
    d->uiWidget.lineEditLegend_2->setText(fileName);
    QImage legendImage(fileName);
    QPixmap legendPixmap = QPixmap::fromImage( legendImage );
    d->uiWidget.labelLegendImage->setPixmap(legendPixmap);
}

QString MapWizard::createArchive( QWidget *parent, const QString& mapId )
{
    QStringList splitMapId( mapId.split(QLatin1Char('/')) );
    QString body = splitMapId[0];
    QString theme = splitMapId[1];
    QDir themeDir;

    QStringList tarArgs;
    tarArgs.append( "--create" );
    tarArgs.append( "--gzip" );
    tarArgs.append( "--file" );
    tarArgs.append( QString( "%1/%2.tar.gz" ).arg( QDir::tempPath(), theme ) );
    tarArgs.append( "--directory" );

    if( QFile::exists( QString( "%1/maps/%2" ).arg( MarbleDirs::localPath(), mapId ) ) )
    {
        tarArgs.append( QString( "%1/maps/" ).arg( MarbleDirs::localPath() ) );
        themeDir.cd( QString( "%1/maps/%2/%3" ).arg( MarbleDirs::localPath(), body, theme ) );
    }
    
    else if( QFile::exists( QString( "%1/maps/%2" ).arg( MarbleDirs::systemPath(), mapId ) ) )
    {
        tarArgs.append( QString( "%1/maps/" ).arg( MarbleDirs::systemPath() ) );
        themeDir.cd( QString( "%1/maps/%2/%3" ).arg( MarbleDirs::systemPath(), body, theme ) );
    }
    
    if( QFile::exists( QString( "%1/%2.dgml" ).arg( themeDir.absolutePath(), theme ) ) )
    {
        tarArgs.append( QString( "%1/%2/%2.dgml" ).arg( body, theme ) );
    }
    
    if( QFile::exists( QString( "%1/legend.html" ).arg( themeDir.absolutePath() ) ) )
    {
        tarArgs.append( QString( "%1/%2/legend.html" ).arg( body, theme ) );
    }
    
    if( QFile::exists( QString( "%1/legend" ).arg( themeDir.absolutePath() ) ) )
    {
        tarArgs.append( QString( "%1/%2/legend" ).arg( body, theme ) );
    }
    
    if( QFile::exists( QString( "%1/0/000000" ).arg( themeDir.absolutePath() ) ) )
    {
        tarArgs.append( QString( "%1/%2/0/000000" ).arg( body, theme ) );
    }
    
    QStringList previewFilters;
    previewFilters << "preview.*";
    QStringList preview = themeDir.entryList( previewFilters );
    if( !preview.isEmpty() ) {
        tarArgs.append( QString( "%1/%2/%3" ).arg( body ).arg( theme, preview[0] ) );
    }
    
    QStringList sourceImgFilters;
    sourceImgFilters << theme + QLatin1String(".jpg") << theme + QLatin1String(".png") << theme + QLatin1String(".jpeg");
    QStringList sourceImg = themeDir.entryList( sourceImgFilters );
    if( !sourceImg.isEmpty() ) {
        tarArgs.append( QString( "%1/%2/%3" ).arg( body ).arg( theme, sourceImg[0] ) );
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
    return QString( "%1/%2.tar.gz" ).arg( QDir::tempPath(), theme );
}

void MapWizard::deleteArchive( const QString& mapId )
{
    QStringList splitMapId( mapId.split(QLatin1Char('/')) );
    QString theme = splitMapId[1];
    QFile::remove( QString( "%1/%2.tar.gz" ).arg( QDir::tempPath(), theme ) );
}

bool MapWizard::validateCurrentPage()
{
    if ( currentId() == WmsSelectionPage && !d->m_serverCapabilitiesValid ) {
        d->uiWidget.progressBarWmsCapabilities->setVisible(true);
        d->owsManager.queryWmsCapabilities(QUrl(d->uiWidget.lineEditWmsUrl->text()));
        button( MapWizard::NextButton )->setEnabled( false );
        return false;
    }

    if ( (currentId() == LayerSelectionPage && !d->m_previewImageValid)
      || (currentId() == XYZUrlPage && !d->m_previewImageValid ) ){
        if( d->mapProviderType == MapWizardPrivate::WmsMap && d->uiWidget.listViewWmsLayers->currentIndex().isValid())
        {
            QString selected = d->sortModel->data(d->uiWidget.listViewWmsLayers->currentIndex(), layerIdRole).toString();
            QString projection = d->uiWidget.comboBoxWmsMaps->currentText() == tr("Equirectangular (epsg:4326)")
                    ? "epsg:4326" : "epsg:3857";
            QString format = d->uiWidget.comboBoxWmsFormat->currentText();
            if (format == QLatin1String("jpeg")) {
                format = "jpg";
            }
            QString style = d->owsManager.wmsCapabilities().style(selected);
            setLayerButtonsVisible(false);
            d->owsManager.queryWmsPreviewImage(QUrl(d->uiWidget.lineEditWmsUrl->text()),
                        selected, projection, format, style);
            button( MapWizard::NextButton )->setEnabled( false );
        }
        else if( d->mapProviderType == MapWizardPrivate::StaticUrlMap )
        {
            QString urlString = d->uiWidget.comboBoxStaticUrlServer->currentText();
            d->owsManager.queryXYZPreviewImage(urlString);
            d->staticUrlServerList.removeAll( urlString );
            d->staticUrlServerList.prepend( urlString );
            // Reset the Theme Description page
            d->uiWidget.lineEditTitle->clear();
            d->uiWidget.lineEditTheme->clear();
            d->uiWidget.textEditDesc->clear();
            d->uiWidget.labelPreview->clear();
            d->uiWidget.lineEditTitle->setFocus();
            button( MapWizard::NextButton )->setEnabled( false );
        }
        return false;
    }

    if ( currentId() == GlobalSourceImagePage ) {
        d->sourceImage = d->uiWidget.lineEditSource->text();
        if ( d->sourceImage.isEmpty() ) {
            QMessageBox::information( this,
                                      tr( "Source Image" ),
                                      tr( "Please specify a source image." ) );
            d->uiWidget.lineEditSource->setFocus();
            return false;
        }

        if ( !QFileInfo::exists(d->sourceImage) ) {
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
        // Reset the Theme Description page
        d->uiWidget.lineEditTitle->clear();
        d->uiWidget.lineEditTheme->clear();
        d->uiWidget.textEditDesc->clear();
        d->uiWidget.labelPreview->clear();
        d->previewImage = QImage( d->sourceImage ).scaled( 100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );;
        QPixmap previewPixmap = QPixmap::fromImage( d->previewImage );
        d->uiWidget.labelPreview->setPixmap( previewPixmap );
        d->uiWidget.labelPreview->resize( QSize(100, 100) );
        d->uiWidget.labelThumbnail->setPixmap( previewPixmap );
        d->uiWidget.labelThumbnail->resize( QSize(100, 100) );
    }

    if ( currentId() == ThemeInfoPage ) {
        if( d->mapProviderType == MapWizardPrivate::WmsMap && !d->m_legendImageValid )
        {
            QString selected = d->sortModel->data(d->uiWidget.listViewWmsLayers->currentIndex(), layerIdRole).toString();
            QString legendUrl = d->owsManager.wmsCapabilities().legendUrl(selected);
            if (!legendUrl.isEmpty()) {
                d->m_legendImageValid = true;
                d->owsManager.queryWmsLegendImage(QUrl(legendUrl));
                button( MapWizard::NextButton )->setEnabled( false );
                return false;
            }
        }
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

        const QDir destinationDir( QString( "%1/maps/earth/%2" ).arg( MarbleDirs::localPath(), d->mapTheme ) );
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
    if (currentId() == LegendPage && !d->m_levelZeroTileValid
        && d->mapProviderType != MapWizardPrivate::StaticImageMap ) {
        if( d->mapProviderType == MapWizardPrivate::WmsMap )
        {
            QString selected = d->sortModel->data(d->uiWidget.listViewWmsLayers->currentIndex(), layerIdRole).toString();
            QString projection = d->uiWidget.comboBoxWmsMaps->currentText() == tr("Equirectangular (epsg:4326)")
                    ? "epsg:4326" : "epsg:3857";
            QString format = d->uiWidget.comboBoxWmsFormat->currentText();
            if (format == QLatin1String("jpeg")) {
                format = "jpg";
            }
            QString style = d->owsManager.wmsCapabilities().style(selected);
            d->owsManager.queryWmsLevelZeroTile(QUrl(d->uiWidget.lineEditWmsUrl->text()),
                        selected, projection, format, style);
        }
        else if( d->mapProviderType == MapWizardPrivate::StaticUrlMap )
        {
            QString urlString = d->uiWidget.comboBoxStaticUrlServer->currentText();
            d->owsManager.queryXYZLevelZeroTile(urlString);
        }
        button( MapWizard::NextButton )->setEnabled( false );
        return false;
    }

    return QWizard::validateCurrentPage();
}

int MapWizard::nextId() const
{
    switch( currentId() )
    {
    case WelcomePage:
        if( d->uiWidget.radioButtonWms->isChecked() ) {
            d->mapProviderType = MapWizardPrivate::WmsMap;
            return WmsSelectionPage;
        } else if( d->uiWidget.radioButtonBitmap->isChecked() ) {
            d->mapProviderType = MapWizardPrivate::StaticImageMap;
            return GlobalSourceImagePage;
        } else if( d->uiWidget.radioButtonStaticUrl->isChecked() ) {
            d->mapProviderType = MapWizardPrivate::StaticUrlMap;
            return XYZUrlPage;
        }
        break;

    case WmsSelectionPage: // WMS Servers
        return LayerSelectionPage;

    case LayerSelectionPage: // WMS Layers
        return ThemeInfoPage;

    case GlobalSourceImagePage: // Static Image
        return ThemeInfoPage;

    case ThemeInfoPage:
        return LegendPage;

    case LegendPage:
        return SummaryPage;

    case SummaryPage: // Finish
        return -1;

    default:
        break;
    }
    
    return currentId() + 1;
}

void MapWizard::cleanupPage(int id)
{
    if ( d->mapProviderType == MapWizardPrivate::StaticUrlMap ) {
        if (id == ThemeInfoPage) {
            d->levelZero.clear();
            d->preview.clear();
        }
    }
    QWizard::cleanupPage(id);
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
    icon->setPixmap( QString("%1-preview.png").arg(document->head()->theme()) );
    
    GeoSceneZoom *zoom = head->zoom();
    zoom->setMinimum( 900 );
    zoom->setMaximum( 3500 );
    zoom->setDiscrete( false );
    
    GeoSceneTileDataset *texture = new GeoSceneTileDataset( "map" );
    texture->setExpire( 31536000 );
    texture->setSourceDir(QLatin1String("earth/") + document->head()->theme());
    if( d->mapProviderType == MapWizardPrivate::WmsMap )
    {
        texture->setFileFormat( d->owsManager.resultFormat() );
        QString layer = d->sortModel->data(d->uiWidget.listViewWmsLayers->currentIndex(), layerIdRole).toString();
        QString style = d->owsManager.wmsCapabilities().style(layer);
        QUrl downloadUrl = QUrl( d->uiWidget.lineEditWmsUrl->text() );
        QUrlQuery urlQuery;
        urlQuery.addQueryItem( "layers", layer );
        urlQuery.addQueryItem( "style", style );
        downloadUrl.setQuery( urlQuery );
        texture->addDownloadUrl( downloadUrl );
        texture->setMaximumTileLevel( 20 );
        texture->setTileSize(QSize(256, 256));
        texture->setLevelZeroRows( 1 );
        texture->setLevelZeroColumns( 1 );
        texture->setServerLayout( new WmsServerLayout( texture ) );
        if (d->uiWidget.comboBoxWmsMaps->currentText() == tr("Web Mercator (epsg:3857)")) {
            texture->setTileProjection(GeoSceneAbstractTileProjection::Mercator);
        }
        else {
            texture->setTileProjection(GeoSceneAbstractTileProjection::Equirectangular);
        }
    }
    
    else if( d->mapProviderType == MapWizardPrivate::StaticUrlMap )
    {
        texture->setFileFormat( d->format );
        QUrl downloadUrl = QUrl( d->uiWidget.comboBoxStaticUrlServer->currentText() );
        texture->addDownloadPolicy( DownloadBrowse, 20 );
        texture->addDownloadPolicy( DownloadBulk, 2 );
        texture->addDownloadUrl( downloadUrl );
        texture->setMaximumTileLevel( 20 );
        texture->setTileSize(QSize(256, 256));
        texture->setLevelZeroRows( 1 );
        texture->setLevelZeroColumns( 1 );
        texture->setServerLayout( new CustomServerLayout( texture ) );
        texture->setTileProjection(GeoSceneAbstractTileProjection::Mercator);
    }
    
    else if( d->mapProviderType == MapWizardPrivate::StaticImageMap )
    {
        QString image = d->uiWidget.lineEditSource->text();
        d->format = image.right(image.length() - image.lastIndexOf(QLatin1Char('.')) - 1).toLower();
        texture->setFileFormat( d->format.toUpper() );
        texture->setInstallMap(document->head()->theme() + QLatin1Char('.') + d->format);
        texture->setServerLayout( new MarbleServerLayout( texture ) );
        texture->setTileProjection(GeoSceneAbstractTileProjection::Equirectangular);
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
//    GeoSceneLegend *legend = document->legend();

 /*
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
   */
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
        Q_ASSERT( !d->owsManager.wmsCapabilities().layers().isEmpty() );
        Q_ASSERT( !d->levelZero.isNull() );
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
            if( !d->owsManager.wmsCapabilities().legendUrl(d->sortModel->data(d->uiWidget.listViewWmsLayers->currentIndex(), layerIdRole).toString()).isEmpty() )
            {
                createWmsLegend();
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
        QTimer::singleShot( 0, this, SLOT(restart()) );
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
            if( !d->owsManager.wmsCapabilities().legendUrl(d->sortModel->data(d->uiWidget.listViewWmsLayers->currentIndex(), layerIdRole).toString()).isEmpty() )
            {
                createWmsLegend();
            }
        } else if( d->mapProviderType == MapWizardPrivate::StaticImageMap || d->mapProviderType == MapWizardPrivate::StaticUrlMap ) {
            createLegend();
        }
    }
    
    QPointer<PreviewDialog> previewDialog = new PreviewDialog( this, document.data()->head()->mapThemeId() );
    previewDialog->exec();
    delete previewDialog;
}

void MapWizard::updateSearchFilter(const QString &text)
{
    d->sortModel->setFilterFixedString(text);
}


}

#include "moc_MapWizard.cpp"
#include "MapWizard.moc" // needed for Q_OBJECT here in source
