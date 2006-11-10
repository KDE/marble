#include <QDebug>
#include <QMenu>

#include "katlasviewpopupmenu.h"
#include "katlasview.h"
#include "katlasglobe.h"
#include "placemark.h"
#include "placemarkinfodialog.h"

KAtlasViewPopupMenu::KAtlasViewPopupMenu(KAtlasView *gpview, KAtlasGlobe *globe):m_gpview(gpview),m_globe(globe){
	m_lmbMenu = new QMenu( m_gpview );

	connect( m_lmbMenu, SIGNAL(  triggered ( QAction* ) ), this, SLOT( showFeatureInfo( QAction* ) ) );
	createActions();
}

void KAtlasViewPopupMenu::createActions(){
//	m_earthaction = new QAction(QIcon("icon.png"), tr("&Earth"), this);
	m_earthaction = new QAction(tr("&Earth"), this);
	m_earthaction->setData( 0 );
}

void KAtlasViewPopupMenu::showLmbMenu( int xpos, int ypos ){
	m_lmbMenu->clear();
	QPoint curpos = QPoint( xpos, ypos ); 

	m_featurelist = m_globe-> whichFeatureAt( curpos );

	QVector<PlaceMark*>::const_iterator it;

	int actionidx = 1;

	for ( it=m_featurelist.constBegin(); it != m_featurelist.constEnd(); it++ ){ // STL iterators
		QAction* action = new QAction( (*it)->name(), m_lmbMenu );
		action->setData( actionidx );
		m_lmbMenu->addAction( action );
		actionidx++;
	}
	m_lmbMenu->addAction( m_earthaction );

	m_lmbMenu->popup( m_gpview->mapToGlobal( curpos ) );
}

void KAtlasViewPopupMenu::showFeatureInfo( QAction* action ){
	int actionidx = action->data().toInt();
	PlaceMark* mark = 0;

	if ( actionidx > 0 ) {
		mark = m_featurelist.at( actionidx -1 );
		PlaceMarkInfoDialog dialog(mark, m_gpview);
		dialog.exec();
	}

}
