//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#include "NavigationFloatItem.h"

#include <QtCore/QRect>
#include <QtGui/QPixmap>
#include <QtGui/QToolButton>
#include <QtGui/QSlider>
#include <QtGui/QWidget>

#include "ui_navigation.h"
#include "ui_navigation_small.h"
#include "ViewportParams.h"
#include "MarbleDebug.h"
#include "MarbleWidget.h"
#include "WidgetGraphicsItem.h"
#include "MarbleGraphicsGridLayout.h"

using namespace Marble;

const int defaultMinZoom = 900;
const int defaultMaxZoom = 2400;

NavigationFloatItem::NavigationFloatItem( const QPointF &point )
    : AbstractFloatItem( point ),
      m_marbleWidget( 0 ),
      m_widgetItem( 0 ),
      m_navigationWidgetSmall( 0 ),
      m_navigationWidget( 0 ),
      m_profiles( MarbleGlobal::getInstance()->profiles() ),
      m_oldViewportRadius( 0 )
{
    // Plugin is enabled by default
    setEnabled( true );
    setVisible( false );

    if( m_profiles & MarbleGlobal::SmallScreen ) {
        setFrame( FrameGraphicsItem::RectFrame );
    }
    else {
        setFrame( FrameGraphicsItem::RoundedRectFrame );
    }

    // This sets the padding to the minimum possible for this Frame
    setPadding( 0 );
}

NavigationFloatItem::~NavigationFloatItem()
{
    delete m_navigationWidgetSmall;
    delete m_navigationWidget;
}

QStringList NavigationFloatItem::backendTypes() const
{
    return QStringList("navigation");
}

QString NavigationFloatItem::name() const
{
    return tr("Navigation");
}

QString NavigationFloatItem::guiString() const
{
    return tr("&Navigation");
}

QString NavigationFloatItem::nameId() const
{
    return QString("navigation");
}

QString NavigationFloatItem::description() const
{
    return tr("A mouse control to zoom and move the map");
}

QIcon NavigationFloatItem::icon() const
{
    return QIcon();
}

void NavigationFloatItem::initialize()
{
    QWidget *navigationParent = new QWidget( 0 );

    if( m_profiles & MarbleGlobal::SmallScreen ) {
        m_navigationWidgetSmall = new Ui::NavigationSmall;
        m_navigationWidgetSmall->setupUi( navigationParent );
    }
    else {
        m_navigationWidget = new Ui::Navigation;
        m_navigationWidget->setupUi( navigationParent );
    }

    m_widgetItem = new WidgetGraphicsItem( this );
    m_widgetItem->setWidget( navigationParent );

    MarbleGraphicsGridLayout *layout = new MarbleGraphicsGridLayout( 1, 1 );
    layout->addItem( m_widgetItem, 0, 0 );

    setLayout( layout );

    if( !( m_profiles & MarbleGlobal::SmallScreen ) ) {
        connect( m_navigationWidget->zoomSlider,  SIGNAL( sliderPressed() ),
                 this, SLOT( adjustForAnimation() ) );
        connect( m_navigationWidget->zoomSlider,  SIGNAL( sliderReleased() ),
                 this, SLOT( adjustForStill() ) );
        connect( m_navigationWidget->zoomSlider, SIGNAL( valueChanged( int ) ),
                 this, SLOT( updateButtons( int ) ) );
        connect( m_navigationWidget->zoomSlider, SIGNAL( sliderMoved(int) ),
                 this, SLOT( setMarbleZoomValue(int) ) );
        // Other signal/slot connections will be initialized when the marble widget is known
    }
}

bool NavigationFloatItem::isInitialized() const
{
    return m_widgetItem;
}

void NavigationFloatItem::changeViewport( ViewportParams *viewport )
{
    if ( viewport->radius() != m_oldViewportRadius ) {
        m_oldViewportRadius = viewport->radius();
        // The slider depends on the map state (zoom factor)
        update();
    }
}

