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

#include "AbstractProjection.h"
#include "GeoPainter.h"
#include "MarbleDebug.h"
#include "MarbleMath.h"
#include "MarbleModel.h"
#include "MarbleLocale.h"
#include "Planet.h"
#include "ViewportParams.h"

#include <QtGui/QColor>
#include <QtGui/QPen>
#include <QtGui/QPixmap>
#include <QtGui/QRadialGradient>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>

namespace Marble
{

MeasureToolPlugin::MeasureToolPlugin()
    : RenderPlugin( 0 ),
      m_configDialog( 0 ),
      m_uiConfigWidget( 0 )
{
}

MeasureToolPlugin::MeasureToolPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
      m_measureLineString( GeoDataLineString( Tessellate ) ),
#ifdef Q_OS_MACX
      m_font_regular( QFont( "Sans Serif", 10, 50, false ) ),
#else
      m_font_regular( QFont( "Sans Serif",  8, 50, false ) ),
#endif
      m_fontascent( QFontMetrics( m_font_regular ).ascent() ),
      m_pen( Qt::red ),
      m_marbleWidget( 0 ),
      m_configDialog( 0 ),
      m_uiConfigWidget( 0 ),
      m_showSegmentLabels( false )
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
            << PluginAuthor( "Mohammed Nafees", "nafees.technocool@gmail.com" );
}

QIcon MeasureToolPlugin::icon () const
{
    return QIcon();
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
        connect( m_uiConfigWidget->m_buttonBox, SIGNAL( accepted() ),
                SLOT( writeSettings() ) );
        QPushButton *applyButton = m_uiConfigWidget->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL( clicked() ),
                 this,        SLOT( writeSettings() ) );
    }

    m_uiConfigWidget->m_showSegLabelsCheckBox->setChecked( m_showSegmentLabels );

    return m_configDialog;
}

QHash<QString,QVariant> MeasureToolPlugin::settings() const
{
    QHash<QString, QVariant> settings;

    settings.insert( "showSegmentLabels", m_showSegmentLabels );

    return settings;
}

void MeasureToolPlugin::setSettings( const QHash<QString,QVariant> &settings )
{
    m_showSegmentLabels = settings.value( "showSegmentLabels", 0 ).toBool();
}

void MeasureToolPlugin::writeSettings()
{
    m_showSegmentLabels = m_uiConfigWidget->m_showSegLabelsCheckBox->isChecked();

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

    // FIXME: Add this stuff into the Layermanager as something to be
    // called before the float items.

    // No way to paint anything if the list is empty.
    if ( m_measureLineString.isEmpty() )
        return true;

    // Prepare for painting the measure line string and paint it.
    painter->autoMapQuality();
    painter->setPen( m_pen );

    if ( m_showSegmentLabels ) {
        drawSegments( painter );
    } else {
        painter->drawPolyline( m_measureLineString );
    }

    // Paint the nodes of the paths.
    drawMeasurePoints( painter, viewport );

    // Paint the total distance in the upper left corner.
    qreal totalDistance = m_measureLineString.length( marbleModel()->planet()->radius() );

    if ( m_measureLineString.size() > 1 )
        drawTotalDistanceLabel( painter, totalDistance );

    return true;
}

void MeasureToolPlugin::drawSegments( GeoPainter* painter )
{
    // Temporary container for each segment of the line string
    GeoDataLineString segment( Tessellate );

    int r = 0, g = 1, b = 2;
    for ( int i = 0; i < m_measureLineString.size() - 1; i++ ) {
        segment << m_measureLineString[i] ;
        segment << m_measureLineString[i + 1];

        qreal segmentLength = segment.length( marbleModel()->planet()->radius() );

        QString distanceString;

        QLocale::MeasurementSystem measurementSystem;
        measurementSystem = MarbleGlobal::getInstance()->locale()->measurementSystem();

        QPen shadowPen( Oxygen::aluminumGray5 );
        shadowPen.setWidthF(4.0);
        painter->setPen( shadowPen );
        painter->drawPolyline( segment, distanceString, LineCenter );

        QPen linePen;

        if ( measurementSystem == QLocale::MetricSystem ) {
            if ( segmentLength >= 1000.0 ) {
                distanceString = tr("%1 km").arg( segmentLength / 1000.0, 0, 'f', 2 );
            }
            else {
                distanceString = tr("%1 m").arg( segmentLength, 0, 'f', 2 );
            }
        }
        else {
            distanceString = QString("%1 mi").arg( segmentLength / 1000.0 * KM2MI, 0, 'f', 2 );
        }

        if ( i == r ) {
            linePen.setColor( Oxygen::brickRed4 );
            r+=3;
        }
        else if ( i == g ) {
            linePen.setColor( Oxygen::forestGreen4 );
            g+=3;
        }
        else if ( i == b ) {
            linePen.setColor( Oxygen::skyBlue4 );
            b+=3;
        }
	linePen.setWidthF(2.0);
	painter->setPen( linePen );
        painter->drawPolyline( segment, distanceString, LineCenter );

        segment.clear();
    }
}

