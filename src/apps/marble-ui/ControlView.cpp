//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//


#include "ControlView.h"

#include <QCloseEvent>
#include <QLayout>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QPainter>
#include <QTextDocument>
#include <QPointer>
#include <QUrl>
#include <QDesktopServices>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QTimer>
#include <QFileInfo>
#include <QMessageBox>
#include <QMainWindow>
#include <QDockWidget>
#include <QShortcut>
#include <QToolBar>
#include <QMimeData>
#include <QPixmap>

#ifdef MARBLE_DBUS
#include <QDBusConnection>
#include "MarbleDBusInterface.h"
#endif

#include "GeoDataLatLonAltBox.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoUriParser.h"
#include "MarbleWidget.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "MarbleMap.h"
#include "MapThemeManager.h"
#include "PrintOptionsWidget.h"
#include "ViewportParams.h"
#include "ViewParams.h"
#include "routing/Route.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"
#include "routing/RouteRequest.h"
#include "routing/RoutingWidget.h"
#include "ExternalEditorDialog.h"
#include "CurrentLocationWidget.h"
#include "SearchWidget.h"
#include "TourWidget.h"
#include "MapViewWidget.h"
#include "FileViewWidget.h"
#include "LegendWidget.h"
#include "BookmarkManager.h"
#include "cloudsync/CloudSyncManager.h"
#include "cloudsync/BookmarkSyncManager.h"
#include "cloudsync/RouteSyncManager.h"
#include "cloudsync/ConflictDialog.h"
#include "cloudsync/MergeItem.h"
#include "RenderPlugin.h"

namespace Marble
{

ControlView::ControlView( QWidget *parent )
   : QWidget( parent ),
     m_mapThemeManager( new MapThemeManager( this ) ),
     m_searchDock( 0 ),
     m_locationWidget( 0 ),
     m_conflictDialog( 0 ),
     m_togglePanelVisibilityAction( 0 ),
     m_isPanelVisible( true ),
     m_tourWidget( 0 ),
     m_annotationDock( 0 ),
     m_annotationPlugin( 0 )
{
    setWindowTitle( tr( "Marble - Virtual Globe" ) );

    resize( 680, 640 );

    m_marbleWidget = new MarbleWidget( this );
    m_marbleWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding,
                                                QSizePolicy::MinimumExpanding ) );
#ifdef MARBLE_DBUS
    new MarbleDBusInterface( m_marbleWidget );
    QDBusConnection::sessionBus().registerObject( "/Marble", m_marbleWidget );
    if (!QDBusConnection::sessionBus().registerService( "org.kde.marble" )) {
        QString const urlWithPid = QString("org.kde.marble-%1").arg( QCoreApplication::applicationPid() );
        if ( !QDBusConnection::sessionBus().registerService( urlWithPid ) ) {
            mDebug() << "Failed to register service org.kde.marble and " << urlWithPid << " with the DBus session bus.";
        }
    }
#endif


    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget( m_marbleWidget );
    layout->setMargin( 0 );
    setLayout( layout );

    m_cloudSyncManager = new CloudSyncManager( this );
    m_cloudSyncManager->routeSyncManager()->setRoutingManager( m_marbleWidget->model()->routingManager() );
    BookmarkSyncManager* bookmarkSyncManager = m_cloudSyncManager->bookmarkSyncManager();
    bookmarkSyncManager->setBookmarkManager( m_marbleWidget->model()->bookmarkManager() );
    m_conflictDialog = new ConflictDialog( m_marbleWidget );
    connect( bookmarkSyncManager, SIGNAL(mergeConflict(MergeItem*)), this, SLOT(showConflictDialog(MergeItem*)) );
    connect( bookmarkSyncManager, SIGNAL(syncComplete()), m_conflictDialog, SLOT(stopAutoResolve()) );
    connect( m_conflictDialog, SIGNAL(resolveConflict(MergeItem*)), bookmarkSyncManager, SLOT(resolveConflict(MergeItem*)) );

    setAcceptDrops(true);
}

ControlView::~ControlView()
{
    // nothing to do
}

QString ControlView::applicationVersion()
{
    return "2.1.0 (stable release)";
}

MapThemeManager *ControlView::mapThemeManager()
{
    return m_mapThemeManager;
}

void ControlView::zoomIn()
{
    m_marbleWidget->zoomIn();
}

void ControlView::zoomOut()
{
    m_marbleWidget->zoomOut();
}

void ControlView::moveLeft()
{
    m_marbleWidget->moveLeft();
}

void ControlView::moveRight()
{
    m_marbleWidget->moveRight();
}

void ControlView::moveUp()
{
    m_marbleWidget->moveUp();
}