bool NavigationFloatItem::eventFilter( QObject *object, QEvent *e )
{
    if ( !enabled() || !visible() ) {
        return false;
    }

    MarbleWidget *widget = dynamic_cast<MarbleWidget*> (object);
    if ( !widget ) {
        return AbstractFloatItem::eventFilter( object, e );
    }

    if ( m_marbleWidget != widget ) {
        // Delayed initialization
        m_marbleWidget = widget;
        int minZoom = m_marbleWidget->minimumZoom();
        int maxZoom = m_marbleWidget->maximumZoom();
        //m_navigationWidget->zoomSlider->setRange(minZoom, maxZoom);

        if( m_profiles & MarbleGlobal::SmallScreen ) {
            connect( m_navigationWidgetSmall->zoomInButton, SIGNAL( clicked() ),
                     m_marbleWidget, SLOT( zoomIn() ) );
            connect( m_navigationWidgetSmall->zoomOutButton, SIGNAL( clicked() ),
                     m_marbleWidget, SLOT( zoomOut() ) );
            connect( m_navigationWidgetSmall->goHomeButton, SIGNAL( clicked() ),
                     m_marbleWidget, SLOT( goHome() ) );
        }
        else {
            m_navigationWidget->zoomSlider->setMinimum(minZoom);
            m_navigationWidget->zoomSlider->setMaximum(maxZoom);
            m_navigationWidget->zoomSlider->setValue(m_marbleWidget->zoom());
            m_navigationWidget->zoomSlider->setTickInterval((maxZoom - minZoom) / 15);

            connect( m_navigationWidget->zoomInButton, SIGNAL( clicked() ),
                     m_marbleWidget, SLOT( zoomIn() ) );
            connect( m_navigationWidget->zoomOutButton, SIGNAL( clicked() ),
                     m_marbleWidget, SLOT( zoomOut() ) );

            connect( m_navigationWidget->moveLeftButton, SIGNAL( clicked() ),
                     m_marbleWidget, SLOT( moveLeft() ) );
            connect( m_navigationWidget->moveRightButton, SIGNAL( clicked() ),
                     m_marbleWidget, SLOT( moveRight() ) );
            connect( m_navigationWidget->moveUpButton, SIGNAL( clicked() ),
                     m_marbleWidget, SLOT( moveUp() ) );
            connect( m_navigationWidget->moveDownButton, SIGNAL( clicked() ),
                     m_marbleWidget, SLOT( moveDown() ) );

            connect( m_navigationWidget->goHomeButton, SIGNAL( clicked() ),
                     m_marbleWidget, SLOT( goHome() ) );
        }

        connect( m_marbleWidget, SIGNAL( zoomChanged(int) ),
                 this, SLOT( setZoomSliderValue( int ) ) );
        connect( m_marbleWidget, SIGNAL( themeChanged( QString ) ),
                 this, SLOT( selectTheme( QString ) ) );

        updateButtons( m_marbleWidget->zoom() );
    }

    return AbstractFloatItem::eventFilter(object, e);
}

void NavigationFloatItem::setZoomSliderValue( int level )
{
    if( !( m_profiles & MarbleGlobal::SmallScreen ) ) {
        m_navigationWidget->zoomSlider->setValue( level );
    }
}

void NavigationFloatItem::setMarbleZoomValue( int level )
{
    // There exists a circular signal/slot connection between MarbleWidget and this widget's
    // zoom slider. MarbleWidget prevents recursion, but it still loops one time unless
    // disconnected here.
    // The circular signal/slot connection results into the Marble Globe flickering, when the
    // zoom slider is used.

    if( !m_marbleWidget ) {
        return;
    }

    disconnect( m_marbleWidget, SIGNAL( zoomChanged(int) ),
                this, SLOT( setZoomSliderValue( int ) ) );
    m_marbleWidget->zoomView( level );
    connect( m_marbleWidget, SIGNAL( zoomChanged(int) ),
                this, SLOT( setZoomSliderValue( int ) ) );
}

void NavigationFloatItem::selectTheme( QString theme )
{
    Q_UNUSED(theme);

    if ( m_marbleWidget ) {
        if( m_profiles & MarbleGlobal::SmallScreen ) {
            updateButtons( m_marbleWidget->zoom() );
        }
        else {
            int minZoom = m_marbleWidget->minimumZoom();
            int maxZoom = m_marbleWidget->maximumZoom();
            m_navigationWidget->zoomSlider->setRange( minZoom, maxZoom );
            m_navigationWidget->zoomSlider->setValue( m_marbleWidget->zoom() );
            updateButtons( m_navigationWidget->zoomSlider->value() );
        }
    }
}

void NavigationFloatItem::adjustForAnimation()
{
    if ( !m_marbleWidget ) {
        return;
    }

    m_marbleWidget->setViewContext( Animation );
}

void NavigationFloatItem::adjustForStill()
{
    if ( !m_marbleWidget ) {
        return;
    }

    m_marbleWidget->setViewContext( Still );

    if ( m_marbleWidget->mapQuality( Still )
        != m_marbleWidget->mapQuality( Animation ) ) {
        m_marbleWidget->updateChangedMap();
    }
}

void NavigationFloatItem::updateButtons( int zoomValue )
{
    int minZoom = defaultMinZoom;
    int maxZoom = defaultMaxZoom;
    QToolButton *zoomInButton = 0;
    QToolButton *zoomOutButton = 0;

    if( m_profiles & MarbleGlobal::SmallScreen ) {
        if ( m_marbleWidget ) {
            minZoom = m_marbleWidget->minimumZoom();
            maxZoom = m_marbleWidget->maximumZoom();
        }

        zoomInButton = m_navigationWidgetSmall->zoomInButton;
        zoomOutButton = m_navigationWidgetSmall->zoomOutButton;
    }
    else {
        minZoom = m_navigationWidget->zoomSlider->minimum();
        maxZoom = m_navigationWidget->zoomSlider->maximum();

        zoomInButton = m_navigationWidget->zoomInButton;
        zoomOutButton = m_navigationWidget->zoomOutButton;
    }

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

    if (m_marbleWidget)
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

Q_EXPORT_PLUGIN2( NavigationFloatItem, NavigationFloatItem )

#include "NavigationFloatItem.moc"