void MeasureToolPlugin::drawMeasurePoints( GeoPainter *painter,
                                      ViewportParams *viewport )
{
    qreal  y = 0.0;

    // Paint the marks.
    GeoDataLineString::const_iterator itpoint = m_measureLineString.constBegin();
    GeoDataLineString::const_iterator const endpoint = m_measureLineString.constEnd();
    for (; itpoint != endpoint; ++itpoint )
    {
        qreal  lon;
        qreal  lat;

        itpoint->geoCoordinates( lon, lat );

        // FIXME: Replace all of this by some appropriate drawPlaceMark( GeoDataCrossHairs )
        //        or drawPlaceMark( GeoDataPlacemark ) method
        int pointRepeatNum = 0;
        bool globeHidesPoint = false;
        qreal * x  = new qreal[100];

        bool visible = viewport->screenCoordinates( GeoDataCoordinates( lon, lat ), x, y, pointRepeatNum, globeHidesPoint );

        if ( visible ) {
            // Draw all the x-repeat-instances of the point on the screen
            for( int it = 0; it < pointRepeatNum; ++it ) {
                drawMark( painter, x[it], y );
            }
        }

        delete[] x;
    }
}

void MeasureToolPlugin::drawMark( GeoPainter* painter, int x, int y )
{
    const int markRadius = 5;

    // Paint the mark, and repeat it if the projection allows it.
    painter->setRenderHint( QPainter::Antialiasing, false );

    QColor backgroundCircleColor( Oxygen::aluminumGray6 );
    backgroundCircleColor.setAlpha( 128 );

    painter->setBrush( backgroundCircleColor );
    painter->setPen( Qt::NoPen );
    painter->drawEllipse( x-8, y-8, 16, 16 );

    painter->setPen( QColor( Qt::white ) );
    painter->drawLine( x - markRadius, y, x + markRadius, y );
    painter->drawLine( x, y - markRadius, x, y + markRadius );
}

void MeasureToolPlugin::drawTotalDistanceLabel( GeoPainter *painter,
                                          qreal totalDistance )
{
    QString  distanceString;

    QLocale::MeasurementSystem measurementSystem;
    measurementSystem = MarbleGlobal::getInstance()->locale()->measurementSystem();

    if ( measurementSystem == QLocale::MetricSystem ) {
        if ( totalDistance >= 1000.0 ) {
            distanceString = tr("Total Distance: %1 km").arg( totalDistance/1000.0 );
        }
        else {
            distanceString = tr("Total Distance: %1 m").arg( totalDistance );
        }
    }
    else {
        distanceString = QString("Total Distance: %1 mi").arg( totalDistance/1000.0 * KM2MI );
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
    m_addMeasurePointAction = new QAction( tr( "Add &Measure Point" ), this );
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

    connect( m_addMeasurePointAction, SIGNAL( triggered() ), SLOT( addMeasurePointEvent() ) );
    connect( m_removeLastMeasurePointAction, SIGNAL(triggered() ), SLOT( removeLastMeasurePoint() ) );
    connect( m_removeMeasurePointsAction, SIGNAL( triggered() ), SLOT( removeMeasurePoints() ) );

    connect( this, SIGNAL( numberOfMeasurePointsChanged( int ) ), SLOT( setNumberOfMeasurePoints( int ) ) );
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