void ControlView::moveDown()
{
    m_marbleWidget->moveDown();
}

QString ControlView::defaultMapThemeId() const
{
    QStringList fallBackThemes;
      fallBackThemes << "earth/srtm/srtm.dgml";
      fallBackThemes << "earth/bluemarble/bluemarble.dgml";
      fallBackThemes << "earth/openstreetmap/openstreetmap.dgml";

    const QStringList installedThemes = m_mapThemeManager->mapThemeIds();

    foreach(const QString &fallback, fallBackThemes) {
        if (installedThemes.contains(fallback)) {
            return fallback;
        }
    }

    if (installedThemes.size()) {
        return installedThemes.first();
    }

    return QString();
}

void ControlView::printMapScreenShot( QPointer<QPrintDialog> printDialog)
{
#ifndef QT_NO_PRINTER
        PrintOptionsWidget* printOptions = new PrintOptionsWidget( this );
        bool const mapCoversViewport = m_marbleWidget->viewport()->mapCoversViewport();
        printOptions->setBackgroundControlsEnabled( !mapCoversViewport );
        bool hasLegend = m_marbleWidget->model()->legend() != 0;
        printOptions->setLegendControlsEnabled( hasLegend );
        bool hasRoute = marbleWidget()->model()->routingManager()->routingModel()->rowCount() > 0;
        printOptions->setPrintRouteSummary( hasRoute );
        printOptions->setPrintDrivingInstructions( hasRoute );
        printOptions->setPrintDrivingInstructionsAdvice( hasRoute );
        printOptions->setRouteControlsEnabled( hasRoute );
        printDialog->setOptionTabs( QList<QWidget*>() << printOptions );

        if ( printDialog->exec() == QDialog::Accepted ) {
            QTextDocument document;
            QString text = "<html><head><title>Marble Printout</title></head><body>";
            QPalette const originalPalette = m_marbleWidget->palette();
            bool const wasBackgroundVisible = m_marbleWidget->showBackground();
            bool const hideBackground = !mapCoversViewport && !printOptions->printBackground();
            if ( hideBackground ) {
                // Temporarily remove the black background and layers painting on it
                m_marbleWidget->setShowBackground( false );
                m_marbleWidget->setPalette( QPalette ( Qt::white ) );
                m_marbleWidget->update();
            }

            if ( printOptions->printMap() ) {
                printMap( document, text, printDialog->printer() );
            }

            if ( printOptions->printLegend() ) {
                printLegend( document, text );
            }

            if ( printOptions->printRouteSummary() ) {
                printRouteSummary( document, text );
            }

            if ( printOptions->printDrivingInstructions() ) {
                printDrivingInstructions( document, text );
            }

            if ( printOptions->printDrivingInstructionsAdvice() ) {
                printDrivingInstructionsAdvice( document, text );
            }

            text += QLatin1String("</body></html>");
            document.setHtml( text );
            document.print( printDialog->printer() );

            if ( hideBackground ) {
                m_marbleWidget->setShowBackground( wasBackgroundVisible );
                m_marbleWidget->setPalette( originalPalette );
                m_marbleWidget->update();
            }
    }
#endif
}

bool ControlView::openGeoUri( const QString& geoUriString )
{
    GeoUriParser uriParser( geoUriString );
    const bool success = uriParser.parse();
    if ( success ) {
        if ( uriParser.planet().id() != marbleModel()->planet()->id() ) {
            MapThemeManager *manager = mapThemeManager();
            foreach( const QString& planetName, manager->mapThemeIds()) {
                if ( planetName.startsWith(uriParser.planet().id(), Qt::CaseInsensitive)) {
                    m_marbleWidget->setMapThemeId(planetName);
                    break;
                }
            }
        }
        m_marbleWidget->centerOn( uriParser.coordinates() );
        if ( uriParser.coordinates().altitude() > 0.0 )
        {
            m_marbleWidget->setDistance( uriParser.coordinates().altitude() * METER2KM );
        }
    }
    return success;
}

