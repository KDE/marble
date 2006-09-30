#include "katlasthemeselectview.h"

#ifdef KDEBUILD
#include "katlasthemeselectview.moc"
#endif


#include <QResizeEvent>

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
	
	connect( this, SIGNAL( pressed( QModelIndex ) ), SLOT( selectedMapTheme( QModelIndex ) ) );
}
/*
KAtlasThemeSelectView::setModel( QAbstractItemModel * model ){

}
*/
void KAtlasThemeSelectView::resizeEvent(QResizeEvent* event)
{
    QListView::resizeEvent(event);
    QSize size = gridSize();
    size.setWidth(event->size().width());
    setGridSize(size);
}
