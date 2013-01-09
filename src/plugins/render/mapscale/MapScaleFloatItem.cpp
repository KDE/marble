//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

#include "MapScaleFloatItem.h"

#include <QtCore/QRect>
#include <QtGui/QPixmap>
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>
#include <QtGui/QMenu>
#include <QtGui/QToolTip>

#include "ui_MapScaleConfigWidget.h"
#include "MarbleDebug.h"
#include "MarbleGlobal.h"
#include "Projections/AbstractProjection.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "ViewportParams.h"

namespace Marble
{

MapScaleFloatItem::MapScaleFloatItem()
    : AbstractFloatItem( 0 ),
      m_configDialog( 0 ),
      ui_configWidget( 0 )
{
}

MapScaleFloatItem::MapScaleFloatItem( const MarbleModel *marbleModel )
    : AbstractFloatItem( marbleModel, QPointF( 10.5, -10.5 ), QSizeF( 0.0, 40.0 ) ),
      m_configDialog(0),
      m_radius(0),
      m_target(QString()),
      m_leftBarMargin(0),
      m_rightBarMargin(0),
      m_scaleBarWidth(0),
      m_viewportWidth(0),
      m_scaleBarHeight(5),
      m_scaleBarDistance(0.0),
      m_bestDivisor(0),
      m_pixelInterval(0),
      m_valueInterval(0),
      m_unit(tr("km")),
      m_scaleInitDone( false ),
      m_showRatioScale( false ),
      m_contextMenu( 0 )
{
#ifdef Q_WS_MAEMO_5
        setPosition( QPointF( 220.0, 10.5 ) );
#endif // Q_WS_MAEMO_5
}

MapScaleFloatItem::~MapScaleFloatItem()
{
}

QStringList MapScaleFloatItem::backendTypes() const
{
    return QStringList( "mapscale" );
}

QString MapScaleFloatItem::name() const
{
    return tr("Scale Bar");
}

QString MapScaleFloatItem::guiString() const
{
    return tr("&Scale Bar");
}

QString MapScaleFloatItem::nameId() const
{
    return QString( "scalebar" );
}

QString MapScaleFloatItem::version() const
{
    return "1.1";
}

QString MapScaleFloatItem::description() const
{
    return tr("This is a float item that provides a map scale.");
}

QString MapScaleFloatItem::copyrightYears() const
{
    return "2008, 2010";
}

QList<PluginAuthor> MapScaleFloatItem::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Torsten Rahn", "tackat@kde.org", tr( "Original Developer" ) )
            << PluginAuthor( "Khanh-Nhan Nguyen", "khanh.nhan@wpi.edu" );
}

QIcon MapScaleFloatItem::icon () const
{
    return QIcon(":/icons/scalebar.png");
}


void MapScaleFloatItem::initialize ()
{
}

bool MapScaleFloatItem::isInitialized () const
{
    return true;
}

void MapScaleFloatItem::changeViewport( ViewportParams *viewport )
{
    int viewportWidth = viewport->width();

    QString target = marbleModel()->planetId();

    if ( !(    m_radius == viewport->radius()
            && viewportWidth == m_viewportWidth
            && m_target == target
            && m_scaleInitDone ) )
    {
        int fontHeight     = QFontMetrics( font() ).ascent();
        if (m_showRatioScale) {
            setContentSize( QSizeF( viewport->width() / 2,
                                    fontHeight + 3 + m_scaleBarHeight + fontHeight + 7 ) );
        } else {
            setContentSize( QSizeF( viewport->width() / 2,
                                    fontHeight + 3 + m_scaleBarHeight ) );
        }

        m_leftBarMargin  = QFontMetrics( font() ).boundingRect( "0" ).width() / 2;
        m_rightBarMargin = QFontMetrics( font() ).boundingRect( "0000" ).width() / 2;

        m_scaleBarWidth = contentSize().width() - m_leftBarMargin - m_rightBarMargin;
        m_viewportWidth = viewport->width();
        m_radius = viewport->radius();
        m_scaleInitDone = true;

        m_pixel2Length = marbleModel()->planetRadius() /
                             (qreal)(viewport->radius());

        if ( viewport->currentProjection()->surfaceType() == AbstractProjection::Cylindrical )
        {
            qreal centerLatitude = viewport->viewLatLonAltBox().center().latitude();
            // For flat maps we calculate the length of the 90 deg section of the
            // central latitude circle. For flat maps this distance matches
            // the pixel based radius propertyy.
            m_pixel2Length *= M_PI / 2 * cos( centerLatitude );
        }

        update();
    }
}

