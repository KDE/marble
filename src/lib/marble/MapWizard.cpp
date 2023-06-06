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
#include <QColorDialog>

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
    QStringList wmtsServerList;
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
        WmtsMap,
        StaticUrlMap
    };
    QStringList selectedLayers;
    QString selectedProjection;

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

    connect( &d->owsManager, &OwsServiceManager::wmtsCapabilitiesReady,
             this, &MapWizard::processCapabilitiesResults);


    connect( d->uiWidget.pushButtonSource, &QAbstractButton::clicked, this, &MapWizard::querySourceImage );
    connect( d->uiWidget.pushButtonPreview, &QAbstractButton::clicked, this, &MapWizard::queryPreviewImage );
    connect( d->uiWidget.pushButtonLegend_2, &QAbstractButton::clicked, this, &MapWizard::queryLegendImage );

    connect( d->uiWidget.comboBoxWmsServer, SIGNAL(activated(QString)), this, SLOT(setLineEditWms(QString)) );
    connect( d->uiWidget.listViewWmsLayers, &QListView::pressed, this, &MapWizard::processSelectedLayerInformation );

    connect( d->uiWidget.lineEditTitle, &QLineEdit::textChanged, d->uiWidget.labelSumMName, &QLabel::setText );
    connect( d->uiWidget.lineEditTheme, &QLineEdit::textChanged, d->uiWidget.labelSumMTheme, &QLabel::setText );
    
    connect( d->uiWidget.pushButtonPreviewMap, &QAbstractButton::clicked, this, &MapWizard::showPreview );
    connect( d->uiWidget.lineEditWmsSearch, &QLineEdit::textChanged, this, &MapWizard::updateSearchFilter );
    connect( d->uiWidget.comboBoxWmsMaps, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBackdropCheckBox()) );
    connect( d->uiWidget.checkBoxWmsBackdrop, &QCheckBox::stateChanged, this, &MapWizard::updateBackdropCheckBox );

    connect( d->uiWidget.checkBoxWmsMultipleSelections, &QCheckBox::stateChanged, this, &MapWizard::updateListViewSelection);
    connect( d->uiWidget.pushButtonColor, &QPushButton::clicked, this, &MapWizard::chooseBackgroundColor);
    updateListViewSelection();

    d->uiWidget.checkBoxWmsMultipleSelections->setVisible(true);
    d->uiWidget.checkBoxWmsBackdrop->setVisible(true);
    d->uiWidget.listViewWmsLayers->setModel(d->sortModel);
    d->uiWidget.listViewWmsLayers->setSelectionMode(QAbstractItemView::ExtendedSelection);

    d->uiWidget.progressBarWmsCapabilities->setVisible(false);
    setLayerButtonsVisible(true);

    d->uiWidget.tabWidgetLayers->setCurrentIndex(0);

    QPalette p = d->uiWidget.labelBackgroundColor->palette();
    p.setColor(QPalette::Window, d->uiWidget.labelBackgroundColor->text());
    d->uiWidget.labelBackgroundColor->setAutoFillBackground(true);
    d->uiWidget.labelBackgroundColor->setPalette(p);

    d->uiWidget.radioButtonXYZServer->setVisible(false);
}

MapWizard::~MapWizard()
{
    delete d;
}

