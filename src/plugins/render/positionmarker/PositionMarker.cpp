//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
// Copyright 2009   Eckhart Wörner  <ewoerner@kde.org>
// Copyright 2010   Thibaut Gridel  <tgridel@free.fr>
// Copyright 2010   Daniel Marth    <danielmarth@gmx.at>
//

#include "PositionMarker.h"

#include "MarbleDebug.h"
#include <QtCore/QRect>
#include <QtGui/QColor>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
 
#include "ui_PositionMarkerConfigWidget.h"
#include "AbstractProjection.h"
#include "MarbleDataFacade.h"
#include "GeoPainter.h"
#include "PositionTracking.h"
#include "ViewportParams.h"

namespace Marble
{

PositionMarker::PositionMarker ()
    : RenderPlugin(),
      m_isInitialized( false ),
      m_useCustomCursor( false ),
      ui_configWidget( 0 ),
      m_aboutDialog( 0 ),
      m_configDialog( 0 ),
      m_viewport( 0 )
{
}

PositionMarker::~PositionMarker ()
{
}

QStringList PositionMarker::renderPosition() const
{
    return QStringList( "HOVERS_ABOVE_SURFACE" );
}

QString PositionMarker::renderPolicy() const
{
    return "ALWAYS";
}

QStringList PositionMarker::backendTypes() const
{
    return QStringList( "positionmarker" );
}

QString PositionMarker::name() const
{
    return tr( "Position Marker" );
}

QString PositionMarker::guiString() const
{
    return tr( "&Position Marker" );
}

QString PositionMarker::nameId() const
{
    return QString( "positionMarker" );
}

QString PositionMarker::description() const
{
    return tr( "draws a marker at the current position" );
}

QIcon PositionMarker::icon() const
{
    return QIcon();
}

QDialog *PositionMarker::aboutDialog() const
{
    if ( !m_aboutDialog ) {
        // Initializing about dialog
        m_aboutDialog = new PluginAboutDialog();
        m_aboutDialog->setName( "Position Marker Plugin" );
        m_aboutDialog->setVersion( "0.1" );
        // FIXME: Can we store this string for all of Marble
        m_aboutDialog->setAboutText( tr( "<br />(c) 2009, 2010 The Marble Project<br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>" ) );
        QList<Author> authors;
        Author amanson, ewoerner, tgridel, dmarth;
        amanson.name = "Andrew Manson";
        amanson.task = tr( "Developer" );
        amanson.email = "g.real.ate@gmail.com";
        ewoerner.name = "Eckhart Woerner";
        ewoerner.task = tr( "Developer" );
        ewoerner.email = "ewoerner@kde.org";
        tgridel.name = "Thibaut Gridel";
        tgridel.task = tr( "Developer" );
        tgridel.email = "tgridel@free.fr";
        dmarth.name = "Daniel Marth";
        dmarth.task = tr( "Developer" );
        dmarth.email = "danielmarth@gmx.at";
        authors.append( amanson );
        authors.append( ewoerner );
        authors.append( tgridel );
        authors.append( dmarth );
        m_aboutDialog->setAuthors( authors );
    }
    return m_aboutDialog;
}

QDialog *PositionMarker::configDialog() const
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::PositionMarkerConfigWidget;
        ui_configWidget->setupUi( m_configDialog );
        readSettings();
        connect( ui_configWidget->m_buttonBox, SIGNAL( accepted() ),
                 SLOT( writeSettings() ) );
        connect( ui_configWidget->m_buttonBox, SIGNAL( rejected() ),
                 SLOT( readSettings() ) );
        QPushButton *applyButton = ui_configWidget->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL( clicked() ),
                 SLOT( writeSettings() ) );
        connect( ui_configWidget->m_fileChooserButton, SIGNAL( clicked() ),
                 SLOT( chooseCustomCursor() ) );
    }
    return m_configDialog;
}

void PositionMarker::initialize()
{
    if ( dataFacade() ) {
        connect( dataFacade()->positionTracking(), SIGNAL( gpsLocation( GeoDataCoordinates,qreal ) ),
                this, SLOT( setPosition( GeoDataCoordinates ) ) );
        m_isInitialized = true;
    }
}

bool PositionMarker::isInitialized() const
{
    return m_isInitialized;
}

