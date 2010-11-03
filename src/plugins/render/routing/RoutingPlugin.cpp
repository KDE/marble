//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Siddharth Srivastava <akssps011@gmail.com>
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutingPlugin.h"

#include "ui_RoutingPlugin.h"

#include "GeoDataCoordinates.h"
#include "GeoPainter.h"
#include "MarbleDataFacade.h"
#include "MarbleGraphicsGridLayout.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "PluginManager.h"
#include "PositionTracking.h"
#include "PositionProviderPlugin.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"
#include "routing/RouteRequest.h"
#include "ViewportParams.h"
#include "WidgetGraphicsItem.h"

#include <QtCore/QRect>
#include <QtGui/QWidget>
#include <QtGui/QToolButton>
#include <QtGui/QFont>
#include <QtGui/QActionGroup>
#include <QtGui/QPixmap>
#include <QtCore/QDebug>

namespace Marble
{

namespace
{
int const thresholdTime = 3; // in minutes
int const thresholdDistance = 1000; // in meter
}

class RoutingPluginPrivate
{
public:
    MarbleWidget* m_marbleWidget;
    WidgetGraphicsItem* m_widgetItem;
    RoutingModel* m_routingModel;
    Ui::RoutingPlugin m_widget;
    bool m_nearNextInstruction;
    bool m_guidanceModeEnabled;

    RoutingPluginPrivate( RoutingPlugin* parent );

    void updateZoomButtons( int zoomValue );

    void updateZoomButtons();

    void updateInstructionLabel( int fontSize, qreal remainingDistance );

    void forceRepaint();

    void updateButtonVisibility();

    void reverseRoute();

    void toggleGuidanceMode( bool enabled );

    void updateDestinationInformation( qint32, qreal );

    void updateGpsButton( PositionProviderPlugin *activePlugin );

    void togglePositionTracking( bool enabled );

