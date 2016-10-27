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
#include "MeasureConfigDialog.h"

#include "GeoPainter.h"
#include "GeoDataLinearRing.h"
#include "MarbleDebug.h"
#include "MarbleMath.h"
#include "MarbleWidgetPopupMenu.h"
#include "MarbleModel.h"
#include "MarbleLocale.h"
#include "ViewportParams.h"
#include "Planet.h"

#include <QDialog>
#include <QColor>
#include <QTextDocument>
#include <qmath.h>

namespace Marble
{

MeasureToolPlugin::MeasureToolPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
      m_measureLineString( GeoDataLineString( Tessellate ) ),
#ifdef Q_OS_MACX
      m_font_regular( QFont( QStringLiteral( "Sans Serif" ), 10, 50, false ) ),
#else
      m_font_regular( QFont( QStringLiteral( "Sans Serif" ),  8, 50, false ) ),
#endif
      m_fontascent(-1),
      m_pen( Qt::red ),
      m_addMeasurePointAction( 0 ),
      m_removeLastMeasurePointAction( 0 ),
      m_removeMeasurePointsAction( 0 ),
      m_separator( 0 ),
      m_marbleWidget( 0 ),
      m_configDialog( 0 ),
      m_showDistanceLabel( true ),
      m_showBearingLabel( true ),
      m_showBearingChangeLabel( true ),
      m_showPolygonArea(false),
      m_showCircularArea(true),
      m_showRadius(true),
      m_showPerimeter(true),
      m_showCircumference(true),
      m_totalDistance(0.0),
      m_polygonArea(0.0),
      m_circularArea(0.0),
      m_radius(0.0),
      m_perimeter(0.0),
      m_circumference(0.0),
      m_paintMode(Polygon)
{
    m_pen.setWidthF( 2.0 );
}

QStringList MeasureToolPlugin::backendTypes() const
{
    return QStringList(QStringLiteral("measuretool"));
}

QString MeasureToolPlugin::renderPolicy() const
{
    return QStringLiteral("ALWAYS");
}

QStringList MeasureToolPlugin::renderPosition() const
{
    return QStringList(QStringLiteral("ATMOSPHERE"));
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
    return QStringLiteral("measure-tool");
}

QString MeasureToolPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString MeasureToolPlugin::description() const
{
    return tr( "Measure distances between two or more points." );
}

QString MeasureToolPlugin::copyrightYears() const
{
    return QStringLiteral("2006-2008, 2011");
}

QVector<PluginAuthor> MeasureToolPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"))
            << PluginAuthor(QStringLiteral("Torsten Rahn"), QStringLiteral("tackat@kde.org"))
            << PluginAuthor(QStringLiteral("Inge Wallin"), QStringLiteral("ingwa@kde.org"))
            << PluginAuthor(QStringLiteral("Carlos Licea"), QStringLiteral("carlos.licea@kdemail.net"))
            << PluginAuthor(QStringLiteral("Michael Henning"), QStringLiteral("mikehenning@eclipse.net"))
            << PluginAuthor(QStringLiteral("Valery Kharitonov"), QStringLiteral("kharvd@gmail.com"))
            << PluginAuthor(QStringLiteral("Mohammed Nafees"), QStringLiteral("nafees.technocool@gmail.com"))
            << PluginAuthor(QStringLiteral("Illya Kovalevskyy"), QStringLiteral("illya.kovalevskyy@gmail.com"));
}

QIcon MeasureToolPlugin::icon () const
{
    return QIcon(QStringLiteral(":/icons/measure.png"));
}

void MeasureToolPlugin::initialize ()
{
     m_fontascent = QFontMetrics( m_font_regular ).ascent();
}

bool MeasureToolPlugin::isInitialized () const
{
    return m_fontascent >= 0;
}

QDialog *MeasureToolPlugin::configDialog()
{
    if ( !m_configDialog ) {
        m_configDialog = new MeasureConfigDialog(m_configDialog);
        connect( m_configDialog, SIGNAL(accepted()),
                 SLOT(writeSettings()) );
        connect( m_configDialog, SIGNAL(applied()),
                 this, SLOT(writeSettings()) );
    }

    m_configDialog->setShowDistanceLabels( m_showDistanceLabel );
    m_configDialog->setShowBearingLabel( m_showBearingLabel );
    m_configDialog->setShowBearingLabelChange( m_showBearingChangeLabel );
    m_configDialog->setShowPolygonArea( m_showPolygonArea );
    m_configDialog->setShowCircularArea( m_showCircularArea );
    m_configDialog->setShowRadius( m_showRadius );
    m_configDialog->setShowPerimeter( m_showPerimeter );
    m_configDialog->setShowCircumference( m_showCircumference );
    m_configDialog->setPaintMode( m_paintMode );

    return m_configDialog;
}

