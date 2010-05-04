//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Dennis Nienhüser <earthwings@gentoo.org>
//

#include "NavigationFloatItem.h"

#include <QtCore/QRect>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmap>
#include <QtGui/QSlider>
#include <QtGui/QWidget>

#include "GeoPainter.h"
#include "ViewportParams.h"
#include "MarbleWidget.h"
#include "MarbleMap.h"

using namespace Marble;

NavigationFloatItem::NavigationFloatItem(const QPointF &point,
        const QSizeF &size) :
    AbstractFloatItem(point, size), m_marbleWidget(0),
            m_navigationParent(0), m_oldViewportRadius(0)
{
    // Plugin is enabled by default
    setEnabled( true );
    // Plugin is not visible by default
    setVisible( false );

    setFrame( FrameGraphicsItem::RoundedRectFrame );
}

NavigationFloatItem::~NavigationFloatItem()
{
    delete m_navigationParent;
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
    m_navigationParent = new QWidget(0);
    m_navigationParent->setFixedSize(size().toSize() - QSize(2 * padding(), 2
            * padding()));
    m_navigationWidget.setupUi(m_navigationParent);

    connect( m_navigationWidget.zoomSlider,  SIGNAL( sliderPressed() ),
             this, SLOT( adjustForAnimation() ) );
    connect( m_navigationWidget.zoomSlider,  SIGNAL( sliderReleased() ),
             this, SLOT( adjustForStill() ) );
    connect( m_navigationWidget.zoomSlider, SIGNAL( valueChanged( int ) ),
             this, SLOT( updateButtons( int ) ) );
    // Other signal/slot connections will be initialized when the marble widget is known
}

bool NavigationFloatItem::isInitialized() const
{
    return m_navigationParent != 0;
}

void NavigationFloatItem::changeViewport( ViewportParams *viewport )
{
    if ( viewport->radius() != m_oldViewportRadius ) {
        m_oldViewportRadius = viewport->radius();
        // The slider depends on the map state (zoom factor)
        update();
    }
}

void NavigationFloatItem::paintContent( GeoPainter *painter, ViewportParams *viewport,
                                        const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( viewport );
    Q_UNUSED( layer );
    Q_UNUSED( renderPos );

    // Paint widget without a background
    m_navigationParent->render( painter, 
          QPoint( padding(), padding() ), QRegion(),QWidget::RenderFlags(QWidget::DrawChildren));
}

bool NavigationFloatItem::eventFilter(QObject *object, QEvent *e)
{
    if ( !enabled() || !visible() ) {
        return false;
    }

    MarbleWidget *widget = dynamic_cast<MarbleWidget*> (object);
    if ( !widget ) {
        return AbstractFloatItem::eventFilter(object, e);
    }

    if ( m_marbleWidget != widget ) {
        // Delayed initialization
        m_marbleWidget = widget;
        int minZoom = m_marbleWidget->map()->minimumZoom();
        int maxZoom = m_marbleWidget->map()->maximumZoom();
        //m_navigationWidget.zoomSlider->setRange(minZoom, maxZoom);
        m_navigationWidget.zoomSlider->setMinimum(minZoom);
        m_navigationWidget.zoomSlider->setMaximum(maxZoom);
        m_navigationWidget.zoomSlider->setValue(m_marbleWidget->map()->zoom());
        m_navigationWidget.zoomSlider->setTickInterval((maxZoom - minZoom) / 15);
        updateButtons(m_marbleWidget->map()->zoom());
        connect(m_marbleWidget->map(), SIGNAL(zoomChanged(int)), this, SLOT(zoomChanged(int)));
        connect(m_marbleWidget, SIGNAL( themeChanged( QString ) ), this, SLOT( selectTheme( QString ) ) );
        connect(m_navigationWidget.zoomSlider, SIGNAL(sliderMoved(int)), m_marbleWidget, SLOT(zoomView(int)));
        connect(m_navigationWidget.zoomInButton, SIGNAL( clicked() ), m_marbleWidget, SLOT( zoomIn() ) );
        connect(m_navigationWidget.zoomOutButton, SIGNAL( clicked() ), m_marbleWidget, SLOT( zoomOut() ) );
        connect(m_navigationWidget.moveLeftButton, SIGNAL( clicked() ), m_marbleWidget, SLOT( moveLeft() ) );
        connect(m_navigationWidget.moveRightButton, SIGNAL( clicked() ), m_marbleWidget, SLOT( moveRight() ) );
        connect(m_navigationWidget.moveUpButton, SIGNAL( clicked() ), m_marbleWidget, SLOT( moveUp() ) );
        connect(m_navigationWidget.moveDownButton, SIGNAL( clicked() ), m_marbleWidget, SLOT( moveDown() ) );
        connect(m_navigationWidget.goHomeButton, SIGNAL( clicked() ), m_marbleWidget, SLOT( goHome() ) );
    }

    Q_ASSERT(m_marbleWidget);

    if ( e->type() == QEvent::MouseButtonDblClick || e->type()
            == QEvent::MouseMove || e->type() == QEvent::MouseButtonPress
            || e->type() == QEvent::MouseButtonRelease ) {
        // Mouse events are forwarded to the underlying widget
        QMouseEvent *event = static_cast<QMouseEvent*> (e);
        QRectF floatItemRect = QRectF( positivePosition(), size() );

        QPoint shiftedPos = event->pos() - floatItemRect.topLeft().toPoint()
                - QPoint(padding(), padding());
        if ( floatItemRect.contains(event->pos()) ) {
            QWidget *child = m_navigationParent->childAt( shiftedPos );
            if ( child ) {
                m_marbleWidget->setCursor( Qt::ArrowCursor );
                shiftedPos -= child->pos(); // transform to children's coordinates
                QMouseEvent shiftedEvent = QMouseEvent( e->type(), shiftedPos,
                        event->globalPos(), event->button(), event->buttons(),
                        event->modifiers() );
                if ( QApplication::sendEvent( child, &shiftedEvent ) ) {
                    return true;
                }
            }
        }
    }

    return AbstractFloatItem::eventFilter(object, e);
}

