#include "katlasthemeselectview.h"

#include <QtGui/QResizeEvent>


KAtlasThemeSelectView::KAtlasThemeSelectView(QWidget *parent)
    : QListView(parent)
{

    setViewMode( QListView::IconMode );
    setFlow( QListView::TopToBottom ); 
    setWrapping( false ); 
    setMovement( QListView::Static );
    setResizeMode( QListView::Fixed );
    setUniformItemSizes ( true );
    setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    setEditTriggers( QAbstractItemView::NoEditTriggers );
    setIconSize( QSize( 136,136 ) );
	
    connect( this, SIGNAL( pressed( QModelIndex ) ),
                   SLOT( selectedMapTheme( QModelIndex ) ) );
}

#if 0
KAtlasThemeSelectView::setModel( QAbstractItemModel * model )
{
}
#endif

void KAtlasThemeSelectView::resizeEvent(QResizeEvent* event)
{
    QListView::resizeEvent(event);

    QSize  size = gridSize();
    size.setWidth(event->size().width());
    setGridSize(size);
}


void KAtlasThemeSelectView::selectedMapTheme( QModelIndex index )
{
    const QAbstractItemModel  *model = index.model();

    QModelIndex  colindex        = model->index( index.row(), 2, 
                                                 QModelIndex() );
    QString      currentmaptheme = (model->data( colindex )).toString();
    emit selectMapTheme( currentmaptheme ); 

    // qDebug() << currentmaptheme;
}


#ifndef Q_OS_MACX
#include "katlasthemeselectview.moc"
#endif
