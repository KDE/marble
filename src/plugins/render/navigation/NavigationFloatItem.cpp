//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "NavigationFloatItem.h"

#include <QtCore/qmath.h>
#include <QtCore/QRect>
#include <QtGui/QPixmap>
#include <QtGui/QToolButton>
#include <QtGui/QSlider>
#include <QtGui/QWidget>
#include <QtGui/QPainter>

#include "ui_navigation.h"
#include "ViewportParams.h"
#include "MarbleDebug.h"
#include "MarbleWidget.h"
#include "WidgetGraphicsItem.h"
#include "MarbleGraphicsGridLayout.h"

using namespace Marble;
/* TRANSLATOR Marble::NavigationFloatItem */

NavigationFloatItem::NavigationFloatItem()
    : AbstractFloatItem( 0 )
{
}

NavigationFloatItem::NavigationFloatItem( const MarbleModel *marbleModel )
    : AbstractFloatItem( marbleModel, QPointF( -10, -30 ) ),
      m_marbleWidget( 0 ),
      m_widgetItem( 0 ),
      m_navigationWidget( 0 ),
      m_oldViewportRadius( 0 )
{
    // Plugin is enabled by default
    setEnabled( true );
    setVisible( false );

    setCacheMode( NoCache );
    setBackground( QBrush( QColor( Qt::transparent ) ) );
    setFrame( NoFrame );
}

NavigationFloatItem::~NavigationFloatItem()
{
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

QString NavigationFloatItem::version() const
{
    return "1.0";
}

QString NavigationFloatItem::description() const
{
    return tr("A mouse control to zoom and move the map");
}

QString NavigationFloatItem::copyrightYears() const
{
    return "2008, 2010, 2013";
}

QList<PluginAuthor> NavigationFloatItem::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" )
            << PluginAuthor( "Bastian Holst", "bastianholst@gmx.de" )
            << PluginAuthor( "Mohammed Nafees", "nafees.technocool@gmail.com" );
}

QIcon NavigationFloatItem::icon() const
{
    return QIcon(":/icons/navigation.png");
}

void NavigationFloatItem::initialize()
{
    QWidget *navigationParent = new QWidget( 0 );
    navigationParent->setAttribute( Qt::WA_NoSystemBackground, true );

    m_navigationWidget = new Ui::Navigation;
    m_navigationWidget->setupUi( navigationParent );

    m_widgetItem = new WidgetGraphicsItem( this );
    m_widgetItem->setWidget( navigationParent );

    MarbleGraphicsGridLayout *layout = new MarbleGraphicsGridLayout( 1, 1 );
    layout->addItem( m_widgetItem, 0, 0 );

    setLayout( layout );
}

bool NavigationFloatItem::isInitialized() const
{
    return m_widgetItem;
}

void NavigationFloatItem::changeViewport( ViewportParams *viewport )
{
    if ( viewport->radius() != m_oldViewportRadius ) {
        const qreal zoomValue = (200.0 * qLn( viewport->radius() ) ); // copied from MarbleWidgetPrivate::zoom()
        setZoomSliderValue( zoomValue );

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

        m_maxZoom = m_marbleWidget->maximumZoom();
        m_minZoom = m_marbleWidget->minimumZoom();

        m_navigationWidget->arrowDisc->setMarbleWidget( m_marbleWidget );
        connect( m_navigationWidget->arrowDisc, SIGNAL(repaintNeeded()), SIGNAL(repaintNeeded()) );

        connect( m_navigationWidget->homeButton, SIGNAL(repaintNeeded()), SIGNAL(repaintNeeded()) );
        connect( m_navigationWidget->homeButton, SIGNAL(clicked()),
                 m_marbleWidget, SLOT(goHome()) );

        connect( m_navigationWidget->zoomInButton, SIGNAL(repaintNeeded()), SIGNAL(repaintNeeded()) );
        connect( m_navigationWidget->zoomInButton, SIGNAL(clicked()),
                 m_marbleWidget, SLOT(zoomIn()) );

        connect( m_navigationWidget->zoomOutButton, SIGNAL(repaintNeeded()), SIGNAL(repaintNeeded()) );
        connect( m_navigationWidget->zoomOutButton, SIGNAL(clicked()),
                 m_marbleWidget, SLOT(zoomOut()) );

        connect( m_marbleWidget, SIGNAL(zoomChanged(int)), SLOT(updateButtons(int)) );
        updateButtons( m_marbleWidget->zoom() );
        connect( m_marbleWidget, SIGNAL(themeChanged(QString)), this, SLOT(selectTheme(QString)) );

     }

    return AbstractFloatItem::eventFilter(object, e);
}

void NavigationFloatItem::setZoomSliderValue( int level )
{
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

    disconnect( m_marbleWidget, SIGNAL(zoomChanged(int)),
                this, SLOT(setZoomSliderValue(int)) );
    m_marbleWidget->zoomView( level );
    connect( m_marbleWidget, SIGNAL(zoomChanged(int)),
                this, SLOT(setZoomSliderValue(int)) );
}

void NavigationFloatItem::selectTheme( QString )
{
    if ( m_marbleWidget ) {
        m_maxZoom = m_marbleWidget->maximumZoom();
        m_minZoom = m_marbleWidget->minimumZoom();
        updateButtons( m_marbleWidget->zoom() );
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
}

void NavigationFloatItem::updateButtons( int zoomValue )
{
    bool const zoomInEnabled = m_navigationWidget->zoomInButton->isEnabled();
    bool const zoomOutEnabled = m_navigationWidget->zoomOutButton->isEnabled();
    m_navigationWidget->zoomInButton->setEnabled( zoomValue < m_maxZoom );
    m_navigationWidget->zoomOutButton->setEnabled( zoomValue > m_minZoom );
    if ( zoomInEnabled != m_navigationWidget->zoomInButton->isEnabled() ||
         zoomOutEnabled != m_navigationWidget->zoomOutButton->isEnabled() ) {
        update();
    }
}

Q_EXPORT_PLUGIN2( NavigationFloatItem, Marble::NavigationFloatItem )

#include "NavigationFloatItem.moc"
