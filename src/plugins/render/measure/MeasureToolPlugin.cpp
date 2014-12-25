//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn     <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin      <ingwa@kde.org>
// Copyright 2007-2008 Carlos Licea     <carlos.licea@kdemail.net>
// Copyright 2011      Michael Henning  <mikehenning@eclipse.net>
// Copyright 2011      Valery Kharitonov  <kharvd@gmail.com>
// Copyright 2012      Mohammed Nafees  <nafees.technocool@gmail.com>
//

#include "MeasureToolPlugin.h"
#include "ui_MeasureConfigWidget.h"

#include "GeoPainter.h"
#include "GeoDataLinearRing.h"
#include "MarbleDebug.h"
#include "MarbleMath.h"
#include "MarbleModel.h"
#include "MarbleLocale.h"
#include "ViewportParams.h"
#include "Planet.h"

#include <QColor>
#include <QPen>
#include <QPixmap>
#include <QPushButton>
#include <QCheckBox>
#include <qmath.h>

namespace Marble
{

MeasureToolPlugin::MeasureToolPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
      m_measureLineString( GeoDataLineString( Tessellate ) ),
      m_mark( ":/mark.png" ),
#ifdef Q_OS_MACX
      m_font_regular( QFont( "Sans Serif", 10, 50, false ) ),
#else
      m_font_regular( QFont( "Sans Serif",  8, 50, false ) ),
#endif
      m_fontascent( QFontMetrics( m_font_regular ).ascent() ),
      m_pen( Qt::red ),
      m_addMeasurePointAction( 0 ),
      m_removeLastMeasurePointAction( 0 ),
      m_removeMeasurePointsAction( 0 ),
      m_separator( 0 ),
      m_marbleWidget( 0 ),
      m_configDialog( 0 ),
      m_uiConfigWidget( 0 ),
      m_showDistanceLabel( true ),
      m_showBearingLabel( true ),
      m_showBearingChangeLabel( true )
{
    m_pen.setWidthF( 2.0 );
}

QStringList MeasureToolPlugin::backendTypes() const
{
    return QStringList( "measuretool" );
}

QString MeasureToolPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList MeasureToolPlugin::renderPosition() const
{
    return QStringList() << "USER_TOOLS";
}

QString MeasureToolPlugin::name() const
{
    return tr( "Measure Tool" );
}

QString MeasureToolPlugin::guiString() const
{
    return tr( "&Measure Tool" );
}

QString MeasureToolPlugin::nameId() const
{
    return QString( "measure-tool" );
}

QString MeasureToolPlugin::version() const
{
    return "1.0";
}

QString MeasureToolPlugin::description() const
{
    return tr( "Measure distances between two or more points." );
}

QString MeasureToolPlugin::copyrightYears() const
{
    return "2006-2008, 2011";
}

QList<PluginAuthor> MeasureToolPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" )
            << PluginAuthor( "Torsten Rahn", "tackat@kde.org" )
            << PluginAuthor( "Inge Wallin", "ingwa@kde.org" )
            << PluginAuthor( "Carlos Licea", "carlos.licea@kdemail.net" )
            << PluginAuthor( "Michael Henning", "mikehenning@eclipse.net" )
            << PluginAuthor( "Valery Kharitonov", "kharvd@gmail.com" )
            << PluginAuthor( "Mohammed Nafees", "nafees.technocool@gmail.com" )
            << PluginAuthor( "Illya Kovalevskyy", "illya.kovalevskyy@gmail.com" );
}

QIcon MeasureToolPlugin::icon () const
{
    return QIcon(":/icons/measure.png");
}

void MeasureToolPlugin::initialize ()
{
}

bool MeasureToolPlugin::isInitialized () const
{
    return true;
}

