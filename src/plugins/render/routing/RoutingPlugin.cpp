//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Siddharth Srivastava <akssps011@gmail.com>
//

#include "RoutingPlugin.h"

#include "ui_RoutingWidgetSmall.h"
#include "ui_RoutingInformationWidget.h"
#include "ui_RoutingInformationWidgetSmall.h"

#include "WidgetGraphicsItem.h"
#include "MarbleGraphicsGridLayout.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "routing/AdjustNavigation.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"
#include "MarbleDirs.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "MarbleDataFacade.h"
#include "PositionTracking.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "MarbleDataFacade.h"
#include "GeoDataCoordinates.h"

#include <QtGui/QWidget>
#include <QtCore/QRect>
#include <QtGui/QToolButton>
#include <QtGui/QFont>
#include <QtGui/QActionGroup>
#include <QtGui/QPixmap>

using namespace Marble;

const int defaultMinZoom = 900;
const int defaultMaxZoom = 2400;
int const thresholdTime = 3;  //in minutes

RoutingPlugin::RoutingPlugin( const QPointF &point )
        : AbstractFloatItem( point ),
          m_marbleWidget( 0 ),
          m_widgetItem( 0 ),
          m_widgetItemRouting( 0 ),
          m_profiles( MarbleGlobal::getInstance()->profiles() ),
          m_routingModel( 0 ),
          m_adjustNavigation( 0 ),
          m_navigationMenu( 0 ),
          m_alwaysRecenterAction( 0 ),
          m_whenRequiredAction( 0 ),
          m_autoZoomAction( 0 ),
          m_disableRecenterAction( 0 ),
          m_currentSpeed( 0 ),
          m_remainingTime( 0 ),
          m_remainingDistance( 0 ),
          m_alwaysRecenter( false ),
          m_recenterWhenRequired( false ),
          m_routingWidgetSmall( 0 ),
          m_routingInformationWidget( 0 ),
          m_routingInformationWidgetSmall( 0 )
{
    setEnabled( true );
    //plugin is visible by default on small screen devices
    setVisible( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen );

    if( m_profiles & MarbleGlobal::SmallScreen ) {
         setFrame( FrameGraphicsItem::RectFrame );
    }
    else {
        setFrame( FrameGraphicsItem::RoundedRectFrame );
    }

    setPadding( 0 );
}

RoutingPlugin::~RoutingPlugin()
{
    delete m_routingWidgetSmall;
    delete m_routingInformationWidgetSmall;
    delete m_routingInformationWidget;
}

QStringList RoutingPlugin::backendTypes() const
{
    return QStringList( "routing" );
}

QString RoutingPlugin::name() const
{
    return tr( "Routing" );
}

QString RoutingPlugin::guiString() const
{
    return tr( "&Routing" );
}

QString RoutingPlugin::nameId() const
{
    return QString("routing");
}

QString RoutingPlugin::description() const
{
    return tr( "Routing information and navigation controls" );
}

QIcon RoutingPlugin::icon() const
{
    return QIcon();
}

void RoutingPlugin::initialize()
{
    QWidget *widget = new QWidget( 0 );
    QWidget *widgetSmall = new QWidget( 0 );

    if( m_profiles & MarbleGlobal::SmallScreen ) {
        m_routingWidgetSmall = new Ui::RoutingWidgetSmall;
        m_routingWidgetSmall->setupUi( widget );
        m_routingInformationWidgetSmall = new Ui::RoutingInformationWidgetSmall;
        m_routingInformationWidgetSmall->setupUi( widgetSmall );

        m_widgetItem = new WidgetGraphicsItem( this );
        m_widgetItem->setWidget( widget );
        m_widgetItemRouting = new WidgetGraphicsItem( this );
        m_widgetItemRouting->setWidget( widgetSmall );
        m_widgetItemRouting->setVisible( false );
        createNavigationMenu();
    }
    else {
        m_routingInformationWidget = new Ui::RoutingInformationWidget;
        m_routingInformationWidget->setupUi( widget );

        m_widgetItem = new WidgetGraphicsItem( this );
        m_widgetItem->setWidget( widget );
    }

    MarbleGraphicsGridLayout *layout = new MarbleGraphicsGridLayout( 1, 1 );
    layout->addItem( m_widgetItem, 0, 0 );
    setLayout( layout );
}

