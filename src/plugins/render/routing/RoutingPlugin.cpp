//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Siddharth Srivastava <akssps011@gmail.com>
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "RoutingPlugin.h"

#include "ui_RoutingPlugin.h"
#include "ui_RoutingConfigDialog.h"

#include "Planet.h"
#include "AudioOutput.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLookAt.h"
#include "GeoPainter.h"
#include "MarbleGraphicsGridLayout.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "MarbleMath.h"
#include "MarbleLocale.h"
#include "MarbleDirs.h"
#include "PluginManager.h"
#include "PositionTracking.h"
#include "PositionProviderPlugin.h"
#include "routing/Route.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"
#include "routing/RouteRequest.h"
#include "routing/SpeakersModel.h"
#include "ViewportParams.h"
#include "WidgetGraphicsItem.h"

#include <QDialog>
#include <QPushButton>

namespace Marble
{

namespace
{
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
    AudioOutput* m_audio;
    QDialog *m_configDialog;
    Ui::RoutingConfigDialog m_configUi;
    bool m_routeCompleted;
    SpeakersModel* m_speakersModel;

    RoutingPluginPrivate( RoutingPlugin* parent );

    void updateZoomButtons( int zoomValue );

    void updateZoomButtons();

    void updateGuidanceModeButton();

    void forceRepaint();

    void updateButtonVisibility();

    void reverseRoute();

    void toggleGuidanceMode( bool enabled );

    void updateDestinationInformation();

    void updateGpsButton( PositionProviderPlugin *activePlugin );

    void togglePositionTracking( bool enabled );

    static QString richText( const QString &source );

    static QString fuzzyDistance( qreal distanceMeter );

    void readSettings();

    qreal nextInstructionDistance() const;