QDialog *MeasureToolPlugin::configDialog()
{
    if ( !m_configDialog ) {
        m_configDialog = new QDialog();
        m_uiConfigWidget = new Ui::MeasureConfigWidget;
        m_uiConfigWidget->setupUi( m_configDialog );
        connect( m_uiConfigWidget->m_buttonBox, SIGNAL(accepted()),
                SLOT(writeSettings()) );
        QPushButton *applyButton = m_uiConfigWidget->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL(clicked()),
                 this,        SLOT(writeSettings()) );
    }

    m_uiConfigWidget->m_showDistanceLabelsCheckBox->setChecked( m_showDistanceLabel );
    m_uiConfigWidget->m_showBearingLabelsCheckBox->setChecked( m_showBearingLabel );
    m_uiConfigWidget->m_showBearingLabelChangeCheckBox->setChecked( m_showBearingChangeLabel );
    m_uiConfigWidget->m_modeCombo->setCurrentIndex( (int)m_paintMode );

    return m_configDialog;
}

QHash<QString,QVariant> MeasureToolPlugin::settings() const
{
    QHash<QString, QVariant> settings = RenderPlugin::settings();

    settings.insert( "showDistanceLabel", m_showDistanceLabel );
    settings.insert( "showBearingLabel", m_showBearingLabel );
    settings.insert( "showBearingChangeLabel", m_showBearingChangeLabel);
    settings.insert( "paintMode", (int)m_paintMode );

    return settings;
}

void MeasureToolPlugin::setSettings( const QHash<QString,QVariant> &settings )
{
    RenderPlugin::setSettings( settings );

    m_showDistanceLabel = settings.value( "showDistanceLabel", true ).toBool();
    m_showBearingLabel = settings.value( "showBearingLabel", true ).toBool();
    m_showBearingChangeLabel = settings.value( "showBearingChangeLabel", true ).toBool();
    m_paintMode = (PaintMode)settings.value( "paintMode", 0 ).toInt();
}

void MeasureToolPlugin::writeSettings()
{
    m_showDistanceLabel = m_uiConfigWidget->m_showDistanceLabelsCheckBox->isChecked();
    m_showBearingLabel = m_uiConfigWidget->m_showBearingLabelsCheckBox->isChecked();
    m_showBearingChangeLabel = m_uiConfigWidget->m_showBearingLabelChangeCheckBox->isChecked();
    m_paintMode = (PaintMode)m_uiConfigWidget->m_modeCombo->currentIndex();

    if (m_paintMode == Circular) {
        if (m_measureLineString.size() < 2) {
            m_addMeasurePointAction->setEnabled(true);
        } else {
            m_addMeasurePointAction->setEnabled(false);
            while (m_measureLineString.size() > 2)
                m_measureLineString.remove(m_measureLineString.size()-1);
        }
    } else {
        m_addMeasurePointAction->setEnabled(true);
    }

    emit settingsChanged( nameId() );
    emit repaintNeeded();
}

bool MeasureToolPlugin::render( GeoPainter *painter,
                          ViewportParams *viewport,
                          const QString& renderPos,
                          GeoSceneLayer * layer )
{
    Q_UNUSED(renderPos)
    Q_UNUSED(layer)

    m_latLonAltBox = viewport->viewLatLonAltBox();

    // No way to paint anything if the list is empty.
    if ( m_measureLineString.isEmpty() )
        return true;

    painter->save();

    // Prepare for painting the measure line string and paint it.
    painter->setPen( m_pen );

    if ( m_showDistanceLabel || m_showBearingLabel || m_showBearingChangeLabel ) {
        drawSegments( painter );
    } else {
        painter->drawPolyline( m_measureLineString );
    }

    // Paint the nodes of the paths.
    drawMeasurePoints( painter );

    // Paint the total distance in the upper left corner.
    qreal totalDistance = m_measureLineString.length( marbleModel()->planet()->radius() );

    if ( m_measureLineString.size() > 1 )
        drawTotalDistanceLabel( painter, totalDistance );

    painter->restore();

    return true;
}