bool RoutingPlugin::isInitialized() const
{
    return m_widgetItem;
}

bool RoutingPlugin::eventFilter( QObject *object, QEvent *e )
{
    if ( !enabled() || !visible() ) {
        return false;
    }

    MarbleWidget *widget = dynamic_cast<MarbleWidget*> (object);

    if ( !widget ) {
       return AbstractFloatItem::eventFilter( object, e );
    }

    if ( m_marbleWidget != widget ) {
        m_marbleWidget = widget;
        m_adjustNavigation = m_marbleWidget->model()->routingManager()->adjustNavigation();
        m_routingModel = m_marbleWidget->model()->routingManager()->routingModel();

        if( m_profiles & MarbleGlobal::SmallScreen ) {

            // disconnect signals
            disconnect( m_marbleWidget->model()->positionTracking(), SIGNAL( positionProviderPluginChanged( PositionProviderPlugin* ) ),
                                    this, SLOT( setNavigationMenuDisabled( PositionProviderPlugin* ) ) );
            disconnect( m_routingWidgetSmall->navigationButton, SIGNAL( clicked( bool ) ),
                         this, SLOT( showNavigationMenu() ) );

            disconnect( m_alwaysRecenterAction, SIGNAL( triggered() ), this, SLOT( setAlwaysRecenter() ) );
            disconnect( m_whenRequiredAction, SIGNAL( triggered() ), this, SLOT( setRecenterWhenRequired() ) );
            disconnect( m_disableRecenterAction, SIGNAL( triggered() ), this, SLOT( setRecenteringDisabled() ) );
            disconnect( m_autoZoomAction, SIGNAL( triggered() ), this, SLOT( setAutoZoom() ) );

            disconnect( m_routingWidgetSmall->routingButton, SIGNAL( clicked( bool ) ),
                     this, SLOT( showRoutingItem( bool ) ) );
            disconnect( m_routingWidgetSmall->zoomInButton, SIGNAL( clicked() ),
                     m_marbleWidget, SLOT( zoomIn() ) );
            disconnect( m_routingWidgetSmall->zoomOutButton, SIGNAL( clicked() ),
                     m_marbleWidget, SLOT( zoomOut() ) );
            disconnect( m_marbleWidget, SIGNAL( themeChanged( QString ) ),
                     this, SLOT( selectTheme( QString ) ) );

            disconnect( m_adjustNavigation, SIGNAL( recenterModeChanged( int ) ),
                     this, SLOT( setRecenterMenu( int ) ) );
            disconnect( m_adjustNavigation, SIGNAL( autoZoomToggled( bool ) ),
                     this, SLOT( setAutoZoomMenu( bool ) ) );


            connect( m_marbleWidget->model()->positionTracking(), SIGNAL( positionProviderPluginChanged( PositionProviderPlugin* ) ),
                                   this, SLOT( setNavigationMenuDisabled( PositionProviderPlugin* ) ) );
            connect( m_routingWidgetSmall->navigationButton, SIGNAL( clicked( bool ) ),
                        this, SLOT( showNavigationMenu() ) );

            connect( m_alwaysRecenterAction, SIGNAL( triggered() ), this, SLOT( setAlwaysRecenter() ) );
            connect( m_whenRequiredAction, SIGNAL( triggered() ), this, SLOT( setRecenterWhenRequired() ) );
            connect( m_disableRecenterAction, SIGNAL( triggered() ), this, SLOT( setRecenteringDisabled() ) );
            connect( m_autoZoomAction, SIGNAL( triggered() ), this, SLOT( setAutoZoom() ) );

            connect( m_routingWidgetSmall->routingButton, SIGNAL( clicked( bool ) ),
                    this, SLOT( showRoutingItem( bool ) ) );
            connect( m_routingWidgetSmall->zoomInButton, SIGNAL( clicked() ),
                    m_marbleWidget, SLOT( zoomIn() ) );
            connect( m_routingWidgetSmall->zoomOutButton, SIGNAL( clicked() ),
                    m_marbleWidget, SLOT( zoomOut() ) );
            connect( m_marbleWidget, SIGNAL( themeChanged( QString ) ),
                    this, SLOT( selectTheme( QString ) ) );

            // connect statements below set the navigation menu if the action(re-center, autozoom) are
            // performed from CurrentLocation UI
            connect( m_adjustNavigation, SIGNAL( recenterModeChanged( int ) ),
                    this, SLOT( setRecenterMenu( int ) ) );
            connect( m_adjustNavigation, SIGNAL( autoZoomToggled( bool ) ),
                    this, SLOT( setAutoZoomMenu( bool ) ) );

            updateButtons( m_marbleWidget->zoom() );
        }
        else {
            disconnect( m_routingModel, SIGNAL( nextInstruction( qint32, qreal ) ),
                    this, SLOT( setDestinationInformation( qint32, qreal ) ) );
            connect( m_routingModel, SIGNAL( nextInstruction( qint32, qreal ) ),
                    this, SLOT( setDestinationInformation( qint32, qreal ) ), Qt::UniqueConnection );

            PositionTracking *tracking = m_marbleWidget->model()->positionTracking();
            disconnect( tracking, SIGNAL( gpsLocation( GeoDataCoordinates, qreal ) ),
                     this, SLOT( setCurrentLocation( GeoDataCoordinates, qreal ) ) );
            connect( tracking, SIGNAL( gpsLocation( GeoDataCoordinates, qreal ) ),
                     this, SLOT( setCurrentLocation( GeoDataCoordinates, qreal ) ) );
        }
    }
    return AbstractFloatItem::eventFilter( object, e );
}