    qreal remainingDistance() const;

private:
    RoutingPlugin* m_parent;
};

RoutingPluginPrivate::RoutingPluginPrivate( RoutingPlugin *parent ) :
    m_marbleWidget( 0 ),
    m_widgetItem( 0 ),
    m_routingModel( 0 ),
    m_nearNextInstruction( false ),
    m_guidanceModeEnabled( false ),
    m_audio( new AudioOutput( parent ) ),
    m_configDialog( 0 ),
    m_routeCompleted( false ),
    m_speakersModel( 0 ),
    m_parent( parent )
{
    m_audio->setMuted( false );
    m_audio->setSoundEnabled( true );
}

QString RoutingPluginPrivate::richText( const QString &source )
{
    return QLatin1String("<font size=\"+1\" color=\"black\">") + source + QLatin1String("</font>");
}

QString RoutingPluginPrivate::fuzzyDistance( qreal length )
{
    int precision = 0;
    QString distanceUnit = QLatin1String( "m" );

    if ( MarbleGlobal::getInstance()->locale()->measurementSystem() != MarbleLocale::MetricSystem ) {
        precision = 1;
        distanceUnit = "mi";
        length *= METER2KM;
        length *= KM2MI;
    } else if (MarbleGlobal::getInstance()->locale()->measurementSystem() ==
               MarbleLocale::MetricSystem) {
        if ( length >= 1000 ) {
            length /= 1000;
            distanceUnit = "km";
            precision = 1;
        } else if ( length >= 200 ) {
            length = 50 * qRound( length / 50 );
        } else if ( length >= 100 ) {
            length = 25 * qRound( length / 25 );
        } else {
            length = 10 * qRound( length / 10 );
        }
    } else if (MarbleGlobal::getInstance()->locale()->measurementSystem() ==
               MarbleLocale::NauticalSystem) {
        precision = 2;
        distanceUnit = "nm";
        length *= METER2KM;
        length *= KM2NM;
    }

    return QString( "%1 %2" ).arg( length, 0, 'f', precision ).arg( distanceUnit );
}

void RoutingPluginPrivate::updateZoomButtons( int zoomValue )
{
    int const minZoom = m_marbleWidget ? m_marbleWidget->minimumZoom() : 900;
    int const maxZoom = m_marbleWidget ? m_marbleWidget->maximumZoom() : 2400;

    bool const zoomInEnabled = zoomValue < maxZoom;
    bool const zoomOutEnabled = zoomValue > minZoom;

    if ( ( zoomInEnabled != m_widget.zoomInButton->isEnabled() ) ||
         ( zoomOutEnabled != m_widget.zoomOutButton->isEnabled() ) ) {
        m_widget.zoomInButton->setEnabled( zoomInEnabled );
        m_widget.zoomOutButton->setEnabled( zoomOutEnabled );
        forceRepaint();
    }
}

void RoutingPluginPrivate::updateGuidanceModeButton()
{
    bool const hasRoute = m_routingModel->rowCount() > 0;
    m_widget.routingButton->setEnabled( hasRoute );
    forceRepaint();
}

void RoutingPluginPrivate::forceRepaint()
{
    m_parent->update();
    emit m_parent->repaintNeeded();
}

void RoutingPluginPrivate::updateButtonVisibility()
{
    bool const show = m_guidanceModeEnabled;
    bool const near = show && m_nearNextInstruction;
    m_widget.progressBar->setVisible( near );
    m_widget.instructionIconLabel->setVisible( show );
    m_widget.spacer->changeSize( show ? 10 : 0, 20 );
    m_widget.instructionLabel->setVisible( show );

    // m_widget.followingInstructionIconLabel->setVisible( show );
    // Disabling the next instruction turn icon for now, it seems to confuse first time users.
    m_widget.followingInstructionIconLabel->setVisible( false );

    m_widget.destinationDistanceLabel->setVisible( show );

    m_widget.gpsButton->setVisible( !show );
    m_widget.zoomOutButton->setVisible( !show );
    m_widget.zoomInButton->setVisible( !show );

    m_widgetItem->widget()->updateGeometry();
    QSize const size = m_widgetItem->widget()->sizeHint();
    m_widgetItem->widget()->resize( size );
    m_widgetItem->setContentSize( size );

    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( smallScreen ) {
        qreal const pluginWidth = size.width();
        int x = -10;
        if ( m_guidanceModeEnabled ) {
            int const parentWidth = m_marbleWidget->width();
            x = qRound( ( parentWidth - pluginWidth ) / 2.0 );
        }
        m_parent->setPosition( QPointF( x, m_parent->position().y() ) );
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
        QObject::connect( m_routingModel, SIGNAL(positionChanged()),
                 m_parent, SLOT(updateDestinationInformation()) );
    } else {
        QObject::disconnect( m_routingModel, SIGNAL(positionChanged()),
                    m_parent, SLOT(updateDestinationInformation()) );
    }

    if ( enabled ) {
        QString const text = QObject::tr( "Starting guidance mode, please wait..." );
        m_widget.instructionLabel->setText( richText( "%1" ).arg( text ) );
    }

    if ( enabled ) {
        RouteRequest* request = m_marbleWidget->model()->routingManager()->routeRequest();
        if ( request && request->size() > 0 ) {
            GeoDataCoordinates source = request->source();
            if ( source.isValid() ) {
                GeoDataLookAt view;
                view.setCoordinates( source );
                // By happy coincidence this equals OpenStreetMap tile level 15
                view.setRange( 851.807 );
                m_marbleWidget->flyTo( view );
            }
        }
    }

    m_marbleWidget->model()->routingManager()->setGuidanceModeEnabled( enabled );

    if ( enabled ) {
        m_routeCompleted = false;
    }

    forceRepaint();
}

void RoutingPluginPrivate::updateDestinationInformation()
{
    if ( m_routingModel->route().currentSegment().isValid() ) {
        qreal remaining = remainingDistance();
        qreal distanceLeft = nextInstructionDistance();
        m_audio->update( m_routingModel->route(), distanceLeft, remaining, m_routingModel->deviatedFromRoute() );

        m_nearNextInstruction = distanceLeft < thresholdDistance;

        QString pixmapHtml = "<img src=\":/flag.png\" /><br />";
        m_widget.destinationDistanceLabel->setText( pixmapHtml + richText( fuzzyDistance( remaining ) ) );

        m_widget.instructionIconLabel->setEnabled( m_nearNextInstruction );
        m_widget.progressBar->setMaximum( thresholdDistance );
        m_widget.progressBar->setValue( qRound( distanceLeft ) );

        updateButtonVisibility();

        QString pixmap = MarbleDirs::path(QStringLiteral("bitmaps/routing_step.png"));
        pixmapHtml = QString( "<img src=\"%1\" />" ).arg( pixmap );

        qreal planetRadius = m_marbleWidget->model()->planet()->radius();
        GeoDataCoordinates const onRoute = m_routingModel->route().positionOnRoute();
        GeoDataCoordinates const ego = m_routingModel->route().position();
        qreal const distanceToRoute = planetRadius * distanceSphere( ego, onRoute );

        if ( !m_routingModel->route().currentSegment().isValid() ) {
            m_widget.instructionLabel->setText( richText( QObject::tr( "Calculate a route to get directions." ) ) );
            m_widget.instructionIconLabel->setText( pixmapHtml );
        } else if ( distanceToRoute > 300.0 ) {
            m_widget.instructionLabel->setText( richText( QObject::tr( "Route left." ) ) );
            m_widget.instructionIconLabel->setText( pixmapHtml );
        } else if ( !m_routingModel->route().currentSegment().nextRouteSegment().isValid() ) {
            m_widget.instructionLabel->setText( richText( QObject::tr( "Destination ahead." ) ) );
            m_widget.instructionIconLabel->setText( pixmapHtml );
        } else {
            pixmap = m_routingModel->route().currentSegment().nextRouteSegment().maneuver().directionPixmap();
            QString const instructionText = m_routingModel->route().currentSegment().nextRouteSegment().maneuver().instructionText();
            m_widget.instructionLabel->setText( richText( "%1" ).arg( instructionText ) );
            pixmapHtml = QString( "<p align=\"center\"><img src=\"%1\" /><br />%2</p>" ).arg( pixmap );
            m_widget.instructionIconLabel->setText( pixmapHtml.arg( richText( fuzzyDistance( distanceLeft ) ) ) );

            if( remaining > 50 ) {
                m_routeCompleted = false;
            } else {
                if ( !m_routeCompleted ) {
                    QString content = QObject::tr( "Arrived at destination. <a href=\"#reverse\">Calculate the way back.</a>" );
                    m_widget.instructionLabel->setText( richText( "%1" ).arg( content ) );
                }
                m_routeCompleted = true;
            }
        }

        forceRepaint();
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
        const PluginManager* pluginManager = m_marbleWidget->model()->pluginManager();
        QList<const PositionProviderPlugin*> plugins = pluginManager->positionProviderPlugins();
        if ( plugins.size() > 0 ) {
            plugin = plugins.first()->newInstance();
        }
    }
    m_parent->marbleModel()->positionTracking()->setPositionProviderPlugin( plugin );
}

void RoutingPluginPrivate::reverseRoute()
{
    if ( m_marbleWidget ) {
        m_marbleWidget->model()->routingManager()->reverseRoute();
    }
}

void RoutingPluginPrivate::readSettings()
{
    if ( m_configDialog ) {
        if ( !m_speakersModel ) {
            m_speakersModel = new SpeakersModel( m_parent );
        }
        int const index = m_speakersModel->indexOf( m_audio->speaker() );
        m_configUi.speakerComboBox->setModel( m_speakersModel );
        m_configUi.speakerComboBox->setCurrentIndex( index );
        m_configUi.voiceNavigationCheckBox->setChecked( !m_audio->isMuted() );
        m_configUi.soundRadioButton->setChecked( m_audio->isSoundEnabled() );
        m_configUi.speakerRadioButton->setChecked( !m_audio->isSoundEnabled() );
    }
}

qreal RoutingPluginPrivate::nextInstructionDistance() const
{
    GeoDataCoordinates position = m_routingModel->route().position();
    GeoDataCoordinates interpolated = m_routingModel->route().positionOnRoute();
    GeoDataCoordinates onRoute = m_routingModel->route().currentWaypoint();
    qreal planetRadius = m_marbleWidget->model()->planet()->radius();
    qreal distance = planetRadius * ( distanceSphere( position, interpolated ) + distanceSphere( interpolated, onRoute ) );
    const RouteSegment &segment = m_routingModel->route().currentSegment();
    for (int i=0; i<segment.path().size(); ++i) {
        if (segment.path()[i] == onRoute) {
            return distance + segment.path().length( planetRadius, i );
        }
    }

    return distance;
}

qreal RoutingPluginPrivate::remainingDistance() const
{
    GeoDataCoordinates position = m_routingModel->route().currentSegment().maneuver().position();
    bool foundSegment = false;
    qreal distance = nextInstructionDistance();
    for ( int i=0; i<m_routingModel->route().size(); ++i ) {
        if ( foundSegment ) {
            distance += m_routingModel->route().at( i ).distance();
        } else {
            foundSegment =  m_routingModel->route().at( i ).maneuver().position() == position;
        }
    }

    return distance;
}

void RoutingPlugin::writeSettings()
{
    Q_ASSERT( d->m_configDialog );
    int const index = d->m_configUi.speakerComboBox->currentIndex();
    if ( index >= 0 ) {
        QModelIndex const idx = d->m_speakersModel->index( index );
        d->m_audio->setSpeaker( d->m_speakersModel->data( idx, SpeakersModel::Path ).toString() );
        if ( !d->m_speakersModel->data( idx, SpeakersModel::IsLocal ).toBool() ) {
            d->m_speakersModel->install( index );
        }
    }
    d->m_audio->setMuted( !d->m_configUi.voiceNavigationCheckBox->isChecked() );
    d->m_audio->setSoundEnabled( d->m_configUi.soundRadioButton->isChecked() );
    d->readSettings();
    emit settingsChanged( nameId() );
}


RoutingPlugin::RoutingPlugin() :
    AbstractFloatItem( 0 ),
    d( 0 )
{
}

RoutingPlugin::RoutingPlugin( const MarbleModel *marbleModel ) :
    AbstractFloatItem( marbleModel, QPointF( -10, -10 ) ),
    d( new RoutingPluginPrivate( this ) )
{
    setEnabled( true );
    //plugin is visible by default on small screen devices
    setVisible( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen );
    setPadding( 0.5 );
    setBorderWidth( 1 );
    setBackground( QBrush( QColor( "white" ) ) );
}

RoutingPlugin::~RoutingPlugin()
{
    delete d;
}

QStringList RoutingPlugin::backendTypes() const
{
    return QStringList(QStringLiteral("routing"));
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
    return QStringLiteral("routing");
}

QString RoutingPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString RoutingPlugin::description() const
{
    return tr( "Routing information and navigation controls" );
}

QString RoutingPlugin::copyrightYears() const
{
    return QStringLiteral("2010");
}

QVector<PluginAuthor> RoutingPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Siddharth Srivastava"), QStringLiteral("akssps011@gmail.com"))
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"));
}