    QString richText( const QString &source, int fontSize ) const;

private:
    RoutingPlugin* m_parent;
};

RoutingPluginPrivate::RoutingPluginPrivate( RoutingPlugin *parent ) :
    m_marbleWidget( 0 ),
    m_widgetItem( 0 ),
    m_routingModel( 0 ),
    m_nearNextInstruction( false ),
    m_guidanceModeEnabled( false ),
    m_parent( parent )
{
    // nothing to do
}

QString RoutingPluginPrivate::richText( const QString &source, int size ) const
{
    QString const fontSize = size > 0 ? "+" + fontSize : QString::number( size );
    return QString( "<font size=\"%1\" color=\"black\">%2</font>" ).arg( fontSize ).arg( source );
}

void RoutingPluginPrivate::updateZoomButtons( int zoomValue )
{
    int const minZoom = m_marbleWidget ? m_marbleWidget->minimumZoom() : 900;
    int const maxZoom = m_marbleWidget ? m_marbleWidget->maximumZoom() : 2400;

    m_widget.zoomInButton->setEnabled( zoomValue < maxZoom );
    m_widget.zoomOutButton->setEnabled( zoomValue > minZoom );

    forceRepaint();
}

void RoutingPluginPrivate::forceRepaint()
{
    if ( m_marbleWidget ) {
        // Trigger a repaint of the float item. Otherwise button state updates are delayed
        m_marbleWidget->setAttribute( Qt::WA_NoSystemBackground, false );
        m_parent->update();
        bool const mapCoversViewport = m_marbleWidget->viewport()->mapCoversViewport();
        m_marbleWidget->setAttribute( Qt::WA_NoSystemBackground, mapCoversViewport );
    }
}

void RoutingPluginPrivate::updateButtonVisibility()
{
    bool const show = m_guidanceModeEnabled;
    bool const near = show && m_nearNextInstruction;
    m_widget.progressBar->setVisible( near );
    m_widget.instructionIconLabel->setVisible( show );
    m_widget.instructionLabel->setVisible( show );
    m_widget.followingInstructionIconLabel->setVisible( show );
    m_widget.destinationDistanceLabel->setVisible( show );

    m_widget.gpsButton->setVisible( !show );
    m_widget.zoomOutButton->setVisible( !show );
    m_widget.zoomInButton->setVisible( !show );

    m_widgetItem->widget()->updateGeometry();
    QSize const size = m_widgetItem->widget()->sizeHint();
    m_widgetItem->widget()->resize( size );
    m_widgetItem->setContentSize( size );

    if ( m_marbleWidget ) {
        m_marbleWidget->repaint();
    }
}

void RoutingPluginPrivate::updateZoomButtons()
{
    if ( m_marbleWidget ) {
        updateZoomButtons( m_marbleWidget->zoom() );
    }
}

void RoutingPluginPrivate::toggleGuidanceMode( bool enabled )
{
    if( !m_marbleWidget || m_guidanceModeEnabled == enabled ) {
        return;
    }

    m_guidanceModeEnabled = enabled;
    updateButtonVisibility();

    if( enabled ) {
        QObject::connect( m_routingModel, SIGNAL( nextInstruction( qint32, qreal ) ),
                 m_parent, SLOT( updateDestinationInformation( qint32, qreal ) ) );
    } else {
        QObject::disconnect( m_routingModel, SIGNAL( nextInstruction( qint32, qreal ) ),
                    m_parent, SLOT( updateDestinationInformation( qint32, qreal ) ) );
    }

    PositionTracking *tracking = m_marbleWidget->model()->positionTracking();
    if ( enabled && !tracking->positionProviderPlugin() ) {
        RouteRequest* request = m_marbleWidget->model()->routingManager()->routeRequest();
        if ( request && request->size() > 0 ) {
            GeoDataCoordinates source = request->source();
            GeoDataLookAt view;
            view.setCoordinates( source );
            view.setRange( 750 );
            m_marbleWidget->flyTo( view );
        }
    }

    m_marbleWidget->model()->routingManager()->setGuidanceModeEnabled( enabled );
    forceRepaint();
}

void RoutingPluginPrivate::updateDestinationInformation( qint32 remainingTime, qreal remainingDistance )
{
    if ( m_routingModel->rowCount() != 0 ) {
        qreal distanceLeft = m_routingModel->nextInstructionDistance();
        qint32 minutesLeft =  qint32( remainingTime * SEC2MIN ) % 60;
        qint32 hoursLeft =  qint32( remainingTime * SEC2HOUR );

        m_nearNextInstruction = distanceLeft < thresholdDistance;
        int fontSize = 1;
        if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
            fontSize = m_nearNextInstruction ? 1 : -1;
        }

        if( hoursLeft ) {
            QString text = richText( "%1:%2", -1 ).arg( hoursLeft ).arg( minutesLeft );
            m_widget.destinationDistanceLabel->setText( text );
        } else if( minutesLeft ) {
            QString text = richText( "%1 min", -1 ).arg( minutesLeft );
            m_widget.destinationDistanceLabel->setText( text );
        } else {
            m_widget.destinationDistanceLabel->setText( richText( "Soon", -1 ) );
        }

        m_widget.instructionIconLabel->setEnabled( m_nearNextInstruction );
        m_widget.progressBar->setMaximum( thresholdDistance );
        m_widget.progressBar->setValue( qRound( distanceLeft ) );

        updateButtonVisibility();

        QPixmap pixmap = m_routingModel->nextInstructionPixmap();
        if ( !pixmap.isNull() ) {
            m_widget.instructionIconLabel->setPixmap( pixmap.scaled( 64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) );
        }
        m_widget.followingInstructionIconLabel->setPixmap( m_routingModel->followingInstructionPixmap() );

        updateInstructionLabel( fontSize, remainingDistance );
    }
}

void RoutingPluginPrivate::updateInstructionLabel( int fontSize, qreal remainingDistance )
{
    if( m_routingModel->remainingTime() < thresholdTime && !m_routingModel->instructionText().isEmpty() ) {
        m_widget.instructionLabel->setText( richText( "%1", fontSize ).arg( m_routingModel->instructionText() ) );
    } else {
        qreal instructionDistance = m_routingModel->nextInstructionDistance();
        QString indicatorText = m_routingModel->instructionText().isEmpty() ?
                                richText( "Destination reached in %1 %2", fontSize ) :
                                richText( "Next turn in %1 %2", fontSize );

        if( remainingDistance ) {
            if( instructionDistance < 1000 ) {
                m_widget.instructionLabel->setText( indicatorText.arg( int( instructionDistance ) ).arg( "meters" ) );
            } else {
                m_widget.instructionLabel->setText( indicatorText.arg( instructionDistance * METER2KM, 0, 'f', 1 ).arg( " km " ) );
            }
        } else {
            QString content = "Arrived at destination. <a href=\"#reverse\">Calculate the way back.</a>";
            m_widget.instructionLabel->setText( richText( "%1", fontSize ).arg( content ) );
        }
    }
}