void PositionMarker::update()
{
    if ( !m_viewport ) {
        return;
    }
    if( ! ( m_currentPosition == m_previousPosition ) )
    {
        QPointF position;
        QPointF previousPosition;

        m_viewport->currentProjection()->screenCoordinates( m_currentPosition,
                                                            m_viewport,
                                                            position );
        m_viewport->currentProjection()->screenCoordinates( m_previousPosition,
                                                            m_viewport,
                                                            previousPosition );

        // calculate the arrow shape, oriented by the heading
        // and with constant size
        QPointF unitVector = position - previousPosition;
        // check that some screen progress was made
        if( unitVector.x() || unitVector.y() ) {
            // magnitude should be >0
            qreal magnitude = sqrt( ( unitVector.x() * unitVector.x() )
                                    + ( unitVector.y() * unitVector.y() ) );
            unitVector = unitVector / magnitude;
            QPointF unitVector2 = QPointF ( -unitVector.y(), unitVector.x() );
            QPointF relativeLeft = - ( unitVector * 9   ) + ( unitVector2 * 9 );
            QPointF relativeRight = - ( unitVector * 9 ) - ( unitVector2 * 9 );
            QPointF relativeTip =  unitVector * 19.0 ;

            m_arrow.clear();
            m_arrow << position
                    << position + relativeLeft
                    << position + relativeTip
                    << position + relativeRight;

            m_dirtyRegion = QRegion();
            m_dirtyRegion += ( m_arrow.boundingRect().toRect() );
            m_dirtyRegion += ( m_previousArrow.boundingRect().toRect() );
        }
    }
}

bool PositionMarker::render( GeoPainter *painter,
                           ViewportParams *viewport,
                           const QString& renderPos,
                           GeoSceneLayer * layer )
{
    Q_UNUSED( layer )
    bool const gpsActive = dataFacade()->positionTracking()->positionProviderPlugin() != 0;
    if ( gpsActive && renderPosition().contains(renderPos) )
    {
        if ( m_viewport != viewport ) {
            m_viewport = viewport;
        }
        update();
        painter->save();
        painter->autoMapQuality();

        GeoDataAccuracy accuracy = dataFacade()->positionTracking()->accuracy();
        if ( accuracy.horizontal > 0 && accuracy.horizontal < 1000 ) {
            // Paint a red circle indicating the position accuracy
            painter->setPen( Qt::transparent );
            QColor transparentRed = oxygenBrickRed4;
            int width = qRound( accuracy.horizontal * viewport->radius() / EARTH_RADIUS );
            if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
                transparentRed.setAlpha( 80 );
                int arrowSize = qMax<int>( m_arrow.boundingRect().width(), m_arrow.boundingRect().height() );
                width = qMax<int>( width, arrowSize + 10 );
            } else {
                transparentRed.setAlpha( 40 );
            }

            painter->setBrush( transparentRed );
            painter->drawEllipse( m_currentPosition, width, width );
        }

        if( m_useCustomCursor)
        {
            QRectF rect = m_arrow.boundingRect().toAlignedRect();
            if( rect.isValid() )
                painter->drawPixmap( rect.topLeft(), m_customCursor );
        }
        else
        {
            painter->setPen( Qt::black );
            painter->setBrush( Qt::white );
            painter->drawPolygon( m_arrow );
        }

        painter->restore();
        m_previousArrow = m_arrow;
    }
    return true;
}

void PositionMarker::readSettings() const
{
    if(m_useCustomCursor)
        ui_configWidget->m_customCursor->click();
    else
        ui_configWidget->m_originalCursor->click();
}

void PositionMarker::writeSettings()
{
    if( ui_configWidget->m_customCursor->isChecked() && 
        ui_configWidget->m_fileChooserButton->icon().isNull() )
    {
        QMessageBox::warning( NULL, tr( "Error" ), tr( "No cursor selected, the default cursor will be used." ), QMessageBox::Ok );
        m_useCustomCursor = false;
        readSettings();
        return;
    }
    m_useCustomCursor = ui_configWidget->m_customCursor->isChecked();
}

void PositionMarker::setPosition( const GeoDataCoordinates &position )
{
    m_previousPosition = m_currentPosition;
    m_currentPosition = position;
    if ( m_viewport->viewLatLonAltBox().contains( m_currentPosition ) )
    {
        emit repaintNeeded( m_dirtyRegion );
    }
}

void PositionMarker::chooseCustomCursor()
{
    QString filename = QFileDialog::getOpenFileName( NULL, tr( "Choose Custom Cursor" ) );
    if( !filename.isEmpty() )
        loadCustomCursor( filename );
}

int PositionMarker::loadCustomCursor( const QString& filename )
{
    m_customCursor = QPixmap( filename ).scaled( 22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation );
    if( !m_customCursor.isNull() )
    {
        ui_configWidget->m_customCursor->click();
        ui_configWidget->m_fileChooserButton->setIcon( QIcon( m_customCursor ) );
        return 0;
    }
    else
    {
        QMessageBox::warning( NULL, tr( "Error" ), tr( "Unable to load custom cursor. Make sure this is a valid image file." ), QMessageBox::Ok );
        ui_configWidget->m_fileChooserButton->setIcon( QIcon() );
        return 1;
    }
}

qreal PositionMarker::zValue() const
{
    return 1.0;
}

}

Q_EXPORT_PLUGIN2( PositionMarker, Marble::PositionMarker )

#include "PositionMarker.moc"