void RoutingPlugin::showNavigationMenu()
{
    QPointF floatItemPosition = positivePosition();
    floatItemPosition += m_routingWidgetSmall->navigationButton->mapToParent( QPoint( 0, 0 ) );
    floatItemPosition.ry() += m_routingWidgetSmall->navigationButton->height();
    m_navigationMenu->exec( m_marbleWidget->mapToGlobal( floatItemPosition.toPoint() ) );
}

void RoutingPlugin::createNavigationMenu()
{
    QObject *object = this;

    m_disableRecenterAction = new QAction( object );
    m_disableRecenterAction->setText( tr( "Auto Center Disabled" ) );
    m_disableRecenterAction->setToolTip( tr( "Disable Auto Centering" ) );
    m_disableRecenterAction->setCheckable( true );
    m_disableRecenterAction->setChecked( true );

    m_alwaysRecenterAction = new QAction( object );
    m_alwaysRecenterAction->setText( tr( "Keep At Center" ) );
    m_alwaysRecenterAction->setToolTip( tr( "Always keep the gps location at the center of the map" ) );
    m_alwaysRecenterAction->setCheckable( true );

    m_whenRequiredAction = new QAction( object );
    m_whenRequiredAction->setText( tr( "Auto Center When Required" ) );
    m_whenRequiredAction->setToolTip( tr( "Re-center when required" ) );
    m_whenRequiredAction->setCheckable( true );

    m_autoZoomAction = new QAction( object );
    m_autoZoomAction->setText( tr( "Auto Zoom" ) );
    m_autoZoomAction->setToolTip( tr( "Auto Zoom to appropriate zoom level" ) );
    m_autoZoomAction->setCheckable( true );

    QActionGroup *recenterGroup = new QActionGroup( object );
    recenterGroup->addAction( m_disableRecenterAction );
    recenterGroup->addAction( m_alwaysRecenterAction );
    recenterGroup->addAction( m_whenRequiredAction );

    QList<QAction*> recenterMenu = recenterGroup->actions();
    m_navigationMenu = new QMenu();
    m_navigationMenu->addActions( recenterMenu );
    m_navigationMenu->addSeparator();
    m_navigationMenu->addAction( m_autoZoomAction );
    m_navigationMenu->setEnabled( false );
}