void MapScaleFloatItem::paintContent( QPainter *painter )
{
    painter->save();

    painter->setRenderHint( QPainter::Antialiasing, true );

    int fontHeight     = QFontMetrics( font() ).ascent();

    //calculate scale ratio
    qreal displayMMPerPixel = 1.0 * painter->device()->widthMM() / painter->device()->width();
    qreal ratio = m_pixel2Length / (displayMMPerPixel * MM2M);

    //round ratio to 3 most significant digits, assume that ratio >= 1, otherwise it may display "1 : 0"
    //i made this assumption because as the primary use case we do not need to zoom in that much
    qreal power = 1;
    int iRatio = (int)(ratio + 0.5); //round ratio to the nearest integer
    while (iRatio >= 1000) {
        iRatio /= 10;
        power *= 10;
    }
    iRatio *= power;
    m_ratioString.setNum(iRatio);
    m_ratioString = m_ratioString = "1 : " + m_ratioString;

    m_scaleBarDistance = (qreal)(m_scaleBarWidth) * m_pixel2Length;

    const QLocale::MeasurementSystem measurementSystem = MarbleGlobal::getInstance()->locale()->measurementSystem();

    if ( measurementSystem == QLocale::ImperialSystem ) {
        m_scaleBarDistance *= KM2MI;
    }

    calcScaleBar();

    painter->setPen(   QColor( Qt::darkGray ) );
    painter->setBrush( QColor( Qt::darkGray ) );
    painter->drawRect( m_leftBarMargin, fontHeight + 3,
                       m_scaleBarWidth,
                       m_scaleBarHeight );

    painter->setPen(   QColor( Qt::black ) );
    painter->setBrush( QColor( Qt::white ) );
    painter->drawRect( m_leftBarMargin, fontHeight + 3,
                       m_bestDivisor * m_pixelInterval, m_scaleBarHeight );

    painter->setPen(   QColor( Oxygen::aluminumGray4 ) );
    painter->drawLine( m_leftBarMargin + 1, fontHeight + 2 + m_scaleBarHeight,
                       m_leftBarMargin + m_bestDivisor * m_pixelInterval - 1, fontHeight + 2 + m_scaleBarHeight );
    painter->setPen(   QColor( Qt::black ) );

    painter->setBrush( QColor( Qt::black ) );

    QString  intervalStr;
    int      lastStringEnds     = 0;
    int      currentStringBegin = 0;

    for ( int j = 0; j <= m_bestDivisor; j += 2 ) {
        if ( j < m_bestDivisor ) {
            painter->drawRect( m_leftBarMargin + j * m_pixelInterval,
                               fontHeight + 3, m_pixelInterval - 1,
                               m_scaleBarHeight );

	    painter->setPen(   QColor( Oxygen::aluminumGray5 ) );
	    painter->drawLine( m_leftBarMargin + j * m_pixelInterval + 1, fontHeight + 4,
			       m_leftBarMargin + (j + 1) * m_pixelInterval - 1, fontHeight + 4 );
	    painter->setPen(   QColor( Qt::black ) );
        }

        QLocale::MeasurementSystem distanceUnit;
        distanceUnit = MarbleGlobal::getInstance()->locale()->measurementSystem();

        switch ( distanceUnit ) {
        case QLocale::MetricSystem:
            if ( m_bestDivisor * m_valueInterval > 10000 ) {
                m_unit = tr("km");
                intervalStr.setNum( j * m_valueInterval / 1000 );
            }
            else {
                m_unit = tr("m");
                intervalStr.setNum( j * m_valueInterval );
            }
            break;

        case QLocale::ImperialSystem:
            m_unit = tr("mi");
            intervalStr.setNum( j * m_valueInterval / 1000 );

            if ( m_bestDivisor * m_valueInterval > 3800 ) {
                intervalStr.setNum( j * m_valueInterval / 1000 );
            }
            else {
                intervalStr.setNum( qreal(j * m_valueInterval ) / 1000.0, 'f', 2 );
            }
            break;
        }

        painter->setFont( font() );

        if ( j == 0 ) {
            painter->drawText( 0, fontHeight, "0 " + m_unit );
            lastStringEnds = QFontMetrics( font() ).width( "0 " + m_unit );
            continue;
        }

        if( j == m_bestDivisor ) {
            currentStringBegin = ( j * m_pixelInterval
                                   - QFontMetrics( font() ).boundingRect( intervalStr ).width() );
        }
        else {
            currentStringBegin = ( j * m_pixelInterval
                                   - QFontMetrics( font() ).width( intervalStr ) / 2 );
        }

        if ( lastStringEnds < currentStringBegin ) {
            painter->drawText( currentStringBegin, fontHeight, intervalStr );
            lastStringEnds = currentStringBegin + QFontMetrics( font() ).width( intervalStr );
        }
    }

    int leftRatioIndent = m_leftBarMargin + (m_scaleBarWidth - QFontMetrics( font() ).width(m_ratioString) ) / 2;
    painter->drawText( leftRatioIndent, fontHeight + 3 + m_scaleBarHeight + fontHeight + 5, m_ratioString );

    painter->restore();
}