void RoutingPluginPrivate::updateGpsButton( PositionProviderPlugin *activePlugin )
{
    m_widget.gpsButton->setChecked( activePlugin != 0 );
    forceRepaint();
}

void RoutingPluginPrivate::togglePositionTracking( bool enabled )
{
    PositionProviderPlugin* plugin = 0;
    if ( enabled ) {
        PluginManager* pluginManager = m_marbleWidget->model()->pluginManager();
        QList<PositionProviderPlugin*> plugins = pluginManager->createPositionProviderPlugins();
        if ( plugins.size() > 0 ) {
            plugin = plugins.takeFirst();
        }
        qDeleteAll( plugins );
    }
    m_parent->dataFacade()->positionTracking()->setPositionProviderPlugin( plugin );
}

void RoutingPluginPrivate::reverseRoute()
{
    if ( m_marbleWidget ) {
        m_marbleWidget->model()->routingManager()->reverseRoute();
    }
}

RoutingPlugin::RoutingPlugin( const QPointF &position ) :
    AbstractFloatItem( position ),
    d( new RoutingPluginPrivate( this ) )
{
    setEnabled( true );
    //plugin is visible by default on small screen devices
    setVisible( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen );
    setPadding( 0 );
}

RoutingPlugin::~RoutingPlugin()
{
    delete d;
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
    return QString( "routing" );
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
    QWidget *widget = new QWidget;
    d->m_widget.setupUi( widget );
    PositionProviderPlugin* activePlugin = dataFacade()->positionTracking()->positionProviderPlugin();
    d->m_widget.gpsButton->setChecked( activePlugin != 0 );
    connect( d->m_widget.instructionLabel, SIGNAL( linkActivated( QString ) ),
             this, SLOT( reverseRoute() ) );

    d->m_widgetItem = new WidgetGraphicsItem( this );
    d->m_widgetItem->setWidget( widget );

    MarbleGraphicsGridLayout *layout = new MarbleGraphicsGridLayout( 1, 1 );
    layout->addItem( d->m_widgetItem, 0, 0 );
    setLayout( layout );
    d->updateButtonVisibility();
}

bool RoutingPlugin::isInitialized() const
{
    return d->m_widgetItem;
}

bool RoutingPlugin::eventFilter( QObject *object, QEvent *e )
{
    if ( !enabled() || !visible() ) {
        return AbstractFloatItem::eventFilter( object, e );
    }

    MarbleWidget *widget = dynamic_cast<MarbleWidget*> ( object );

    if ( widget && !d->m_marbleWidget ) {
        d->m_marbleWidget = widget;
        d->m_routingModel = d->m_marbleWidget->model()->routingManager()->routingModel();

        connect( d->m_marbleWidget->model()->positionTracking(),
                 SIGNAL( positionProviderPluginChanged( PositionProviderPlugin* ) ),
                 this, SLOT( updateGpsButton( PositionProviderPlugin* ) ) );

        connect( d->m_widget.routingButton, SIGNAL( clicked( bool ) ),
                 this, SLOT( toggleGuidanceMode( bool ) ) );
        connect( d->m_widget.gpsButton, SIGNAL( clicked( bool ) ),
                 this, SLOT( togglePositionTracking( bool ) ) );
        connect( d->m_widget.zoomInButton, SIGNAL( clicked() ),
                 d->m_marbleWidget, SLOT( zoomIn() ) );
        connect( d->m_widget.zoomOutButton, SIGNAL( clicked() ),
                 d->m_marbleWidget, SLOT( zoomOut() ) );
        connect( d->m_marbleWidget, SIGNAL( themeChanged( QString ) ),
                 this, SLOT( updateZoomButtons() ) );
        connect( d->m_marbleWidget, SIGNAL( zoomChanged( int ) ),
                 this, SLOT( updateZoomButtons( int ) ) );

        d->updateZoomButtons();
    }
    return AbstractFloatItem::eventFilter( object, e );
}

}

Q_EXPORT_PLUGIN2( RoutingPlugin, Marble::RoutingPlugin )

#include "RoutingPlugin.moc"