void MapWizard::processCapabilitiesResults()
{
    d->uiWidget.progressBarWmsCapabilities->setVisible(false);

    button( MapWizard::NextButton )->setEnabled( true );

    if (d->owsManager.capabilitiesStatus() == OwsCapabilitiesReplyUnreadable)
    {
        QMessageBox::critical( this, tr( "Error while parsing" ), tr( "Wizard cannot parse server's response" ) );
        return;
    }
    if(d->owsManager.capabilitiesStatus() == OwsCapabilitiesNoOwsServer)
    {
        QMessageBox::critical( this, tr( "Error while parsing" ), tr( "Server is not an OWS Server." ) );
        return;
    }    
    d->model->clear();

    OwsMappingCapabilities owsCapabilities;
    if (d->owsManager.owsServiceType() == WmsType) {
        owsCapabilities = d->owsManager.wmsCapabilities();
    }
    else if (d->owsManager.owsServiceType() == WmtsType) {
        owsCapabilities = d->owsManager.wmtsCapabilities();
    }

    d->uiWidget.labelWmsTitle->setText(QString("Web Service: <b>%1</b>").arg(owsCapabilities.title() ) );
    d->uiWidget.labelWmsTitle->setToolTip(QString("<small>%1</small>").arg(owsCapabilities.abstract() ) );

    for (auto layer : owsCapabilities.layers()) {
        if (!layer.isEmpty()) {
                QStandardItem * item = new QStandardItem(owsCapabilities.title(layer));
                item->setData(layer, layerIdRole);
                item->setToolTip(owsCapabilities.abstract(layer));
                d->model->appendRow(item);
        }
    }

    // default to the first layer
    d->uiWidget.listViewWmsLayers->setCurrentIndex( d->sortModel->index(0,0) );
    d->uiWidget.lineEditWmsSearch->setText(QString());


    d->uiWidget.comboBoxWmsFormat->clear();
    if (d->owsManager.owsServiceType() == WmsType) {
        d->uiWidget.comboBoxWmsFormat->addItems(d->owsManager.wmsCapabilities().formats());

        // default to png or jpeg
        d->uiWidget.comboBoxWmsFormat->setCurrentText("png");
        if (d->uiWidget.comboBoxWmsFormat->currentText() != "png") {
            d->uiWidget.comboBoxWmsFormat->setCurrentText("jpeg");
        }
    }

    QString serviceInfo;
    serviceInfo += "<html>";
    serviceInfo += d->owsManager.wmsCapabilities().abstract();
    serviceInfo += QString("<br><br><i>Contact:</i> %1").arg( d->owsManager.wmsCapabilities().contactInformation());
    serviceInfo += QString("<br><br><i>Fees:</i> %1").arg( d->owsManager.wmsCapabilities().fees());
    serviceInfo += "</html>";

    d->uiWidget.textEditWmsServiceInfo->setText(serviceInfo);
    d->uiWidget.tabServiceInfo->setEnabled(!serviceInfo.isEmpty());

    if (d->uiWidget.listViewWmsLayers->model()->rowCount() > 0) {
        processSelectedLayerInformation();
    }

    d->m_serverCapabilitiesValid = true;

    if (d->owsManager.owsServiceType() == WmsType) {
        if (!d->wmsServerList.contains(d->uiWidget.lineEditWmsUrl->text())) {
            d->wmsServerList.append( d->uiWidget.lineEditWmsUrl->text() );
        }
        setWmsServers( d->wmsServerList );
    }
    else if (d->owsManager.owsServiceType() == WmtsType) {
        if (!d->wmtsServerList.contains(d->uiWidget.lineEditWmsUrl->text())) {
            d->wmtsServerList.append( d->uiWidget.lineEditWmsUrl->text() );
        }
        setWmtsServers( d->wmtsServerList );
    }


    next();

    setSearchFieldVisible(d->model->rowCount() > 20);
}