void MapScaleFloatItem::calcScaleBar()
{
    qreal  magnitude = 1;

    // First we calculate the exact length of the whole area that is possibly
    // available to the scalebar in kilometers
    int  magValue = (int)( m_scaleBarDistance );

    // We calculate the two most significant digits of the km-scalebar-length
    // and store them in magValue.
    while ( magValue >= 100 ) {
        magValue  /= 10;
        magnitude *= 10;
    }

    m_bestDivisor = 4;
    int  bestMagValue = 1;

    for ( int i = 0; i < magValue; i++ ) {
        // We try to find the lowest divisor between 4 and 8 that
        // divides magValue without remainder.
        for ( int j = 4; j < 9; j++ ) {
            if ( ( magValue - i ) % j == 0 ) {
                // We store the very first result we find and store
                // m_bestDivisor and bestMagValue as a final result.
                m_bestDivisor = j;
                bestMagValue  = magValue - i;

                // Stop all for loops and end search
                i = magValue;
                j = 9;
            }
        }

        // If magValue doesn't divide through values between 4 and 8
        // (e.g. because it's a prime number) try again with magValue
        // decreased by i.
    }

    m_pixelInterval = (int)( m_scaleBarWidth * (qreal)( bestMagValue )
                             / (qreal)( magValue ) / m_bestDivisor );
    m_valueInterval = (int)( bestMagValue * magnitude / m_bestDivisor );
}

QDialog *MapScaleFloatItem::configDialog()
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::MapScaleConfigWidget;
        ui_configWidget->setupUi( m_configDialog );

        readSettings();

        connect( ui_configWidget->m_buttonBox, SIGNAL(accepted()),
                                            SLOT(writeSettings()) );
        connect( ui_configWidget->m_buttonBox, SIGNAL(rejected()),
                                            SLOT(readSettings()) );
        QPushButton *applyButton = ui_configWidget->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL(clicked()) ,
                this,        SLOT(writeSettings()) );
    }
    return m_configDialog;
}

void MapScaleFloatItem::contextMenuEvent( QWidget *w, QContextMenuEvent *e )
{
    if ( !m_contextMenu ) {
        m_contextMenu = contextMenu();

        foreach( QAction *action, m_contextMenu->actions() ) {
            if ( action->text() == tr( "&Configure..." ) ) {
                m_contextMenu->removeAction( action );
                break;
            }
        }

        QAction *toggleAction = m_contextMenu->addAction( tr("&Ratio Scale"), this,
                                                SLOT(toggleRatioScaleVisibility()) );
        toggleAction->setCheckable( true );
        toggleAction->setChecked( m_showRatioScale );
    }

    Q_ASSERT( m_contextMenu );
    m_contextMenu->exec( w->mapToGlobal( e->pos() ) );
}

void MapScaleFloatItem::toolTipEvent( QHelpEvent *e )
{
    QToolTip::showText( e->globalPos(), m_ratioString );
}

void MapScaleFloatItem::readSettings()
{
    if ( !m_configDialog )
        return;

    if ( m_showRatioScale ) {
        ui_configWidget->m_showRatioScaleCheckBox->setCheckState( Qt::Checked );
    }
    else {
        ui_configWidget->m_showRatioScaleCheckBox->setCheckState( Qt::Unchecked );
    }
}

void MapScaleFloatItem::writeSettings()
{
    if ( ui_configWidget->m_showRatioScaleCheckBox->checkState() == Qt::Checked ) {
        m_showRatioScale = true;
    } else {
        m_showRatioScale = false;
    }

    emit settingsChanged( nameId() );
}

void MapScaleFloatItem::toggleRatioScaleVisibility()
{
    m_showRatioScale = !m_showRatioScale;
    readSettings();
    emit settingsChanged( nameId() );
}

}

Q_EXPORT_PLUGIN2(MapScaleFloatItem, Marble::MapScaleFloatItem)

#include "MapScaleFloatItem.moc"