QIcon RoutingPlugin::icon() const
{
    return QIcon(QStringLiteral(":/icons/routeplanning.png"));
}

void RoutingPlugin::initialize()
{
    QWidget *widget = new QWidget;
    d->m_widget.setupUi( widget );
    d->m_widgetItem = new WidgetGraphicsItem( this );
    d->m_widgetItem->setWidget( widget );

    PositionProviderPlugin* activePlugin = marbleModel()->positionTracking()->positionProviderPlugin();
    d->updateGpsButton( activePlugin );
    connect( marbleModel()->positionTracking(),
             SIGNAL(positionProviderPluginChanged(PositionProviderPlugin*)),
             this, SLOT(updateGpsButton(PositionProviderPlugin*)) );

    d->m_widget.routingButton->setEnabled( false );
    connect( d->m_widget.instructionLabel, SIGNAL(linkActivated(QString)),
             this, SLOT(reverseRoute()) );

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
    if ( d->m_marbleWidget || !enabled() || !visible() ) {
        return AbstractFloatItem::eventFilter( object, e );
    }

    MarbleWidget *widget = dynamic_cast<MarbleWidget*> ( object );

    if ( widget && !d->m_marbleWidget ) {
        d->m_marbleWidget = widget;
        d->m_routingModel = d->m_marbleWidget->model()->routingManager()->routingModel();

        connect( d->m_widget.routingButton, SIGNAL(clicked(bool)),
                 this, SLOT(toggleGuidanceMode(bool)) );
        connect( d->m_widget.gpsButton, SIGNAL(clicked(bool)),
                 this, SLOT(togglePositionTracking(bool)) );
        connect( d->m_widget.zoomInButton, SIGNAL(clicked()),
                 d->m_marbleWidget, SLOT(zoomIn()) );
        connect( d->m_widget.zoomOutButton, SIGNAL(clicked()),
                 d->m_marbleWidget, SLOT(zoomOut()) );
        connect( d->m_marbleWidget, SIGNAL(themeChanged(QString)),
                 this, SLOT(updateZoomButtons()) );
        connect( d->m_marbleWidget, SIGNAL(zoomChanged(int)),
                 this, SLOT(updateZoomButtons(int)) );
        connect( d->m_routingModel, SIGNAL(currentRouteChanged()),
                this, SLOT(updateGuidanceModeButton()) );
        d->updateGuidanceModeButton();
    }
    return AbstractFloatItem::eventFilter( object, e );
}