QActionGroup *ControlView::createViewSizeActionGroup( QObject* parent )
{
    QActionGroup* actionGroup = new QActionGroup( parent );

    QAction *defaultAction = new QAction( tr( "Default (Resizable)" ), parent );
    defaultAction->setCheckable( true );
    defaultAction->setChecked( true );
    actionGroup->addAction(defaultAction);

    QAction *separator = new QAction( parent );
    separator->setSeparator( true );
    actionGroup->addAction(separator);

    addViewSizeAction( actionGroup, tr("NTSC (%1x%2)"), 720, 486 );
    addViewSizeAction( actionGroup, tr("PAL (%1x%2)"), 720, 576 );
    addViewSizeAction( actionGroup, tr("NTSC 16:9 (%1x%2)"), 864, 486 );
    addViewSizeAction( actionGroup, tr("PAL 16:9 (%1x%2)"), 1024, 576 );
    // xgettext:no-c-format
    addViewSizeAction( actionGroup, tr("DVD (%1x%2p)"), 852, 480 );
    // xgettext:no-c-format
    addViewSizeAction( actionGroup, tr("HD (%1x%2p)"), 1280, 720 );
    // xgettext:no-c-format
    addViewSizeAction( actionGroup, tr("Full HD (%1x%2p)"), 1920, 1080 );
    addViewSizeAction( actionGroup, tr("Digital Cinema (%1x%2)"), 2048, 1536 );
    /** FIXME: Needs testing, worked with errors.
    addViewSizeAction(actionGroup, "4K UHD (%1x%2)", 3840, 2160);
    addViewSizeAction(actionGroup, "4K (%1x%2)", 4096, 3072);
    */

    return actionGroup;
}

void ControlView::printPixmap( QPrinter * printer, const QPixmap& pixmap  )
{
#ifndef QT_NO_PRINTER
    QSize printSize = pixmap.size();
    QRect mapPageRect = printer->pageRect();
    printSize.scale( printer->pageRect().size(), Qt::KeepAspectRatio );
    QPoint printTopLeft( ( mapPageRect.width() - printSize.width() ) / 2 ,
                         ( mapPageRect.height() - printSize.height() ) / 2 );
    QRect mapPrintRect( printTopLeft, printSize );

    QPainter painter;
    if (!painter.begin(printer))
        return;
    painter.drawPixmap( mapPrintRect, pixmap, pixmap.rect() );
    painter.end();
#endif
}

// QPointer is used because of issues described in http://www.kdedevelopers.org/node/3919
void ControlView::printPreview()
{
#ifndef QT_NO_PRINTER
    QPrinter printer( QPrinter::HighResolution );

    QPointer<QPrintPreviewDialog> preview = new QPrintPreviewDialog( &printer, this );
    preview->setWindowFlags ( Qt::Window );
    preview->resize(640, 480);
    connect( preview, SIGNAL(paintRequested(QPrinter*)), SLOT(paintPrintPreview(QPrinter*)) );
    preview->exec();
    delete preview;
#endif
}

void ControlView::paintPrintPreview( QPrinter * printer )
{
#ifndef QT_NO_PRINTER
    QPixmap mapPixmap = mapScreenShot();
    printPixmap( printer, mapPixmap );
#endif
}

void ControlView::printMap( QTextDocument &document, QString &text, QPrinter *printer )
{
#ifndef QT_NO_PRINTER
    QPixmap image = mapScreenShot();

    if ( m_marbleWidget->viewport()->mapCoversViewport() ) {
        // Paint a black frame. Looks better.
        QPainter painter(&image);
        painter.setPen( Qt::black );
        painter.drawRect( 0, 0, image.width() - 2, image.height() - 2 );
    }

    QString uri = "marble://screenshot.png";
    document.addResource( QTextDocument::ImageResource, QUrl( uri ), QVariant( image) );
    QString img = "<img src=\"%1\" width=\"%2\" align=\"center\">";
    int width = qRound( printer->pageRect( QPrinter::Point ).width() );
    text += img.arg( uri ).arg( width );
#endif
}

void ControlView::printLegend( QTextDocument &document, QString &text )
{
#ifndef QT_NO_PRINTER
    QTextDocument *legend = m_marbleWidget->model()->legend();
    if ( legend ) {
        legend->adjustSize();
        QSize size = legend->size().toSize();
        QSize imageSize = size + QSize( 4, 4 );
        QImage image( imageSize, QImage::Format_ARGB32);
        QPainter painter( &image );
        painter.setRenderHint( QPainter::Antialiasing, true );
        painter.drawRoundedRect( QRect( QPoint( 0, 0 ), size ), 5, 5 );
        legend->drawContents( &painter );
        document.addResource( QTextDocument::ImageResource, QUrl( "marble://legend.png" ), QVariant(image) );
        QString img = "<p><img src=\"%1\" align=\"center\"></p>";
        text += img.arg( "marble://legend.png" );
    }
#endif
}