void RoutingPlugin::setAlwaysRecenter()
{
    if( !m_marbleWidget ) {
        return;
    }

    if( m_recenterWhenRequired ) {
        m_adjustNavigation->setRecenter( Disabled );
        m_recenterWhenRequired = false;
    }

    m_alwaysRecenter = true;

    if( m_alwaysRecenterAction->isChecked() ) {
        m_adjustNavigation->setRecenter( AlwaysRecenter );
    }
}

void RoutingPlugin::setRecenterWhenRequired()
{
    if( !m_marbleWidget ) {
        return;
    }

    if( m_alwaysRecenter ) {
        m_adjustNavigation->setRecenter( Disabled );
        m_alwaysRecenter = false;
    }

    m_recenterWhenRequired = true;

    if( m_whenRequiredAction->isChecked() ) {
        m_adjustNavigation->setRecenter( RecenterOnBorder );
    }
}

void RoutingPlugin::setAutoZoom()
{
    if( !m_marbleWidget ) {
        return;
    }

    if( m_autoZoomAction->isChecked() ) {
        m_adjustNavigation->setAutoZoom( true );
    }
    else {
        m_adjustNavigation->setAutoZoom( false );
        m_autoZoomAction->setChecked( false );
    }
}

//similar to updateButtons(int value) in navigation plugin
void RoutingPlugin::updateButtons( int zoomValue )
{
    int minZoom = defaultMinZoom;
    int maxZoom = defaultMaxZoom;
    QToolButton *zoomInButton = 0;
    QToolButton *zoomOutButton = 0;

    if ( m_marbleWidget ) {
        minZoom = m_marbleWidget->minimumZoom();
        maxZoom = m_marbleWidget->maximumZoom();
    }

    zoomInButton = m_routingWidgetSmall->zoomInButton;
    zoomOutButton = m_routingWidgetSmall->zoomOutButton;

    if ( zoomValue <= minZoom ) {
        zoomInButton->setEnabled( true );
        zoomOutButton->setEnabled( false );
    } else if ( zoomValue >= maxZoom ) {
        zoomInButton->setEnabled( false );
        zoomOutButton->setEnabled( true );
    } else {
        zoomInButton->setEnabled( true );
        zoomOutButton->setEnabled( true );
    }

    if ( m_marbleWidget )
    {
        // Trigger a repaint of the float item. Otherwise button state updates
        // are delayed
        QRectF floatItemRect = QRectF( positivePosition(), size() ).toRect();
        QRegion dirtyRegion( floatItemRect.toRect() );

        m_marbleWidget->setAttribute( Qt::WA_NoSystemBackground, false );

        update();

        m_marbleWidget->setAttribute( Qt::WA_NoSystemBackground,
                                      m_marbleWidget->viewport()->mapCoversViewport() );
    }
}

void RoutingPlugin::selectTheme( const QString &theme )
{
    Q_UNUSED( theme );

    if ( m_marbleWidget ) {
            updateButtons( m_marbleWidget->zoom() );
    }
}