void MeasureToolPlugin::drawSegments( GeoPainter* painter )
{
    for ( int segmentIndex = 0; segmentIndex < m_measureLineString.size() - 1; ++segmentIndex ) {
        GeoDataLineString segment( Tessellate );
        segment << m_measureLineString[segmentIndex] ;
        segment << m_measureLineString[segmentIndex + 1];

        QPen shadowPen( Oxygen::aluminumGray5 );
        shadowPen.setWidthF(4.0);
        painter->setPen( shadowPen );
        painter->drawPolyline( segment );

        QString infoString;

        if ( m_showDistanceLabel ) {

            const MarbleLocale::MeasurementSystem measurementSystem =
                    MarbleGlobal::getInstance()->locale()->measurementSystem();

            const qreal segmentLength = segment.length( marbleModel()->planet()->radius() );

            if ( measurementSystem == MarbleLocale::MetricSystem ) {
                if ( segmentLength >= 1000.0 ) {
                    infoString = tr("%1 km").arg( segmentLength / 1000.0, 0, 'f', 2 );
                }
                else {
                    infoString = tr("%1 m").arg( segmentLength, 0, 'f', 2 );
                }
            } else if (measurementSystem == MarbleLocale::ImperialSystem) {
                infoString = QString("%1 mi").arg( segmentLength / 1000.0 * KM2MI, 0, 'f', 2 );
            } else if (measurementSystem == MarbleLocale::NauticalSystem) {
                infoString = QString("%1 nm").arg( segmentLength / 1000.0 * KM2NM, 0, 'f', 2 );
            }
        }

        if ( m_showBearingLabel ) {
            GeoDataCoordinates coordinates = segment.first();
            qreal bearing = coordinates.bearing( segment.last(), GeoDataCoordinates::Degree );

            if ( bearing < 0 ) {
                bearing += 360;
            }
            QString bearingString = QString::fromUtf8( "%1°" ).arg( bearing, 0, 'f', 2 );
            if ( !infoString.isEmpty() ) {
                infoString.append( "\n" );
            }
            infoString.append( bearingString );
        }

        if ( m_showBearingChangeLabel && segmentIndex != 0 ) {
            GeoDataCoordinates currentCoordinates = m_measureLineString[segmentIndex];
            qreal currentBearing = currentCoordinates.bearing(m_measureLineString[segmentIndex+1]);
            qreal previousBearing = currentCoordinates.bearing( m_measureLineString[segmentIndex-1]);

            GeoDataLinearRing ring;
            painter->setPen( Qt::NoPen );
            painter->setBrush( QBrush ( QColor ( 127, 127, 127, 127 ) ) );

            if (currentBearing < previousBearing) currentBearing += 2 * M_PI;
            ring << currentCoordinates;

            qreal angleLength = qAbs(m_latLonAltBox.north() - m_latLonAltBox.south()) / 20;

            qreal iterBearing = previousBearing;
            while ( iterBearing < currentBearing ) {
                ring << currentCoordinates.moveByBearing( iterBearing, angleLength );;
                iterBearing += 0.1;
            }

            ring << currentCoordinates.moveByBearing( currentBearing, angleLength );;

            painter->drawPolygon( ring );

            qreal currentBearingChange = (currentBearing - previousBearing) * RAD2DEG;
            if (currentBearingChange < 0) currentBearingChange += 360;
            QString bearingChangedString = QString::fromUtf8( "%1°" ).arg( currentBearingChange, 0, 'f', 2 );
            painter->setPen( Qt::black );
            GeoDataCoordinates textPosition = ring.latLonAltBox().center();
            qreal deltaEast = ring.latLonAltBox().east() - currentCoordinates.longitude();
            qreal deltaWest = currentCoordinates.longitude() - ring.latLonAltBox().west();
            if (deltaEast > deltaWest) {
                textPosition.setLongitude(currentCoordinates.longitude() + deltaEast / 2);
            }
            else {
                textPosition.setLongitude(currentCoordinates.longitude() - deltaWest);
            }
            painter->drawText(textPosition, bearingChangedString );
       }

        // Drawing ellipse around 1st point towards the 2nd
        if ( m_paintMode == Circular ) {
            GeoDataCoordinates currentCoordinates = m_measureLineString[segmentIndex];

            GeoDataLinearRing ring;

            // R - planet radius
            // d - distance between points
            // S - area of the painted circle
            qreal R = marbleModel()->planet()->radius();
            qreal d = m_measureLineString.length(1);
            qreal S = 2 * M_PI * R*R * (1 - qCos(d));

            qreal iterBearing = 0;
            while ( iterBearing < 2 * M_PI ) {
                ring << currentCoordinates.moveByBearing(iterBearing, d);
                iterBearing += 0.1;
            }

            painter->setPen( Qt::NoPen );
            painter->setBrush( QBrush ( QColor ( 127, 127, 127, 127 ) ) );
            painter->drawPolygon(ring);

            painter->setPen(Qt::white);
            GeoDataCoordinates textPosition = ring.latLonAltBox().center();
            painter->drawText(textPosition, tr("Circular area: %1 km^2").arg(S/1000000));
        }

        if ( !infoString.isEmpty() ) {
            QPen linePen;

            // have three alternating colors for the segments
            switch ( segmentIndex % 3 ) {
            case 0:
                linePen.setColor( Oxygen::brickRed4 );
                break;
            case 1:
                linePen.setColor( Oxygen::forestGreen4 );
                break;
            case 2:
                linePen.setColor( Oxygen::skyBlue4 );
                break;
            }

            linePen.setWidthF(2.0);
            painter->setPen( linePen );
            painter->drawPolyline( segment, infoString, LineCenter );
        }
    }
}

