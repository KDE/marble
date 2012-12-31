
//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Patrick Spendrin <ps_ml@gmx.de>
//

#include "FileViewFloatItem.h"

#include <QtCore/QRect>
#include <QtCore/QPoint>
#include <QtGui/QApplication>
#include <QtGui/QFileDialog>
#include <QtGui/QListView>
#include <QtGui/QListWidget>
#include <QtGui/QMenu>
#include <QtGui/QPixmap>
#include <QtGui/QSlider>
#include <QtGui/QMouseEvent>

#include "MarbleDebug.h"
#include "FileViewModel.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "MarbleWidget.h"


using namespace Marble;

FileViewFloatItem::FileViewFloatItem(const QPointF &point,
        const QSizeF &size) :
    AbstractFloatItem(point, size), m_marbleWidget(0),
            m_fileView(0), m_fileViewParent(0),
            m_repaintScheduled(true), m_persIndex(0)
{
    // Plugin is not enabled by default
    setEnabled(false);
}

FileViewFloatItem::~FileViewFloatItem()
{
    delete m_fileViewParent;
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
    connect(m_fileView, SIGNAL(customContextMenuRequested(QPoint)),
            this,       SLOT(contextMenu(QPoint)));
}

bool FileViewFloatItem::isInitialized() const
{
    return m_fileViewParent != 0;
}

QPainterPath FileViewFloatItem::backgroundShape() const
{
    QPainterPath path;
    QSizeF paintedSize = paintedRect().size();
    path.addRoundedRect( QRectF( 0.0, 0.0, paintedSize.width() - 1, paintedSize.height() - 1 ), 6, 6 );
    return path;
}

void FileViewFloatItem::changeViewport( ViewportParams *viewport )
{
    Q_UNUSED( viewport );
    update();
}

void FileViewFloatItem::paintContent( GeoPainter *painter, ViewportParams *viewport,
                                      const QString& renderPos, GeoSceneLayer * layer)
{
    Q_UNUSED( viewport );
    Q_UNUSED( layer );
    Q_UNUSED( renderPos );

    if( !m_fileView->model() ) {
        m_fileView->setModel(marbleModel()->fileViewModel());
    }
    painter->end();
    // Paint widget without a background
    m_fileViewParent->render( painter->device(), 
          QPoint( padding(), padding() ), QRegion(),QWidget::RenderFlags(QWidget::DrawChildren));

    painter->begin( painter->device() );
    m_fileView->update();
}

bool FileViewFloatItem::eventFilter(QObject *object, QEvent *e)
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
    }

    Q_ASSERT( m_marbleWidget );
    // Mouse events are forwarded to the underlying widget
    QMouseEvent *event = static_cast<QMouseEvent*> (e);
    QRectF floatItemRect = QRectF( positivePosition(), size() );

    QPoint shiftedPos = event->pos() - floatItemRect.topLeft().toPoint()
            - QPoint( padding(), padding() );
    if( e->type() == QEvent::MouseMove ) {
        m_itemPosition = event->globalPos();
    }

    if( floatItemRect.contains( event->pos() ) ) {
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
            QMouseEvent shiftedEvent( e->type(), shiftedPos,
                    event->globalPos(), event->button(), event->buttons(),
                    event->modifiers() );
            if( QApplication::sendEvent(child, &shiftedEvent) ) {
                if( e->type() == QEvent::MouseButtonPress || 
                    e->type() == QEvent::MouseButtonRelease || 
                    e->type() == QEvent::MouseButtonDblClick ||
                    e->type() == QEvent::MouseMove )
                    updateFileView();
                return true;
            }
            
        }
    }

    return AbstractFloatItem::eventFilter(object, e);
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
        QRectF floatItemRect = QRectF(positivePosition(), size()).toRect();
        QRegion dirtyRegion(floatItemRect.toRect());

        m_marbleWidget->update(dirtyRegion);
    }
}

void FileViewFloatItem::contextMenu( const QPoint& pos )
{
    if( !m_marbleWidget )
        return;

    QMenu *test = new QMenu( m_fileView );
    // We need the global position to move the menu.
    // pos contains the relative position.
    test->move( m_itemPosition );
    connect( test->addAction( tr( "Open file..." ) ), SIGNAL(triggered()),
             this, SLOT(addFile()) );
    connect( test->addAction( tr( "Close this file" ) ), SIGNAL(triggered()),
             this, SLOT(removeFile()) );
    m_persIndex = new QPersistentModelIndex( m_fileView->indexAt( pos ) );
    test->exec();
}

void FileViewFloatItem::addFile()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(m_marbleWidget, tr("Open File"),
                                            QString(),
                                            tr("All Supported Files (*.gpx *.kml *.pnt);;GPS Data (*.gpx);;Google Earth KML (*.kml);PNT Data (*.pnt)"));

    if ( ! fileName.isEmpty() ) {
        m_marbleWidget->model()->addGeoDataFile( fileName );
    }
}

void FileViewFloatItem::removeFile()
{
    //reinterpret_cast<FileViewModel*>(m_fileView->model())->setSelectedIndex( *m_persIndex );
    mDebug() << m_fileView->model()->data( *m_persIndex, Qt::DisplayRole ).toString();
    // close selected file
    reinterpret_cast<FileViewModel*>(m_fileView->model())->closeFile();
}
	 

Q_EXPORT_PLUGIN2(FileViewFloatItem, FileViewFloatItem)

#include "FileViewFloatItem.moc"