void ControlView::printRouteSummary( QTextDocument &document, QString &text)
{
#ifndef QT_NO_PRINTER
    RoutingModel* routingModel = m_marbleWidget->model()->routingManager()->routingModel();

    if ( !routingModel ) {
        return;
    }

    RouteRequest* routeRequest = m_marbleWidget->model()->routingManager()->routeRequest();
    if ( routeRequest ) {
        QString summary = "<h3>Route to %1: %2 %3</h3>";
        QString destination;
        if ( routeRequest->size() ) {
            destination = routeRequest->name( routeRequest->size()-1 );
        }

        QString label = "<p>%1 %2</p>";
        qreal distance = routingModel->route().distance();
        QString unit = distance > 1000 ? "km" : "m";
        int precision = distance > 1000 ? 1 : 0;
        if ( distance > 1000 ) {
            distance /= 1000;
        }
        summary = summary.arg(destination).arg( distance, 0, 'f', precision ).arg( unit );
        text += summary;

        text += QLatin1String("<table cellpadding=\"2\">");
        QString pixmapTemplate = "marble://viaPoint-%1.png";
        for ( int i=0; i<routeRequest->size(); ++i ) {
            text += QLatin1String("<tr><td>");
            QPixmap pixmap = routeRequest->pixmap(i);
            QString pixmapResource = pixmapTemplate.arg( i );
            document.addResource(QTextDocument::ImageResource,
                                          QUrl( pixmapResource ), QVariant( pixmap ) );
            QString myimg = "<img src=\"%1\">";
            text += myimg.arg(pixmapResource) +
                    QLatin1String("</td><td>");
                    routeRequest->name(i) +
                    QLatin1String("</td></tr>");
        }
        text += QLatin1String("</table>");
    }
#endif
}

void ControlView::printDrivingInstructions( QTextDocument &document, QString &text )
{
#ifndef QT_NO_PRINTER
    RoutingModel* routingModel = m_marbleWidget->model()->routingManager()->routingModel();

    if (!routingModel) {
        return;
    }

    GeoDataLineString total = routingModel->route().path();

    text += QLatin1String("<table cellpadding=\"4\">"
            "<tr><th>No.</th><th>Distance</th><th>Instruction</th></tr>");
    for ( int i=0; i<routingModel->rowCount(); ++i ) {
        QModelIndex index = routingModel->index(i, 0);
        GeoDataCoordinates coordinates = index.data( RoutingModel::CoordinateRole ).value<GeoDataCoordinates>();
        GeoDataLineString accumulator;
        for (int k=0; k<total.size(); ++k) {
            accumulator << total.at(k);

            if (total.at(k) == coordinates)
                break;
        }

        if ( i%2 == 0 ) {
            text += QLatin1String("<tr bgcolor=\"lightGray\"><td align=\"right\" valign=\"middle\">");
        }
        else {
            text += QLatin1String("<tr><td align=\"right\" valign=\"middle\">");
        }
        text += QString::number(i+1) +
                QLatin1String("</td><td align=\"right\" valign=\"middle\">");

        qreal planetRadius = marbleModel()->planet()->radius();
        text += QString::number(accumulator.length(planetRadius) * METER2KM, 'f', 1) +
                /** @todo: support localization */
                QLatin1String(" km</td><td valign=\"middle\">");

        QPixmap instructionIcon = index.data( Qt::DecorationRole ).value<QPixmap>();
        if ( !instructionIcon.isNull() ) {
            QString uri = QString("marble://turnIcon%1.png").arg(i);
            document.addResource( QTextDocument::ImageResource, QUrl( uri ), QVariant( instructionIcon ) );
            text += QString("<img src=\"%1\">").arg(uri);
        }

        text += routingModel->data( index ).toString() +
                QLatin1String("</td></tr>");
    }
    text += QLatin1String("</table>");
#endif
}

void ControlView::printDrivingInstructionsAdvice( QTextDocument &, QString &text )
{
#ifndef QT_NO_PRINTER
    text += QLatin1String("<p>") + tr("The Marble development team wishes you a pleasant and safe journey.") + QLatin1String("</p>") +
            QLatin1String("<p>") + tr("Caution: Driving instructions may be incomplete or inaccurate.") +
            QLatin1Char(' ') + tr("Road construction, weather and other unforeseen variables can result in this suggested route not to be the most expedient or safest route to your destination.") +
            QLatin1Char(' ') + tr("Please use common sense while navigating.") + QLatin1String("</p>");
#endif
}

void ControlView::addViewSizeAction( QActionGroup* actionGroup, const QString &nameTemplate, int width, int height )
{
    QString const name = nameTemplate.arg( width ).arg( height );
    QAction *action = new QAction( name, actionGroup->parent() );
    action->setCheckable( true );
    action->setData( QSize( width, height ) );
    actionGroup->addAction( action );
}