void MeasureToolPlugin::drawMeasurePoints( GeoPainter *painter ) const
{
    // Paint the marks.
    GeoDataLineString::const_iterator itpoint = m_measureLineString.constBegin();
    GeoDataLineString::const_iterator const endpoint = m_measureLineString.constEnd();
    for (; itpoint != endpoint; ++itpoint )
    {
        painter->drawPixmap( *itpoint, m_mark );
    }
}

void MeasureToolPlugin::drawTotalDistanceLabel( GeoPainter *painter,
                                          qreal totalDistance ) const
{
    QString  distanceString;

    MarbleLocale::MeasurementSystem measurementSystem;
    measurementSystem = MarbleGlobal::getInstance()->locale()->measurementSystem();

    if ( measurementSystem == MarbleLocale::MetricSystem ) {
        if ( totalDistance >= 1000.0 ) {
            distanceString = tr("Total Distance: %1 km").arg( totalDistance/1000.0 );
        }
        else {
            distanceString = tr("Total Distance: %1 m").arg( totalDistance );
        }
    }
    else if (measurementSystem == MarbleLocale::ImperialSystem) {
        distanceString = QString("Total Distance: %1 mi").arg( totalDistance/1000.0 * KM2MI );
    } else if (measurementSystem == MarbleLocale::NauticalSystem) {
        distanceString = QString("Total Distance: %1 nm").arg( totalDistance/1000.0 * KM2NM );
    }

    painter->setPen( QColor( Qt::black ) );
    painter->setBrush( QColor( 192, 192, 192, 192 ) );

    painter->drawRect( 10, 105, 10 + QFontMetrics( m_font_regular ).boundingRect( distanceString ).width() + 5, 10 + m_fontascent + 2 );
    painter->setFont( m_font_regular );
    painter->drawText( 15, 110 + m_fontascent, distanceString );
}


void MeasureToolPlugin::addMeasurePoint( qreal lon, qreal lat )
{
    m_measureLineString << GeoDataCoordinates( lon, lat );

    emit numberOfMeasurePointsChanged( m_measureLineString.size() );
}