void MapWizard::processSelectedLayerInformation()
{
    updateListViewSelection();

    QStringList selectedList;
    QModelIndexList selectedIndexes = d->uiWidget.listViewWmsLayers->selectionModel()->selectedIndexes();
    for (auto selectedIndex : selectedIndexes) {
        selectedList << d->sortModel->data(selectedIndex, layerIdRole).toString();
    }
    d->selectedLayers = selectedList;
    OwsMappingCapabilities owsCapabilities;
    if (d->owsManager.owsServiceType() == WmsType) {
        owsCapabilities = d->owsManager.wmsCapabilities();
    }
    else if (d->owsManager.owsServiceType() == WmtsType) {
        owsCapabilities = d->owsManager.wmtsCapabilities();
    }
    d->uiWidget.comboBoxWmsMaps->clear();
    QMap<QString, QString> epsgToText;
    epsgToText["epsg:3857"] = tr("Web Mercator (epsg:3857)");
    epsgToText["epsg:4326"] = tr("Equirectangular (epsg:4326)");
    epsgToText["crs:84"] = tr("Equirectangular (crs:84)");
    QStringList projectionTextList;
    if (d->selectedLayers.isEmpty()) {
        return;
    }

    if (d->owsManager.owsServiceType() == WmsType) {
        WmsCapabilities capabilities = d->owsManager.wmsCapabilities();
        for (auto projection : capabilities.projections(d->selectedLayers.first())) {
            projectionTextList << epsgToText[projection];
        }
        d->uiWidget.labelWmsTileProjection->setText(tr("Tile Projection:"));
        d->uiWidget.comboBoxWmsMaps->addItems(projectionTextList);

        // default to Web Mercator
        d->uiWidget.comboBoxWmsMaps->setCurrentText(tr("Web Mercator (epsg:3857)"));

        updateBackdropCheckBox(); // align the backdrop checkbox state with the available/selected projection

        //    bool projectionSelectionVisible = d->uiWidget.comboBoxWmsMaps->count() > 0;
        //    d->uiWidget.comboBoxWmsMaps->setVisible(projectionSelectionVisible);
        //    d->uiWidget.labelWmsTileProjection->setVisible(projectionSelectionVisible);
        //    d->uiWidget.comboBoxWmsMaps->setEnabled(d->uiWidget.comboBoxWmsMaps->count() > 1);
    }
    if (d->owsManager.owsServiceType() == WmtsType) {
        WmtsCapabilities capabilities = d->owsManager.wmtsCapabilities();
        QString selectedLayer = d->selectedLayers.first();
        d->uiWidget.labelWmsTileProjection->setText(tr("Tile Matrix Set:"));
        d->uiWidget.comboBoxWmsMaps->addItems(capabilities.wmtsTileMatrixSets()[selectedLayer]);
        d->uiWidget.comboBoxWmsFormat->addItems(capabilities.wmtsTileResource()[selectedLayer].keys());
        // default to png or jpeg
        d->uiWidget.comboBoxWmsFormat->setCurrentText("png");
        if (d->uiWidget.comboBoxWmsFormat->currentText() != "png") {
            d->uiWidget.comboBoxWmsFormat->setCurrentText("jpeg");
        }
    }


    d->uiWidget.lineEditTitle->setText( owsCapabilities.title(d->selectedLayers.first()) );

    // Remove all invalid characters from the theme-String
    // that will make the TileLoader malfunction.
    QString themeString = d->selectedLayers.first();
    themeString.remove(QRegExp("[:\"\\\\/]"));
    d->uiWidget.lineEditTheme->setText( themeString );
    QRegularExpression rx("^[^:\"\\\\/]*$");
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    d->uiWidget.lineEditTheme->setValidator(validator);

    QString description;
    description += "<html>";
    description += owsCapabilities.abstract(d->selectedLayers.first());
    if (d->owsManager.owsServiceType() == WmsType) {
        description += QString("<br><br><i>Contact:</i> %1").arg( d->owsManager.wmsCapabilities().contactInformation());
        description += QString("<br><br><i>Fees:</i> %1").arg( d->owsManager.wmsCapabilities().fees());
    }
    description += "</html>";
    d->uiWidget.textEditDesc->setText(description);

    QString layerInfo;
    layerInfo += owsCapabilities.abstract(d->selectedLayers.first());
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
}

QStringList MapWizard::wmtsServers() const
{
    return d->wmtsServerList;
}