QHash<QString,QVariant> MeasureToolPlugin::settings() const
{
    QHash<QString, QVariant> settings = RenderPlugin::settings();

    settings.insert(QStringLiteral("showDistanceLabel"), m_showDistanceLabel);
    settings.insert(QStringLiteral("showBearingLabel"), m_showBearingLabel);
    settings.insert(QStringLiteral("showBearingChangeLabel"), m_showBearingChangeLabel);
    settings.insert(QStringLiteral("showPolygonArea"), m_showPolygonArea);
    settings.insert(QStringLiteral("showCircularArea"), m_showCircularArea);
    settings.insert(QStringLiteral("showRadius"), m_showRadius);
    settings.insert(QStringLiteral("showPerimeter"), m_showPerimeter);
    settings.insert(QStringLiteral("showCircumference"), m_showCircumference);
    settings.insert(QStringLiteral("paintMode"), (int)m_paintMode);

    return settings;
}

void MeasureToolPlugin::setSettings( const QHash<QString,QVariant> &settings )
{
    RenderPlugin::setSettings( settings );

    m_showDistanceLabel = settings.value(QStringLiteral("showDistanceLabel"), true).toBool();
    m_showBearingLabel = settings.value(QStringLiteral("showBearingLabel"), true).toBool();
    m_showBearingChangeLabel = settings.value(QStringLiteral("showBearingChangeLabel"), true).toBool();
    m_showPolygonArea = settings.value(QStringLiteral("showPolygonArea"), false).toBool();
    m_showCircularArea = settings.value(QStringLiteral("showCircularArea"), true).toBool();
    m_showRadius = settings.value(QStringLiteral("showRadius"), true).toBool();
    m_showPerimeter = settings.value(QStringLiteral("showPerimeter"), true).toBool();
    m_showCircumference = settings.value(QStringLiteral("showCircumference"), true).toBool();
    m_paintMode = (PaintMode)settings.value(QStringLiteral("paintMode"), 0).toInt();
}