void ControlView::launchExternalMapEditor()
{
    QString editor = m_externalEditor;
    if ( editor.isEmpty() ) {
        QPointer<ExternalEditorDialog> dialog = new ExternalEditorDialog( this );
        if( dialog->exec() == QDialog::Accepted ) {
            editor = dialog->externalEditor();
            if ( dialog->saveDefault() ) {
                m_externalEditor = editor;
            }
        } else {
            return;
        }
    }

    if (editor == QLatin1String("josm")) {
        // JOSM, the java based editor
        synchronizeWithExternalMapEditor( editor, "--download=%1,%4,%3,%2" );
    }
    else if (editor == QLatin1String("merkaartor")) {
        // Merkaartor, a Qt based editor
        QString argument = "osm://download/load_and_zoom?top=%1&right=%2&bottom=%3&left=%4";
        synchronizeWithExternalMapEditor( editor, argument );
    }
    else {
        // Potlatch, the flash based editor running at the osm main website
        QString url = "http://www.openstreetmap.org/edit?lat=%1&lon=%2&zoom=%3";
        qreal lat = m_marbleWidget->centerLatitude();
        qreal lon = m_marbleWidget->centerLongitude();
        int zoom = m_marbleWidget->tileZoomLevel();
        url = url.arg( lat, 0, 'f', 8 ).arg( lon, 0, 'f', 8 ).arg( zoom );
        QDesktopServices::openUrl( QUrl(url) );
    }
}

void ControlView::synchronizeWithExternalMapEditor( const QString &application, const QString &argument )
{
    QTimer watchdog; // terminates network connection after a short timeout
    watchdog.setSingleShot( true );
    QEventLoop localEventLoop;
    connect( &watchdog, SIGNAL(timeout()), &localEventLoop, SLOT(quit()) );
    QNetworkAccessManager manager;
    connect( &manager, SIGNAL(finished(QNetworkReply*)), &localEventLoop, SLOT(quit()) );

    // Wait at most two seconds for the local server to respond
    QNetworkReply *reply = manager.get( QNetworkRequest( QUrl( "http://localhost:8111/") ) );
    watchdog.start( 2000 );
    localEventLoop.exec();

    GeoDataLatLonAltBox box = m_marbleWidget->viewport()->viewLatLonAltBox();
    qreal north = box.north( GeoDataCoordinates::Degree );
    qreal east  = box.east( GeoDataCoordinates::Degree );
    qreal south = box.south( GeoDataCoordinates::Degree );
    qreal west  = box.west( GeoDataCoordinates::Degree );

    if( watchdog.isActive() && reply->bytesAvailable() > 0 ) {
        // The local server is alive. Tell it to download the current region
        watchdog.stop();
        QString serverUrl = "http://localhost:8111/load_and_zoom?top=%1&right=%2&bottom=%3&left=%4";
        serverUrl = serverUrl.arg( north, 0, 'f', 8 ).arg( east, 0, 'f', 8 );
        serverUrl = serverUrl.arg( south, 0, 'f', 8 ).arg( west, 0, 'f', 8 );
        mDebug() << "Connecting to local server URL " << serverUrl;
        manager.get( QNetworkRequest( QUrl( serverUrl ) ) );

        // Give it five seconds to process the request
        watchdog.start( 5000 );
        localEventLoop.exec();
    } else {
        // The local server is not alive. Start the application
        QString applicationArgument = argument.arg( south, 0, 'f', 8 ).arg( east, 0, 'f', 8 );
        applicationArgument = applicationArgument.arg( north, 0, 'f', 8 ).arg( west, 0, 'f', 8 );
        mDebug() << "No local server found. Launching " << application << " with argument " << applicationArgument;
        if ( !QProcess::startDetached( application, QStringList() << applicationArgument ) ) {
            QString text = tr( "Unable to start the external editor. Check that %1 is installed or choose a different external editor in the settings dialog." );
            text = text.arg( application );
            QMessageBox::warning( this, tr( "Cannot start external editor" ), text );
        }
    }
}

void ControlView::setExternalMapEditor( const QString &editor )
{
    m_externalEditor = editor;
}

