//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Patrick Spendrin <ps_ml@gmx.de>
//

#include "FileViewFloatItem.h"

#include <QtCore/QDebug>
#include <QtCore/QRect>
#include <QtCore/QPoint>
#include <QtGui/QApplication>
#include <QtGui/QListView>
#include <QtGui/QListWidget>
#include <QtGui/QMenu>
#include <QtGui/QPixmap>
#include <QtGui/QSlider>

#include "FileViewModel.h"
#include "AbstractFileViewItem.h"
#include "AbstractProjection.h"
#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "MarbleWidget.h"
#include "MarbleDataFacade.h"
#include "MarbleMap.h"


using namespace Marble;

FileViewFloatItem::FileViewFloatItem(const QPointF &point,
        const QSizeF &size) :
    MarbleAbstractFloatItem(point, size), m_marbleWidget(0),
            m_fileViewParent(0), m_repaintScheduled(true),
            m_fileView(0), m_persIndex(0)
{
    // Plugin is not enabled by default
    setEnabled(false);
}

FileViewFloatItem::~FileViewFloatItem()
{
}

QStringList FileViewFloatItem::backendTypes() const
{
    return QStringList("fileview");
}

QString FileViewFloatItem::name() const
{
    return tr("File View");
}

QString FileViewFloatItem::guiString() const
{
    return tr("&File View");
}

QString FileViewFloatItem::nameId() const
{
    return QString("fileview");
}

QString FileViewFloatItem::description() const
{
    return tr("A list of currently opened files");
}

QIcon FileViewFloatItem::icon() const
{
    return QIcon();
}

void FileViewFloatItem::initialize()
{
    m_fileViewParent = new QWidget(0);
    m_fileViewParent->setFixedSize(size().toSize() - QSize(2 * padding(), 2
            * padding()));
    m_fileView = new QListView(m_fileViewParent);
    m_fileView->resize(100, 240);
    m_fileView->setResizeMode(QListView::Adjust);
    m_fileView->setContextMenuPolicy( Qt::CustomContextMenu );
    connect(m_fileView, SIGNAL(customContextMenuRequested ( const QPoint & )),
            this,       SLOT(contextMenu(const QPoint& )));
}

bool FileViewFloatItem::isInitialized() const
{
    return m_fileViewParent != 0;
}

bool FileViewFloatItem::needsUpdate(ViewportParams *viewport)
{
    return true;
}

QPainterPath FileViewFloatItem::backgroundShape() const
{
    QPainterPath path;
    path.addRoundedRect( QRectF( 0.0, 0.0, renderedRect().size().width() - 1, renderedRect().size().height() - 1 ), 6, 6 );
    return path;
}

bool FileViewFloatItem::renderFloatItem(GeoPainter *painter,
        ViewportParams *viewport, GeoSceneLayer * layer)
{
    Q_UNUSED(viewport);
    Q_UNUSED(layer);

    if( !m_fileView->model() ) {
        m_fileView->setModel(dataFacade()->fileViewModel());
    }
    painter->end();
    // Paint widget without a background
    m_fileViewParent->render( painter->device(), 
          QPoint( padding(), padding() ), QRegion(),QWidget::RenderFlags(QWidget::DrawChildren));

    painter->begin( painter->device() );
    m_fileView->repaint();

    return true;
}

bool FileViewFloatItem::eventFilter(QObject *object, QEvent *e)
{
    if ( !enabled() || !visible() ) {
        return false;
    }

    MarbleWidget *widget = dynamic_cast<MarbleWidget*> (object);
    if ( !widget ) {
        return MarbleAbstractFloatItem::eventFilter(object, e);
    }

    if ( m_marbleWidget != widget ) {
        // Delayed initialization
        m_marbleWidget = widget;
    }

    Q_ASSERT( m_marbleWidget );
    // Mouse events are forwarded to the underlying widget
    QMouseEvent *event = static_cast<QMouseEvent*> (e);
    QRectF floatItemRect = QRectF(positivePosition(QRectF(0, 0,
            widget->width(), widget->height())), size());

    QPoint shiftedPos = event->pos() - floatItemRect.topLeft().toPoint()
            - QPoint( padding(), padding() );
    if( e->type() == QEvent::MouseMove ) {
        m_itemPosition = event->pos() + QPoint( 6 * padding(), 0 );
    }

    if( floatItemRect.contains(event->pos()) ) {
        QWidget *child = m_fileViewParent->childAt( shiftedPos );

        if( child ) {
            m_marbleWidget->setCursor( Qt::ArrowCursor );

            // there needs to be some extra handling for the scrollbars
            // these need some special treatment due to them not forwarding 
            // their mouseevents to their scrollbars.
            if( reinterpret_cast<QScrollBar*>( child ) == m_fileView->horizontalScrollBar() ) {
                shiftedPos -= QPoint( 0, m_fileView->viewport()->size().height() );
            } else if( reinterpret_cast<QScrollBar*>( child ) == m_fileView->verticalScrollBar() ) {
                shiftedPos -= QPoint( m_fileView->viewport()->size().width(), 0 );
            }
            QMouseEvent* shiftedEvent = new QMouseEvent( e->type(), shiftedPos,
                    event->globalPos(), event->button(), event->buttons(),
                    event->modifiers() );
            if( QApplication::sendEvent(child, shiftedEvent) ) {
                if( e->type() == QEvent::MouseButtonPress || 
                    e->type() == QEvent::MouseButtonRelease || 
                    e->type() == QEvent::MouseButtonDblClick ||
                    e->type() == QEvent::MouseMove )
                    updateFileView();
                return true;
            }
            
        }
    }

    return MarbleAbstractFloatItem::eventFilter(object, e);
}

void FileViewFloatItem::selectTheme(QString theme)
{
    Q_UNUSED(theme);
    
    if ( m_marbleWidget ) {
    }
}

void FileViewFloatItem::updateFileView()
{
    if (m_marbleWidget)
    {
        // Trigger a repaint of the float item. Otherwise button state updates
        // are delayed
        QRectF floatItemRect = QRectF(positivePosition(QRect(0,0,
                m_marbleWidget->width(),m_marbleWidget->height())), size()).toRect();
        QRegion dirtyRegion(floatItemRect.toRect());

        m_marbleWidget->repaint(dirtyRegion);
    }
}

void FileViewFloatItem::contextMenu( const QPoint& pos )
{
    if( !m_marbleWidget )
        return;

    qDebug() << "custom Context menu requested!" << pos;
    QMenu *test = new QMenu( m_fileView );
    test->move( m_itemPosition );
    connect( test->addAction( "Open new kml file..." ), SIGNAL( triggered() ),
             this, SLOT( addFile() ) );
    connect( test->addAction("close this kml file..." ), SIGNAL( triggered() ),
             this, SLOT( removeFile() ) );
    m_persIndex = new QPersistentModelIndex( m_fileView->indexAt( pos ) );
    test->exec();
}

void FileViewFloatItem::addFile()
{
}

void FileViewFloatItem::removeFile()
{
    reinterpret_cast<FileViewModel*>(m_fileView->model())->setSelectedIndex( *m_persIndex );
    qDebug() << m_fileView->model()->data( *m_persIndex, Qt::DisplayRole ).toString();
    // close selected file
    reinterpret_cast<FileViewModel*>(m_fileView->model())->closeFile();
}
	 

Q_EXPORT_PLUGIN2(FileViewFloatItem, FileViewFloatItem)

#include "FileViewFloatItem.moc"