void NavigationFloatItem::zoomChanged(int level)
{
    m_navigationWidget.zoomSlider->setValue(level);
}

void NavigationFloatItem::selectTheme(QString theme)
{
    Q_UNUSED(theme);
    
    if ( m_marbleWidget ) {
        int minZoom = m_marbleWidget->map()->minimumZoom();
        int maxZoom = m_marbleWidget->map()->maximumZoom();
        m_navigationWidget.zoomSlider->setRange(minZoom, maxZoom);
        m_navigationWidget.zoomSlider->setValue(m_marbleWidget->map()->zoom());
        updateButtons(m_navigationWidget.zoomSlider->value());
    }
}

void NavigationFloatItem::adjustForAnimation()
{
    if ( !m_marbleWidget ) {
        return;
    }

    m_marbleWidget->setViewContext( Marble::Animation );
}

void NavigationFloatItem::adjustForStill()
{
    if ( !m_marbleWidget ) {
        return;
    }

    m_marbleWidget->setViewContext( Marble::Still );

    if ( m_marbleWidget->mapQuality( Marble::Still )
        != m_marbleWidget->mapQuality( Marble::Animation ) ) {
        m_marbleWidget->updateChangedMap();
    }
}

void NavigationFloatItem::updateButtons( int value )
{
    if ( value <= m_navigationWidget.zoomSlider->minimum() ) {
        m_navigationWidget.zoomInButton->setEnabled( true );
        m_navigationWidget.zoomOutButton->setEnabled( false );
    } else if ( value >= m_navigationWidget.zoomSlider->maximum() ) {
        m_navigationWidget.zoomInButton->setEnabled( false );
        m_navigationWidget.zoomOutButton->setEnabled( true );
    } else {
        m_navigationWidget.zoomInButton->setEnabled( true );
        m_navigationWidget.zoomOutButton->setEnabled( true );
    }
    
    if (m_marbleWidget)
    {
        // Trigger a repaint of the float item. Otherwise button state updates
        // are delayed
        QRectF floatItemRect = QRectF(positivePosition(), size()).toRect();
        QRegion dirtyRegion(floatItemRect.toRect());
    
        m_marbleWidget->setAttribute( Qt::WA_NoSystemBackground,  false );

        update();

        m_marbleWidget->setAttribute( Qt::WA_NoSystemBackground,  m_marbleWidget->map()->mapCoversViewport() );
    }
}

Q_EXPORT_PLUGIN2(NavigationFloatItem, NavigationFloatItem)

#include "NavigationFloatItem.moc"