void RoutingPlugin::showRoutingItem( bool show )
{
    if( !m_marbleWidget ) {
        return;
    }

    PositionTracking *tracking = m_marbleWidget->model()->positionTracking();

    if( show ) {
        MarbleGraphicsGridLayout *gridLayout = new MarbleGraphicsGridLayout( 2, 1 );
        gridLayout->addItem( m_widgetItemRouting, 0, 0 );
        gridLayout->addItem( m_widgetItem, 1, 0 );
        setLayout( gridLayout );
        m_widgetItemRouting->setVisible( show );
        m_marbleWidget->repaint();
        connect( m_routingModel, SIGNAL( nextInstruction( qint32, qreal ) ),
                this, SLOT( setDestinationInformation( qint32, qreal ) ), Qt::UniqueConnection );
        connect( tracking, SIGNAL( gpsLocation( GeoDataCoordinates, qreal ) ),
                 this, SLOT( setCurrentLocation( GeoDataCoordinates, qreal ) ) );
    }
    else {
        MarbleGraphicsGridLayout *gridLayout = new MarbleGraphicsGridLayout( 1, 1 );
        gridLayout->addItem( m_widgetItem, 0 , 0 );
        setLayout( gridLayout );
        m_widgetItemRouting->setVisible( show );
        m_marbleWidget->repaint();
        disconnect( m_routingModel, SIGNAL( nextInstruction( qint32, qreal ) ),
                this, SLOT( setDestinationInformation( qint32, qreal ) ) );
        disconnect( tracking, SIGNAL( gpsLocation( GeoDataCoordinates, qreal ) ),
                 this, SLOT( setCurrentLocation( GeoDataCoordinates, qreal ) ) );
    }
}

void RoutingPlugin::setDestinationInformation( qint32 remainingTime, qreal remainingDistance )
{
    m_remainingTime = remainingTime;
    m_remainingDistance = remainingDistance;
}

void RoutingPlugin::setCurrentLocation( GeoDataCoordinates position, qreal speed )
{
    m_currentPosition = position;
    m_currentSpeed = speed;
    bool hasRoute = m_routingModel->rowCount() != 0;
    if ( hasRoute ) {
        updateRoutingItem();
    }
}

void RoutingPlugin::updateRoutingItem()
{
    qreal instructionLength = m_routingModel->currentInstructionLength();
    qreal remainingInstructionDistance = m_routingModel->nextInstructionDistance();
    qint32 remainingTimeMinutes =  qint32( m_remainingTime * SEC2MIN ) % 60;
    qint32 remainingTimeHours =  qint32( m_remainingTime * SEC2HOUR );
    qint32 remainingInstructionTime = 0;

    //This icon represents that the gps device is now close to the instruction point(turn)
    QPixmap instructionIconNear( ":/icons/instruction-near.png" );
    // This icon represents the next instruction point on the route
    QPixmap instructionIconNext( ":/icons/instruction-next.png" );

    if( !m_currentSpeed ) {
        remainingInstructionTime = qint32( ( remainingInstructionDistance / m_currentSpeed ) / 60 ) % 60;
    }

    if( m_profiles & MarbleGlobal::SmallScreen ) {
       m_routingInformationWidgetSmall->remainingTimeLabel->setAlignment( Qt::AlignCenter );

       if( remainingTimeHours ) {
           m_routingInformationWidgetSmall->remainingTimeLabel->setText( QString::number( remainingTimeHours ) % " hr " %
                                                                         QString::number( remainingTimeMinutes ) % " min " );
       }
       else if( remainingTimeMinutes ) {
           m_routingInformationWidgetSmall->remainingTimeLabel->setText( tr( "%n minutes", "", remainingTimeMinutes ) );
       }
       else {
           m_routingInformationWidgetSmall->remainingTimeLabel->setText( "Less than a minute." );
       }

       if( remainingTimeMinutes < thresholdTime ) {
           m_routingInformationWidgetSmall->instructionIconLabel->setPixmap( instructionIconNear );
       }
       else {
           m_routingInformationWidgetSmall->instructionIconLabel->setPixmap( instructionIconNext );
       }

       updateInstructionLabel( m_routingInformationWidgetSmall->distanceAndInstructionLabel );

       if( !m_routingModel->deviatedFromRoute() && remainingInstructionDistance != 0 ) {
           m_routingInformationWidgetSmall->distanceCoveredProgressBar->setRange( 0, qRound( instructionLength ) );
           m_routingInformationWidgetSmall->distanceCoveredProgressBar->setValue( qRound ( instructionLength - remainingInstructionDistance ) );
        }

        if ( remainingInstructionDistance == 0 && remainingInstructionTime == 0 ) {
            m_routingInformationWidgetSmall->distanceCoveredProgressBar->setValue( qRound( instructionLength ) );
        }

    }
    else {
        m_routingInformationWidget->remainingTimeLabel->setAlignment( Qt::AlignCenter );

        if( remainingTimeHours ) {
            m_routingInformationWidget->remainingTimeLabel->setText( tr( "%n hours", "",  remainingTimeHours) % tr( " %n minutes", "", remainingTimeMinutes ) );
        }
        else if ( remainingTimeMinutes ) {
            m_routingInformationWidget->remainingTimeLabel->setText( tr( "%n minutes", "", remainingTimeMinutes ) );
        }
        else {
            m_routingInformationWidget->remainingTimeLabel->setText( "Less than a minute." );
        }

        if( remainingTimeMinutes < thresholdTime ) {
            m_routingInformationWidget->instructionIconLabel->setPixmap( instructionIconNear );
        }
        else {
            m_routingInformationWidget->instructionIconLabel->setPixmap( instructionIconNext );
        }

        updateInstructionLabel( m_routingInformationWidget->distanceAndInstructionLabel );

        if( !m_routingModel->deviatedFromRoute() && remainingInstructionDistance !=0 ) {
            m_routingInformationWidget->distanceCoveredProgressBar->setRange( 0, qRound( instructionLength ) );
            m_routingInformationWidget->distanceCoveredProgressBar->setValue( qRound ( instructionLength - remainingInstructionDistance ) );
        }

        if (  remainingInstructionDistance == 0 && remainingInstructionTime == 0 ) {
            m_routingInformationWidget->distanceCoveredProgressBar->setValue( qRound( instructionLength ) );
        }
    }
}