void MeasureToolPlugin::writeSettings()
{
    m_showDistanceLabel = m_configDialog->showDistanceLabels();
    m_showBearingLabel = m_configDialog->showBearingLabel();
    m_showBearingChangeLabel = m_configDialog->showBearingLabelChange();
    m_showPolygonArea = m_configDialog->showPolygonArea();
    m_showCircularArea = m_configDialog->showCircularArea();
    m_showRadius = m_configDialog->showRadius();
    m_showPerimeter = m_configDialog->showPerimeter();
    m_showCircumference = m_configDialog->showCircumference();
    m_paintMode = (PaintMode)m_configDialog->paintMode();

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

    m_totalDistance = m_measureLineString.length( marbleModel()->planet()->radius() );

    if ( m_measureLineString.size() > 1 )
        drawInfobox(painter);

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

        if ( (m_paintMode == Polygon && m_showDistanceLabel)
             || (m_paintMode == Circular && m_showRadius) ) {
            const qreal segmentLength = segment.length( marbleModel()->planet()->radius() );
            m_radius = segmentLength;

            infoString = meterToPreferredUnit(segmentLength);
        }

        if ( m_showBearingLabel && m_paintMode != Circular ) {
            GeoDataCoordinates coordinates = segment.first();
            qreal bearing = coordinates.bearing( segment.last(), GeoDataCoordinates::Degree );

            if ( bearing < 0 ) {
                bearing += 360;
            }
            QString bearingString = QString::fromUtf8( "%1°" ).arg( bearing, 0, 'f', 2 );
            if ( !infoString.isEmpty() ) {
                infoString += QLatin1Char('\n');
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

            // planetRadius - planet radius
            // d - distance between points
            // S - area of the painted circle
            qreal planetRadius = marbleModel()->planet()->radius();
            qreal d = m_measureLineString.length(1);
            m_circularArea = 2 * M_PI * planetRadius * planetRadius * (1 - qCos(d));

            qreal iterBearing = 0;
            while ( iterBearing < 2 * M_PI ) {
                ring << currentCoordinates.moveByBearing(iterBearing, d);
                iterBearing += 0.1;
            }

            painter->setPen( Qt::NoPen );
            painter->setBrush( QBrush ( QColor ( 127, 127, 127, 127 ) ) );
            painter->drawPolygon(ring);

            if ( m_showCircularArea ) {
                painter->setPen(Qt::white);
                GeoDataCoordinates textPosition = ring.latLonAltBox().center();

                QString areaText = tr("Area:\n%1").arg(meterToPreferredUnit(m_circularArea, true));

                QFontMetrics fontMetrics = painter->fontMetrics();
                QRect boundingRect = fontMetrics.boundingRect(QRect(), Qt::AlignCenter, areaText);

                painter->drawText(textPosition,
                                  areaText,
                                  -boundingRect.width()/2, -boundingRect.height()*1.5,
                                  boundingRect.width(), boundingRect.height(),
                                  QTextOption(Qt::AlignCenter));
            }

            if ( m_showCircumference ) {
                painter->setPen(Qt::white);
                GeoDataCoordinates textPosition = ring.latLonAltBox().center();

                m_circumference = 2 * M_PI * planetRadius * qSin(d);

                QString circumferenceText = tr("Circumference:\n%1").arg(meterToPreferredUnit(m_circumference));

                QFontMetrics fontMetrics = painter->fontMetrics();
                QRect boundingRect = fontMetrics.boundingRect(QRect(),Qt::AlignCenter,
                                                              circumferenceText);

                painter->drawText(textPosition,
                                  circumferenceText,
                                  -boundingRect.width()/2, boundingRect.height(),
                                  boundingRect.width(), boundingRect.height(),
                                  QTextOption(Qt::AlignCenter));
            }
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

    if (m_paintMode == Polygon && m_measureLineString.size() > 2) {
        GeoDataLinearRing measureRing(m_measureLineString);

        if (m_showPolygonArea || m_showPerimeter) {
            painter->setPen( Qt::NoPen );
            painter->setBrush( QBrush ( QColor ( 127, 127, 127, 127 ) ) );
            painter->drawPolygon(measureRing);

            QPen shadowPen( Oxygen::aluminumGray5 );
            shadowPen.setStyle(Qt::DashLine);
            shadowPen.setWidthF(3.0);
            painter->setPen( shadowPen );
            painter->drawPolyline(GeoDataLineString( Tessellate ) << m_measureLineString.first()
                                                      << m_measureLineString.last());
        }

        if (m_showPolygonArea) {
            qreal theta1 = 0.0;
            qreal n = m_measureLineString.size();

            for (int segmentIndex = 1; segmentIndex < m_measureLineString.size()-1; segmentIndex++) {
                GeoDataCoordinates current = m_measureLineString[segmentIndex];
                qreal prevBearing = current.bearing(m_measureLineString[segmentIndex-1]);
                qreal nextBearing = current.bearing(m_measureLineString[segmentIndex+1]);
                if (nextBearing < prevBearing)
                    nextBearing += 2 * M_PI;

                qreal angle = nextBearing - prevBearing;
                theta1 += angle;
            }

            // Traversing first vertex
            GeoDataCoordinates current = m_measureLineString[0];
            qreal prevBearing = current.bearing(m_measureLineString[n-1]);
            qreal nextBearing = current.bearing(m_measureLineString[1]);
            if (nextBearing < prevBearing)
                nextBearing += 2 * M_PI;
            qreal angle = nextBearing - prevBearing;
            theta1 += angle;

            // And the last one
            current = m_measureLineString[n-1];
            prevBearing = current.bearing(m_measureLineString[n-2]);
            nextBearing = current.bearing(m_measureLineString[0]);
            if (nextBearing < prevBearing)
                nextBearing += 2 * M_PI;
            angle = nextBearing - prevBearing;
            theta1 += angle;

            qreal theta2 = 2 * M_PI * n - theta1;

            // theta = smaller of theta1 and theta2
            qreal theta = (theta1 < theta2) ? theta1 : theta2;

            qreal planetRadius = marbleModel()->planet()->radius();
            qreal S = qAbs((theta - (n-2) * M_PI) * planetRadius * planetRadius);
            m_polygonArea = S;

            painter->setPen(Qt::white);
            GeoDataCoordinates textPosition = measureRing.latLonAltBox().center();

            QString areaText = tr("Area:\n%1").arg(meterToPreferredUnit(S, true));

            QFontMetrics fontMetrics = painter->fontMetrics();
            QRect boundingRect = fontMetrics.boundingRect(QRect(), Qt::AlignCenter, areaText);

            painter->drawText(textPosition,
                              areaText,
                              -boundingRect.width()/2, -(boundingRect.height()+fontMetrics.height()*0.25),
                              boundingRect.width(), boundingRect.height(),
                              QTextOption(Qt::AlignCenter));
        }

        if (m_showPerimeter) {
            painter->setPen(Qt::white);
            GeoDataCoordinates textPosition = measureRing.latLonAltBox().center();

            qreal P = measureRing.length(marbleModel()->planet()->radius());
            m_perimeter = P;
            QString perimeterText = tr("Perimeter:\n%1").arg(meterToPreferredUnit(P));

            QFontMetrics fontMetrics = painter->fontMetrics();
            QRect boundingRect = fontMetrics.boundingRect(QRect(),Qt::AlignCenter,
                                                          perimeterText);

            painter->drawText(textPosition,
                              perimeterText,
                              -boundingRect.width()/2, 0,
                              boundingRect.width(), boundingRect.height(),
                              QTextOption(Qt::AlignCenter));
        }
    }
}

QString MeasureToolPlugin::meterToPreferredUnit(qreal meters, bool isSquare) const
{
    MarbleLocale *locale = MarbleGlobal::getInstance()->locale();
    const MarbleLocale::MeasurementSystem measurementSystem = locale->measurementSystem();
    MarbleLocale::MeasureUnit unit;
    qreal convertedMeters;
    if (isSquare)
        meters = qSqrt(meters);

    locale->meterToTargetUnit(meters, measurementSystem, convertedMeters, unit);
    QString unitString = locale->unitAbbreviation(unit);

    if (isSquare) {
        qreal k = convertedMeters/meters;
        convertedMeters *= k;
        convertedMeters *= meters;

        unitString.append(QChar(0xB2));
    }

    return QString("%L1 %2").arg(convertedMeters, 8, 'f', 1, QLatin1Char(' '))
                            .arg(unitString);
}

void MeasureToolPlugin::drawMeasurePoints( GeoPainter *painter )
{
    // Paint the marks.
    GeoDataLineString::const_iterator itpoint = m_measureLineString.constBegin();
    GeoDataLineString::const_iterator const endpoint = m_measureLineString.constEnd();
    if (m_mark.isNull()) {
        m_mark = QPixmap(QStringLiteral(":/mark.png"));
    }
    for (; itpoint != endpoint; ++itpoint )
    {
        painter->drawPixmap( *itpoint, m_mark );
    }
}

void MeasureToolPlugin::drawInfobox( GeoPainter *painter ) const
{
    QString boxContent;

    if (m_paintMode == Polygon) {
        boxContent += QLatin1String("<strong>") + tr("Polygon Ruler") + QLatin1String(":</strong><br/>\n");
    } else /* Circular */ {
        boxContent += QLatin1String("<strong>") + tr("Circle Ruler") + QLatin1String(":</strong><br/>\n");
    }
    if (m_paintMode == Polygon) {
        boxContent += tr("Total Distance: %1<br/>\n").arg( meterToPreferredUnit(m_totalDistance) );
        if (m_showPolygonArea)
            boxContent += tr("Area: %1<br/>\n").arg( meterToPreferredUnit(m_polygonArea, true) );
        if (m_showPerimeter)
            boxContent += tr("Perimeter: %1<br/>\n").arg( meterToPreferredUnit(m_perimeter) );
    } else /* Circular */ {
        if (m_showRadius)
            boxContent += tr("Radius: %1<br/>\n").arg( meterToPreferredUnit(m_radius) );
        if (m_showCircumference)
            boxContent += tr("Circumference: %1<br/>\n").arg( meterToPreferredUnit(m_circumference) );
        if (m_showCircularArea)
            boxContent += tr("Area: %1<br/>\n").arg( meterToPreferredUnit(m_circularArea, true) );
    }

    painter->setPen( QColor( Qt::black ) );
    painter->setBrush( QColor( 192, 192, 192, 192 ) );

    QTextDocument doc;
    doc.setHtml(boxContent);
    doc.setDefaultFont(m_font_regular);
    doc.adjustSize();
    QSizeF pageSize = doc.size();

    painter->drawRect( 10, 105, 10 + pageSize.width(), pageSize.height() );
    QTransform transform;
    transform.translate(15, 110);
    painter->setTransform(transform);
    doc.drawContents(painter);
    painter->setTransform(QTransform());
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
    m_addMeasurePointAction = new QAction(QIcon(QStringLiteral(":/icons/measure.png")), tr("Add &Measure Point"), this);
    m_removeLastMeasurePointAction = new QAction( tr( "Remove &Last Measure Point" ), this );
    m_removeLastMeasurePointAction->setEnabled( false );
    m_removeMeasurePointsAction = new QAction( tr( "&Remove Measure Points" ), this );
    m_removeMeasurePointsAction->setEnabled( false );
    m_separator = new QAction( this );
    m_separator->setSeparator( true );

    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( !smallScreen ) {
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

#include "moc_MeasureToolPlugin.cpp"