QList<QAction*> ControlView::setupDockWidgets( QMainWindow *mainWindow )
{
    Q_ASSERT( !m_searchDock && "Please create dock widgets just once" );

    mainWindow->setTabPosition( Qt::LeftDockWidgetArea, QTabWidget::North );
    mainWindow->setTabPosition( Qt::RightDockWidgetArea, QTabWidget::North );

    QDockWidget* legendDock = new QDockWidget( tr( "Legend" ), this );
    legendDock->setObjectName( "legendDock" );
    legendDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    LegendWidget* legendWidget = new LegendWidget( this );
    legendWidget->setMarbleModel( m_marbleWidget->model() );
    connect( legendWidget, SIGNAL(tourLinkClicked(QString)),
             this, SLOT(handleTourLinkClicked(QString)) );
    connect( legendWidget, SIGNAL(propertyValueChanged(QString,bool)),
             marbleWidget(), SLOT(setPropertyValue(QString,bool)) );
    legendDock->setWidget( legendWidget );

    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( smallScreen ) {
        // Show only the legend as a dock widget on small screen, the others are dialogs
        mainWindow->addDockWidget( Qt::LeftDockWidgetArea, legendDock );
        return QList<QAction*>() << legendDock->toggleViewAction();
    }

    QDockWidget *routingDock = new QDockWidget( tr( "Routing" ), mainWindow );
    routingDock->setObjectName( "routingDock" );
    routingDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    RoutingWidget* routingWidget = new RoutingWidget( marbleWidget(), mainWindow );
    routingWidget->setRouteSyncManager( cloudSyncManager()->routeSyncManager() );
    routingDock->setWidget( routingWidget );
    mainWindow->addDockWidget( Qt::LeftDockWidgetArea, routingDock );

    QDockWidget *locationDock = new QDockWidget( tr( "Location" ), this );
    locationDock->setObjectName( "locationDock" );
    locationDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    m_locationWidget = new CurrentLocationWidget( this );
    m_locationWidget->setMarbleWidget( marbleWidget() );
    locationDock->setWidget( m_locationWidget );
    mainWindow->addDockWidget( Qt::LeftDockWidgetArea, locationDock );

    m_searchDock = new QDockWidget( tr( "Search" ), this );
    m_searchDock->setObjectName( "searchDock" );
    m_searchDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    SearchWidget* searchWidget = new SearchWidget( this );
    searchWidget->setMarbleWidget( marbleWidget() );
    m_searchDock->setWidget( searchWidget );
    mainWindow->addDockWidget( Qt::LeftDockWidgetArea, m_searchDock );

    mainWindow->tabifyDockWidget( m_searchDock, routingDock );
    mainWindow->tabifyDockWidget( routingDock, locationDock );
    m_searchDock->raise();

    QKeySequence searchSequence( Qt::CTRL + Qt::Key_F );
    searchWidget->setToolTip( tr( "Search for cities, addresses, points of interest and more (%1)" ).arg( searchSequence.toString() ) );
    QShortcut* searchShortcut = new QShortcut( mainWindow );
    connect( searchShortcut, SIGNAL(activated()), this, SLOT(showSearch()) );

    QDockWidget *mapViewDock = new QDockWidget( tr( "Map View" ), this );
    mapViewDock->setObjectName( "mapViewDock" );
    mapViewDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    MapViewWidget* mapViewWidget = new MapViewWidget( this );
    mapViewWidget->setMarbleWidget( marbleWidget(), m_mapThemeManager );
    connect( mapViewWidget, SIGNAL(showMapWizard()), this, SIGNAL(showMapWizard()) );
    connect( mapViewWidget, SIGNAL(showUploadDialog()), this, SIGNAL(showUploadDialog()) );
    connect( mapViewWidget, SIGNAL(mapThemeDeleted()), this, SIGNAL(mapThemeDeleted()) );
    mapViewDock->setWidget( mapViewWidget );
    mainWindow->addDockWidget( Qt::LeftDockWidgetArea, mapViewDock );

    QDockWidget *fileViewDock = new QDockWidget( tr( "Files" ), this );
    fileViewDock->setObjectName( "fileViewDock" );
    fileViewDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    FileViewWidget* fileViewWidget = new FileViewWidget( this );
    fileViewWidget->setMarbleWidget( marbleWidget() );
    fileViewDock->setWidget( fileViewWidget );
    mainWindow->addDockWidget( Qt::LeftDockWidgetArea, fileViewDock );
    fileViewDock->hide();

    QDockWidget *tourDock = new QDockWidget( tr( "Tour" ), this );
    tourDock->setObjectName( "tourDock" );
    tourDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    m_tourWidget = new TourWidget( this );
    m_tourWidget->setMarbleWidget( marbleWidget() );
    tourDock->setWidget( m_tourWidget );
    mainWindow->addDockWidget( Qt::LeftDockWidgetArea, tourDock );
    tourDock->hide();

    mainWindow->addDockWidget( Qt::LeftDockWidgetArea, legendDock );
    mainWindow->tabifyDockWidget( mapViewDock, legendDock );
    mapViewDock->raise();

    m_annotationDock = new QDockWidget( QObject::tr( "Edit Maps" ) );
    m_annotationDock->setObjectName( "annotateDock" );
    m_annotationDock->hide();
    m_annotationDock->toggleViewAction()->setVisible( false );

    QList<RenderPlugin *> renderPluginList = marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = renderPluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = renderPluginList.constEnd();

    for (; i != end; ++i ) {
        if ((*i)->nameId() == QLatin1String("annotation")) {
            m_annotationPlugin = *i;
            QObject::connect(m_annotationPlugin, SIGNAL(enabledChanged(bool)),
                             this, SLOT(updateAnnotationDockVisibility()));
            QObject::connect(m_annotationPlugin, SIGNAL(visibilityChanged(bool,QString)),
                             this, SLOT(updateAnnotationDockVisibility()));
            QObject::connect(m_annotationPlugin, SIGNAL(actionGroupsChanged()),
                             this, SLOT(updateAnnotationDock()));
            updateAnnotationDock();
            updateAnnotationDockVisibility();
            mainWindow->addDockWidget( Qt::LeftDockWidgetArea, m_annotationDock );
        }
    }

    mainWindow->tabifyDockWidget( tourDock, m_annotationDock );
    mainWindow->tabifyDockWidget( m_annotationDock, fileViewDock );

    QList<QAction*> panelActions;
    panelActions << routingDock->toggleViewAction();
    panelActions << locationDock->toggleViewAction();
    panelActions << m_searchDock->toggleViewAction();
    panelActions << mapViewDock->toggleViewAction();
    panelActions << fileViewDock->toggleViewAction();
    panelActions << m_annotationDock->toggleViewAction();
    panelActions << legendDock->toggleViewAction();
    panelActions << tourDock->toggleViewAction();

    // Local list of panel view toggle actions
    m_panelActions << routingDock->toggleViewAction();
    m_panelActions << locationDock->toggleViewAction();
    m_panelActions << m_searchDock->toggleViewAction();
    m_panelActions << mapViewDock->toggleViewAction();
    m_panelActions << fileViewDock->toggleViewAction();
    m_panelActions << m_annotationDock->toggleViewAction();
    m_panelActions << legendDock->toggleViewAction();
    m_panelActions << tourDock->toggleViewAction();
    foreach( QAction* action, m_panelActions ) {
        m_panelVisibility << action->isVisible();
    }

    // Create Settings->Panels Menu
    // Toggle All Panels action
    m_togglePanelVisibilityAction = new QAction( tr("Hide &All Panels"), this);
    m_togglePanelVisibilityAction->setShortcut( Qt::Key_F9 );
    m_togglePanelVisibilityAction->setStatusTip(tr("Show or hide all panels."));
    connect(m_togglePanelVisibilityAction, SIGNAL(triggered()), this, SLOT(togglePanelVisibility()));

    // Include a Separator in the List
    QAction *panelSeparatorAct = new QAction( this );
    panelSeparatorAct->setSeparator( true );

    // Return a list of panel view actions for Marble Menu including show/hide all
    QList<QAction*> panelMenuActions;
    panelMenuActions << m_togglePanelVisibilityAction;
    panelMenuActions << panelSeparatorAct;
    foreach( QAction* action, m_panelActions ) {
        panelMenuActions << action;
    }

    return panelMenuActions;
}