void RoutingPlugin::updateInstructionLabel( QLabel *label )
{
    if( m_routingModel->remainingTime() < thresholdTime && !m_routingModel->instructionText().isEmpty() ) {
        QLabel *instructionLabel = label;
        instructionLabel->setAlignment( Qt::AlignCenter );
        instructionLabel->setText( m_routingModel->instructionText() );
    }
    else {
        QLabel *nextInstructionDistanceLabel = label;
        nextInstructionDistanceLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );

        qreal instructionDistance = m_routingModel->nextInstructionDistance();

        QString indicatorText = m_routingModel->instructionText().isEmpty() ? "<b>Destination: </b>" : "<b>Next Turn at: </b>";

        if( m_remainingDistance ) {
            if( instructionDistance < 1000 ) {
                nextInstructionDistanceLabel->setText( indicatorText % QString::number( int( instructionDistance ) ) % " Meters " );
            }
            else {
                nextInstructionDistanceLabel->setText( indicatorText % QString::number( instructionDistance * METER2KM , 'f', 1 ) % " KM " );
            }
        }
        else {
            nextInstructionDistanceLabel->setText( "Arrived at Destination" );
        }
    }
}

void RoutingPlugin::setRecenterMenu( int centerMode )
{
    if( centerMode == AlwaysRecenter ) {
        m_alwaysRecenterAction->setChecked( true );
    }
    else if ( centerMode == RecenterOnBorder ) {
        m_whenRequiredAction->setChecked( true );
    }
    else {
        m_disableRecenterAction->setChecked( true );
    }
}

void RoutingPlugin::setAutoZoomMenu( bool autoZoom )
{
    m_autoZoomAction->setChecked( autoZoom );
}

void RoutingPlugin::setRecenteringDisabled()
{
    m_adjustNavigation->setRecenter( Disabled );
}

void RoutingPlugin::setNavigationMenuDisabled( PositionProviderPlugin *activePlugin )
{
    m_navigationMenu->setEnabled( activePlugin != 0 );
}

Q_EXPORT_PLUGIN2( RoutingPlugin, Marble::RoutingPlugin )

#include "RoutingPlugin.moc"
