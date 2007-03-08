#include <QtCore/QDebug>
#include <QtGui/QMenu>

#include "katlasviewpopupmenu.moc"
#include "katlasviewpopupmenu.h"
#include "katlasview.h"
#include "katlasglobe.h"
#include "placemark.h"
#include "placemarkinfodialog.h"

KAtlasViewPopupMenu::KAtlasViewPopupMenu(KAtlasView *gpview, KAtlasGlobe *globe):m_gpview(gpview),m_globe(globe){
	m_lmbMenu = new QMenu( m_gpview );
	m_rmbMenu = new QMenu( m_gpview );

	connect( m_lmbMenu, SIGNAL(  triggered ( QAction* ) ), this, SLOT( showFeatureInfo( QAction* ) ) );
	createActions();
}

void KAtlasViewPopupMenu::createActions(){

//	Property actions (Left mouse button)

//	m_earthaction = new QAction(QIcon("icon.png"), tr("&Earth"), this);
	m_earthaction = new QAction(tr("&Earth"), this);
	m_earthaction->setData( 0 );
	m_posaction = new QAction(tr("0 N 0 W"), this);
	m_posaction->setFont( QFont("Sans Serif",7, 50, false ) );

//	Tool actions (Right mouse button)

	m_pAddMeasurePointAction = new QAction(tr("&Add Measure Point"), this);	
	m_rmbMenu->addAction( m_pAddMeasurePointAction );
	m_pRemoveMeasurePointsAction = new QAction(tr("&Remove Measure Points"), this);
	m_rmbMenu->addAction( m_pRemoveMeasurePointsAction );

	connect( m_pAddMeasurePointAction, SIGNAL( triggered() ), SLOT( slotAddMeasurePoint() ) );
	connect( m_pRemoveMeasurePointsAction, SIGNAL( triggered() ), SIGNAL( removeMeasurePoints() ) );

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
		action->setIcon( (*it)->symbolPixmap() );
		m_lmbMenu->addAction( action );
		actionidx++;
	}

	// Not implemented yet ;-)
	m_earthaction->setEnabled( false );

	m_lmbMenu->addAction( m_earthaction );
	m_lmbMenu->addSeparator();

	float lat,lng;

	m_gpview->getGlobeSphericals(xpos, ypos, lng, lat);

	// Any idea what this could do on activation?
	m_posaction->setEnabled( false );

	m_posaction->setText( GeoPoint( lng, lat ).toString() );
	m_lmbMenu->addAction( m_posaction );

	m_lmbMenu->popup( m_gpview->mapToGlobal( curpos ) );
}


void KAtlasViewPopupMenu::showRmbMenu( int xpos, int ypos ){

	QPoint curpos = QPoint( xpos, ypos );
	m_pAddMeasurePointAction->setData( curpos );
	m_rmbMenu->popup( m_gpview->mapToGlobal( curpos ) );
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

void KAtlasViewPopupMenu::slotAddMeasurePoint(){
	QPoint p = m_pAddMeasurePointAction->data().toPoint();

	float lng, lat;

	m_gpview->getGlobeSphericals( p.x(), p.y(), lng, lat);

	emit addMeasurePoint( lng, lat );
}