void MapWizard::setWmtsServers(const QStringList &uris)
{
    d->wmtsServerList = uris;
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
    d->uiWidget.checkBoxWmsMultipleSelections->setVisible(visible);
    d->uiWidget.checkBoxWmsBackdrop->setVisible(visible);
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

        else if( d->mapProviderType == MapWizardPrivate::WmsMap || d->mapProviderType == MapWizardPrivate::WmtsMap )
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
    if (currentId() == WelcomePage) {
        updateOwsServiceType();
        return true;
    }
    if ( currentId() == WmsSelectionPage && !d->m_serverCapabilitiesValid ) {
        d->uiWidget.progressBarWmsCapabilities->setVisible(true);
        QString serviceString = d->uiWidget.radioButtonWms->isChecked()
                ? "WMS" : d->uiWidget.radioButtonWmts->isChecked() ? "WMTS" : "";
        d->owsManager.queryOwsCapabilities(QUrl(d->uiWidget.lineEditWmsUrl->text()), serviceString);
        button( MapWizard::NextButton )->setEnabled( false );
        return false;
    }

    if ( (currentId() == LayerSelectionPage && !d->m_previewImageValid)
      || (currentId() == XYZUrlPage && !d->m_previewImageValid ) ){
        if( d->mapProviderType == MapWizardPrivate::WmsMap && d->uiWidget.listViewWmsLayers->currentIndex().isValid())
        {
            QStringList selectedList;
            QModelIndexList selectedIndexes = d->uiWidget.listViewWmsLayers->selectionModel()->selectedIndexes();
            for (auto selectedIndex : selectedIndexes) {
                selectedList << d->sortModel->data(selectedIndex, layerIdRole).toString();
            }
            d->selectedLayers = selectedList;

            QString projection;
            if (d->uiWidget.comboBoxWmsMaps->currentText() == tr("Equirectangular (epsg:4326)"))
                projection = "epsg:4326";
            else if (d->uiWidget.comboBoxWmsMaps->currentText() == tr("Equirectangular (crs:84)"))
                projection = "crs:84";
            else
                projection = "epsg:3857";
            d->selectedProjection = projection;
            QString format = d->uiWidget.comboBoxWmsFormat->currentText();
            QStringList styles = d->owsManager.wmsCapabilities().styles(d->selectedLayers);
            d->owsManager.queryWmsPreviewImage(QUrl(d->uiWidget.lineEditWmsUrl->text()),
                        d->selectedLayers.join(','), projection, format, styles.join(','));
            setLayerButtonsVisible(false);
            button( MapWizard::NextButton )->setEnabled( false );
        }
        else if( d->mapProviderType == MapWizardPrivate::WmtsMap && d->uiWidget.listViewWmsLayers->currentIndex().isValid())
        {
            QStringList selectedList;
            QModelIndexList selectedIndexes = d->uiWidget.listViewWmsLayers->selectionModel()->selectedIndexes();
            for (auto selectedIndex : selectedIndexes) {
                selectedList << d->sortModel->data(selectedIndex, layerIdRole).toString();
            }
            d->selectedLayers = selectedList;

            QString tileMatrixSet = d->uiWidget.comboBoxWmsMaps->currentText();
            QString tileFormat = d->uiWidget.comboBoxWmsFormat->currentText();
            QString url = d->owsManager.wmtsCapabilities().wmtsTileResource()[d->selectedLayers.first()][tileFormat];
            QString style = d->owsManager.wmtsCapabilities().style(d->selectedLayers.first());
            d->owsManager.queryWmtsPreviewImage(url, style, tileMatrixSet);
            setLayerButtonsVisible(false);
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
            QString legendUrl = d->owsManager.wmsCapabilities().legendUrl(d->selectedLayers.first());
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
            QString projection;
            if (d->uiWidget.comboBoxWmsMaps->currentText() == tr("Equirectangular (epsg:4326)"))
                projection = "epsg:4326";
            else if (d->uiWidget.comboBoxWmsMaps->currentText() == tr("Equirectangular (crs:84)"))
                projection = "crs:84";
            else
                projection = "epsg:3857";
            d->selectedProjection = projection;
            QString format = d->uiWidget.comboBoxWmsFormat->currentText();
            QStringList styles = d->owsManager.wmsCapabilities().styles(d->selectedLayers);
            d->owsManager.queryWmsLevelZeroTile(QUrl(d->uiWidget.lineEditWmsUrl->text()),
                        d->selectedLayers.first(), projection, format, styles.first());
        }
        else if( d->mapProviderType == MapWizardPrivate::WmtsMap)
        {
            QStringList selectedList;
            QModelIndexList selectedIndexes = d->uiWidget.listViewWmsLayers->selectionModel()->selectedIndexes();
            for (auto selectedIndex : selectedIndexes) {
                selectedList << d->sortModel->data(selectedIndex, layerIdRole).toString();
            }
            d->selectedLayers = selectedList;

            QString tileMatrixSet = d->uiWidget.comboBoxWmsMaps->currentText();
            QString tileFormat = d->uiWidget.comboBoxWmsFormat->currentText();
            QString url = d->owsManager.wmtsCapabilities().wmtsTileResource()[d->selectedLayers.first()][tileFormat];
            QString style = d->owsManager.wmtsCapabilities().style(d->selectedLayers.first());
            d->owsManager.queryWmtsLevelZeroTile(url, style, tileMatrixSet);
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
        }
        else if( d->uiWidget.radioButtonWmts->isChecked() ) {
            d->mapProviderType = MapWizardPrivate::WmtsMap;
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
    
    GeoSceneTileDataset *backdropTexture = nullptr;
    bool isBackdropTextureAvailable = d->uiWidget.checkBoxWmsBackdrop->isEnabled()
            && d->uiWidget.checkBoxWmsBackdrop->isChecked()
            && d->uiWidget.radioButtonOpenStreetMap->isChecked();
    if (isBackdropTextureAvailable) {
        if (d->uiWidget.radioButtonXYZServer) {
            backdropTexture = new GeoSceneTileDataset( "backdrop" );
            backdropTexture->setExpire( 31536000 );
            backdropTexture->setSourceDir(QLatin1String("earth/openstreetmap"));
            backdropTexture->setFileFormat( "PNG" );
            backdropTexture->addDownloadPolicy( DownloadBrowse, 20 );
            backdropTexture->addDownloadPolicy( DownloadBulk, 2 );
            backdropTexture->addDownloadUrl( QUrl("https://tile.openstreetmap.org/") );
            backdropTexture->setMaximumTileLevel( 20 );
            backdropTexture->setTileSize(QSize(256, 256));
            backdropTexture->setLevelZeroRows( 1 );
            backdropTexture->setLevelZeroColumns( 1 );
            backdropTexture->setServerLayout( new OsmServerLayout( backdropTexture ) );
            backdropTexture->setTileProjection(GeoSceneAbstractTileProjection::Mercator);
        }
    }

    GeoSceneTileDataset *texture = new GeoSceneTileDataset( "map" );
    texture->setExpire( 31536000 );
    texture->setSourceDir(QLatin1String("earth/") + document->head()->theme());
    if( d->mapProviderType == MapWizardPrivate::WmsMap )
    {
        texture->setFileFormat( d->owsManager.resultFormat() );
        QStringList styles = d->owsManager.wmsCapabilities().styles(d->selectedLayers);
        QUrl downloadUrl = QUrl( d->uiWidget.lineEditWmsUrl->text() );
        QUrlQuery urlQuery;
        urlQuery.addQueryItem( "layers", d->selectedLayers.join(',') );
        urlQuery.addQueryItem( "styles", styles.join(",") );
        bool isBackdropAvailable = d->uiWidget.checkBoxWmsBackdrop->isEnabled()
                && d->uiWidget.checkBoxWmsBackdrop->isChecked();
        urlQuery.addQueryItem( "transparent", isBackdropTextureAvailable ? "true" : "false" );

        if (d->uiWidget.checkBoxWmsBackdrop->isChecked() && d->uiWidget.radioButtonColor->isChecked()) {
            QString bgColorName = d->uiWidget.labelBackgroundColor->palette().color(QPalette::Window).name();
            bgColorName = bgColorName.remove("#");
            bgColorName = "0x" + bgColorName;
            urlQuery.addQueryItem( "bgcolor", bgColorName);
        }
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
        if (isBackdropAvailable) {
            texture->setBlending("AlphaBlending");
        }
    }
    if( d->mapProviderType == MapWizardPrivate::WmtsMap )
    {
        QString format = d->uiWidget.comboBoxWmsFormat->currentText();
        texture->setFileFormat( format );
        QString selectedLayer = d->selectedLayers.first();
        QString urlString = d->owsManager.wmtsCapabilities().wmtsTileResource()[selectedLayer][format];
        urlString.replace(urlString.indexOf(QLatin1String("{Time}")), 6, "current");
        urlString.replace(urlString.indexOf(QLatin1String("{style}")), 7, d->owsManager.wmtsCapabilities().style(selectedLayer));
        urlString.replace(urlString.indexOf(QLatin1String("{Style}")), 7, d->owsManager.wmtsCapabilities().style(selectedLayer));
        urlString.replace(urlString.indexOf(QLatin1String("{TileMatrixSet}")), 15,  d->uiWidget.comboBoxWmsMaps->currentText());
        QUrl downloadUrl = QUrl( urlString );
        texture->addDownloadPolicy( DownloadBrowse, 20 );
        texture->addDownloadPolicy( DownloadBulk, 2 );
        texture->addDownloadUrl( downloadUrl );
        texture->setMaximumTileLevel( 20 );
        texture->setTileSize(QSize(256, 256));
        texture->setLevelZeroRows( 1 );
        texture->setLevelZeroColumns( 1 );
        texture->setServerLayout( new WmtsServerLayout( texture ) );
        texture->setTileProjection(GeoSceneAbstractTileProjection::Mercator);
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
    layer->addDataset( backdropTexture );
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

    if( d->mapProviderType == MapWizardPrivate::WmsMap )
    {
        QString bbox;
        bbox = d->owsManager.wmsCapabilities().boundingBoxNSEWDegrees(d->selectedLayers, d->selectedProjection);
        QStringList bboxList = bbox.split(',');
        // Only center if the bbox does not cover roughly the whole earth
        if (bboxList.at(0).toDouble() < 85 && bboxList.at(1).toDouble() > -85
         && bboxList.at(2).toDouble() < 179 && bboxList.at(3).toDouble() > -179) {
            map->setCenter(bbox);
        }
    }

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
    else if ( d->mapProviderType == MapWizardPrivate::WmtsMap )
    {
        Q_ASSERT( !d->owsManager.wmtsCapabilities().layers().isEmpty() );
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

void MapWizard::updateListViewSelection()
{
    QAbstractItemView::SelectionMode selectionModeWMS =
            d->uiWidget.checkBoxWmsMultipleSelections->isChecked()
            ? QAbstractItemView::MultiSelection
            : QAbstractItemView::ExtendedSelection;
    QAbstractItemView::SelectionMode selectionMode =
            d->owsManager.owsServiceType() == WmtsType
            ? QAbstractItemView::SingleSelection
            : selectionModeWMS;
    d->uiWidget.listViewWmsLayers->setSelectionMode(selectionMode);
    d->uiWidget.checkBoxWmsMultipleSelections->setVisible(d->uiWidget.radioButtonWms->isChecked());
}

void MapWizard::updateBackdropCheckBox()
{
    // The only backdrop supported is the Mercator-based OSM tile server map
    bool isMercator = d->uiWidget.comboBoxWmsMaps->currentText() == "Web Mercator (epsg:3857)";
    d->uiWidget.checkBoxWmsBackdrop->setEnabled(isMercator);
    d->uiWidget.tabCustomizeBackdrop->setEnabled(isMercator && d->uiWidget.checkBoxWmsBackdrop->isChecked());
}

void MapWizard::updateOwsServiceType()
{
    if (d->uiWidget.radioButtonWms->isChecked()) {
        d->uiWidget.labelWmsServer->setText(tr("WMS Server"));
        d->uiWidget.labelOwsServiceHeader->setText(tr("<h4>WMS Server</h4>Please choose a <a href=\"https://en.wikipedia.org/wiki/Web_Map_Service\">WMS</a> server or enter a custom server URL."));
        d->uiWidget.comboBoxWmsServer->clear();
        d->uiWidget.comboBoxWmsServer->addItems( d->wmsServerList );
        d->uiWidget.comboBoxWmsServer->addItem( tr( "Custom" ), "http://" );
        d->uiWidget.comboBoxWmsServer->setCurrentText( tr( "Custom" ) );

    }
    else if (d->uiWidget.radioButtonWmts->isChecked()) {
        d->uiWidget.labelWmsServer->setText(tr("WMTS Server"));
        d->uiWidget.labelOwsServiceHeader->setText(tr("<h4>WMTS Server</h4>Please choose a <a href=\"https://de.wikipedia.org/wiki/Web_Map_Tile_Service\">WMTS</a> server or enter a custom server URL."));
        d->uiWidget.comboBoxWmsServer->clear();
        d->uiWidget.comboBoxWmsServer->addItems( d->wmtsServerList );
        d->uiWidget.comboBoxWmsServer->addItem( tr( "Custom" ), "http://" );
        d->uiWidget.comboBoxWmsServer->setCurrentText( tr( "Custom" ) );
    }
    else if (d->uiWidget.radioButtonStaticUrl->isChecked()) {
        d->uiWidget.comboBoxStaticUrlServer->clear();
        d->uiWidget.comboBoxStaticUrlServer->addItems( d->staticUrlServerList );
//        d->uiWidget.comboBoxWmsServer->addItem( tr( "Custom" ), "http://" );
//        d->uiWidget.comboBoxWmsServer->setCurrentText( tr( "Custom" ) );
    }
}

void MapWizard::chooseBackgroundColor()
{
    QColor selectedColor = QColorDialog::getColor(d->uiWidget.pushButtonColor->text());
    if (selectedColor.isValid()) {
        d->uiWidget.labelBackgroundColor->setText(selectedColor.name());
        QPalette p = d->uiWidget.labelBackgroundColor->palette();
        p.setColor(QPalette::Window, selectedColor);
        d->uiWidget.labelBackgroundColor->setPalette(p);
    }
}


}

#include "moc_MapWizard.cpp"
#include "MapWizard.moc" // needed for Q_OBJECT here in source