QHash<QString,QVariant> RoutingPlugin::settings() const
{
    QHash<QString, QVariant> result = AbstractFloatItem::settings();

    result.insert(QStringLiteral("muted"), d->m_audio->isMuted());
    result.insert(QStringLiteral("sound"), d->m_audio->isSoundEnabled());
    result.insert(QStringLiteral("speaker"), d->m_audio->speaker());

    return result;
}

void RoutingPlugin::setSettings( const QHash<QString,QVariant> &settings )
{
    AbstractFloatItem::setSettings( settings );

    d->m_audio->setMuted(settings.value(QStringLiteral("muted"), false).toBool());
    d->m_audio->setSoundEnabled(settings.value(QStringLiteral("sound"), true).toBool());
    d->m_audio->setSpeaker(settings.value(QStringLiteral("speaker")).toString());

    d->readSettings();
}

QDialog *RoutingPlugin::configDialog()
{
    if ( !d->m_configDialog ) {
        d->m_configDialog = new QDialog;
        d->m_configUi.setupUi( d->m_configDialog );
        d->readSettings();

        connect( d->m_configDialog, SIGNAL(accepted()), this, SLOT(writeSettings()) );
        connect( d->m_configDialog, SIGNAL(rejected()), this, SLOT(readSettings()) );
        connect( d->m_configUi.buttonBox->button( QDialogButtonBox::Reset ), SIGNAL(clicked()),
                 SLOT(restoreDefaultSettings()) );
    }

    return d->m_configDialog;
}

}

#include "moc_RoutingPlugin.cpp"