CurrentLocationWidget *ControlView::currentLocationWidget()
{
    return m_locationWidget;
}

void ControlView::setWorkOffline( bool offline )
{
    marbleWidget()->model()->setWorkOffline( offline );
    if ( !offline ) {
        marbleWidget()->clearVolatileTileCache();
    }
}

CloudSyncManager *ControlView::cloudSyncManager()
{
    return m_cloudSyncManager;
}

QString ControlView::externalMapEditor() const
{
    return m_externalEditor;
}

void ControlView::addGeoDataFile( const QString &filename )
{
    QFileInfo const file( filename );
    if ( file.exists() ) {
        m_marbleWidget->model()->addGeoDataFile( file.absoluteFilePath() );
    } else {
        qWarning() << "File" << filename << "does not exist, cannot open it.";
    }
}

void ControlView::showSearch()
{
    if ( !m_searchDock ) {
        return;
    }

    m_searchDock->show();
    m_searchDock->raise();
    m_searchDock->widget()->setFocus();
}

void ControlView::showConflictDialog( MergeItem *item )
{
    Q_ASSERT( m_conflictDialog );
    m_conflictDialog->setMergeItem( item );
    m_conflictDialog->open();
}

void ControlView::updateAnnotationDockVisibility()
{
    if( m_annotationPlugin != 0 && m_annotationDock != 0 ) {
        if( m_annotationPlugin->visible() && m_annotationPlugin->enabled() ) {
            m_annotationDock->toggleViewAction()->setVisible( true );
        } else {
            m_annotationDock->setVisible( false );
            m_annotationDock->toggleViewAction()->setVisible( false );
        }
    }
}