void MeasureToolPlugin::removeLastMeasurePoint()
{
    if (!m_measureLineString.isEmpty())
	m_measureLineString.remove( m_measureLineString.size() - 1 );

    emit numberOfMeasurePointsChanged( m_measureLineString.size() );
}

void MeasureToolPlugin::removeMeasurePoints()
{
    m_measureLineString.clear();

    emit numberOfMeasurePointsChanged( m_measureLineString.size() );
}

void MeasureToolPlugin::addContextItems()
{
    MarbleWidgetPopupMenu *menu = m_marbleWidget->popupMenu();

    // Connect the inputHandler and the measure tool to the popup menu
    m_addMeasurePointAction = new QAction( QIcon(":/icons/measure.png"), tr( "Add &Measure Point" ), this );
    m_removeLastMeasurePointAction = new QAction( tr( "Remove &Last Measure Point" ), this );
    m_removeLastMeasurePointAction->setEnabled( false );
    m_removeMeasurePointsAction = new QAction( tr( "&Remove Measure Points" ), this );
    m_removeMeasurePointsAction->setEnabled( false );
    m_separator = new QAction( this );
    m_separator->setSeparator( true );

    if ( ! MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        menu->addAction( Qt::RightButton, m_addMeasurePointAction );
        menu->addAction( Qt::RightButton, m_removeLastMeasurePointAction );
        menu->addAction( Qt::RightButton, m_removeMeasurePointsAction );
        menu->addAction( Qt::RightButton, m_separator );
    }

    connect( m_addMeasurePointAction, SIGNAL(triggered()), SLOT(addMeasurePointEvent()) );
    connect( m_removeLastMeasurePointAction, SIGNAL(triggered()), SLOT(removeLastMeasurePoint()) );
    connect( m_removeMeasurePointsAction, SIGNAL(triggered()), SLOT(removeMeasurePoints()) );

    connect( this, SIGNAL(numberOfMeasurePointsChanged(int)), SLOT(setNumberOfMeasurePoints(int)) );
}

void MeasureToolPlugin::removeContextItems()
{
    delete m_addMeasurePointAction;
    delete m_removeLastMeasurePointAction;
    delete m_removeMeasurePointsAction;
    delete m_separator;
}

void MeasureToolPlugin::addMeasurePointEvent()
{
    QPoint p = m_marbleWidget->popupMenu()->mousePosition();

    qreal  lat;
    qreal  lon;
    m_marbleWidget->geoCoordinates( p.x(), p.y(), lon, lat, GeoDataCoordinates::Radian );

    addMeasurePoint( lon, lat );
}

void MeasureToolPlugin::setNumberOfMeasurePoints( int newNumber )
{
    const bool enableMeasureActions = ( newNumber > 0 );
    m_removeMeasurePointsAction->setEnabled(enableMeasureActions);
    m_removeLastMeasurePointAction->setEnabled(enableMeasureActions);

    if (m_paintMode == Circular) {
        if (newNumber >= 2) {
            m_addMeasurePointAction->setEnabled(false);
        } else {
            m_addMeasurePointAction->setEnabled(true);
        }
    }
}

bool MeasureToolPlugin::eventFilter( QObject *object, QEvent *e )
{
    if ( m_marbleWidget && !enabled() ) {
        m_marbleWidget = 0;
        removeContextItems();
        m_measureLineString.clear();
    }

    if ( m_marbleWidget || !enabled() || !visible() ) {
        return RenderPlugin::eventFilter( object, e );
    }

    MarbleWidget *widget = qobject_cast<MarbleWidget*>( object );

    if ( widget ) {
        m_marbleWidget = widget;
        addContextItems();
    }

    return RenderPlugin::eventFilter( object, e );
}

}

Q_EXPORT_PLUGIN2( MeasureToolPlugin, Marble::MeasureToolPlugin )

#include "MeasureToolPlugin.moc"