void ControlView::updateAnnotationDock()
{
    const QList<QActionGroup*> *tmp_actionGroups = m_annotationPlugin->actionGroups();
    QWidget *widget = new QWidget( m_annotationDock );
    QVBoxLayout *layout = new QVBoxLayout;
    QToolBar *firstToolbar = new QToolBar( widget );
    QToolBar *secondToolbar = new QToolBar( widget );
    QSpacerItem *spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    if( !tmp_actionGroups->isEmpty() ) {
        bool firstToolbarFilled = false;
        foreach( QAction *action, tmp_actionGroups->first()->actions() ) {
            if (action->objectName() == QLatin1String("toolbarSeparator")) {
                firstToolbarFilled = true;
            } else {
                if( !firstToolbarFilled ) {
                    firstToolbar->addAction( action );
                } else {
                    secondToolbar->addAction( action );
                }
            }
        }
    }
    layout->addWidget( firstToolbar );
    layout->addWidget( secondToolbar );
    layout->addSpacerItem( spacer );
    widget->setLayout( layout );
    m_annotationDock->setWidget( widget );
}

void ControlView::togglePanelVisibility()
{
    Q_ASSERT( m_panelVisibility.size() == m_panelActions.size() );
    if ( m_isPanelVisible ) {
        for( int p=0; p<m_panelActions.size(); ++p ) {
            // Save state of individual dock visibility
            m_panelVisibility[p] = m_panelActions.at(p)->isChecked();

            // hide panel if it is showing
            if ( m_panelActions.at(p)->isChecked() ) {
                m_panelActions.at(p)->activate( QAction::Trigger );
            }
        }

        // Change Menu Item Text
        m_togglePanelVisibilityAction->setText( tr("Show &All Panels") );
        m_isPanelVisible = false;
    } else {
        for( int p=0; p<m_panelActions.size(); ++p ) {
            // show panel if it was showing before all panels were hidden
            if ( m_panelVisibility.at(p) && !m_panelActions.at(p)->isChecked() ) {
                m_panelActions.at(p)->activate( QAction::Trigger );
            }
        }

        // Change Menu Item Text
        m_togglePanelVisibilityAction->setText( tr("Hide &All Panels") );
        m_isPanelVisible = true;
    }
}

void ControlView::handleTourLinkClicked(const QString& path)
{
    QString tourPath = MarbleDirs::path( path );
    if ( !tourPath.isEmpty() ) {
        openTour( tourPath );
    }
}

void ControlView::openTour( const QString &filename )
{
    if ( m_tourWidget->openTour( filename ) ) {
        m_tourWidget->startPlaying();
    }
}

void ControlView::closeEvent( QCloseEvent *event )
{
    QCloseEvent newEvent;
    QCoreApplication::sendEvent( m_tourWidget, &newEvent );

    if ( newEvent.isAccepted() ) {
        event->accept();
    } else {
        event->ignore();
    }
}

void ControlView::dragEnterEvent(QDragEnterEvent *event)
{
    bool success = false;

    const QMimeData *mimeData = event->mimeData();

    GeoUriParser uriParser;

    // prefer urls
    if (mimeData->hasUrls()) {
        // be generous and take the first usable url
        foreach(const QUrl& url, mimeData->urls()) {
            uriParser.setGeoUri(url.url());
            success = uriParser.parse();
            if (success) {
                break;
            }
        }
    }

    // fall back to own string parsing
    if (!success && mimeData->hasText()) {
        const QString text = mimeData->text();
        // first try human readable coordinates
        GeoDataCoordinates::fromString(text, success);
        // next geo uri
        if (!success) {
            uriParser.setGeoUri(text);
            success = uriParser.parse();
        }
    }

    if (success) {
        event->acceptProposedAction();
    }
}

void ControlView::dropEvent(QDropEvent *event)
{
    bool success = false;

    const QMimeData *mimeData = event->mimeData();

    // prefer urls
    if (mimeData->hasUrls()) {
        // be generous and take the first usable url
        foreach(const QUrl& url, mimeData->urls()) {
            success = openGeoUri(url.url());
            if (success) {
                break;
            }
        }
    }

    // fall back to own string parsing
    if (!success && mimeData->hasText()) {
        const QString text = mimeData->text();
        // first try human readable coordinates
        const GeoDataCoordinates coordinates = GeoDataCoordinates::fromString(text, success);
        if (success) {
            const qreal longitude = coordinates.longitude(GeoDataCoordinates::Degree);
            const qreal latitude = coordinates.latitude(GeoDataCoordinates::Degree);
            m_marbleWidget->centerOn(longitude, latitude);
        } else {
            success = openGeoUri(text);
        }
    }
    if (success) {
        event->acceptProposedAction();
    }
}

}

#include "moc_ControlView.cpp"
