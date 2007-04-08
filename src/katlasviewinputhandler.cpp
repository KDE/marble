#include <QtCore/QDebug>
#include "katlasviewinputhandler.h"

#ifndef Q_OS_MACX
#include "katlasviewinputhandler.moc"
#endif

#include "Quaternion.h"
#include "katlasview.h"
#include "katlasdirs.h"

const float pi = 3.14159f ;

KAtlasViewInputHandler::KAtlasViewInputHandler(KAtlasView *gpview, KAtlasGlobe *globe):m_gpview(gpview),m_globe(globe){

	curpmtl.load( KAtlasDirs::path("bitmaps/cursor_tl.xpm") );
	curpmtc.load( KAtlasDirs::path("bitmaps/cursor_tc.xpm") );
	curpmtr.load( KAtlasDirs::path("bitmaps/cursor_tr.xpm") );
	curpmcr.load( KAtlasDirs::path("bitmaps/cursor_cr.xpm") );
	curpmcl.load( KAtlasDirs::path("bitmaps/cursor_cl.xpm") );
	curpmbl.load( KAtlasDirs::path("bitmaps/cursor_bl.xpm") );
	curpmbc.load( KAtlasDirs::path("bitmaps/cursor_bc.xpm") );
	curpmbr.load( KAtlasDirs::path("bitmaps/cursor_br.xpm") );

	arrowcur [0][0] = QCursor(curpmtl,2,2);
	arrowcur [1][0] = QCursor(curpmtc,10,3);
	arrowcur [2][0] = QCursor(curpmtr,19,2);
	arrowcur [0][1] = QCursor(curpmcl,3,10);
	arrowcur [1][1] = QCursor(Qt::PointingHandCursor);
	arrowcur [2][1] = QCursor(curpmcr,18,10);
	arrowcur [0][2] = QCursor(curpmbl,2,19);
	arrowcur [1][2] = QCursor(curpmbc,11,18);
	arrowcur [2][2] = QCursor(curpmbr,19,19);

	m_leftpressed = false;
	m_midpressed = false;
}

bool KAtlasViewInputHandler::eventFilter( QObject* o, QEvent* e ){
		
//	if ( o == gpview ){
		if ( e->type() == QEvent::KeyPress ) {
			QKeyEvent *k = (QKeyEvent *)e;

			dirx = 0; diry = 0;
			switch (k->key()){
			case 0x01000012: 
				dirx = -1;
				break;
			case 0x01000013: 
				diry = -1;
				break;
			case 0x01000014: 
				dirx = 1;
				break;
			case 0x01000015: 
				diry = 1;
				break;
			case 0x2b:
				m_gpview->zoomIn();
				break;
			case 0x2d:
				m_gpview->zoomOut();
				break;
			case 0x01000010:
				m_gpview->goHome();
				break;
			default:
				break;
			}

			if ((dirx != 0) || (diry != 0)) {
				m_globe->rotateBy(-(m_gpview->getMoveStep())*(float)(diry), -(m_gpview->getMoveStep())*(float)(dirx));
				m_gpview->repaint();
			}
			return TRUE;
		} 
		
		else if (( e->type() == QEvent::MouseMove )||( e->type() == QEvent::MouseButtonPress )||( e->type() == QEvent::MouseButtonRelease)){

			QMouseEvent* event = static_cast<QMouseEvent*>(e);			
			QRegion activeRegion = m_gpview->getActiveRegion();

			dirx = 0; diry = 0;

			if ((activeRegion.contains(event->pos()))){
				if ( e->type() == QEvent::MouseButtonDblClick){
					qDebug("check");					
				}


				if ( e->type() == QEvent::MouseButtonPress && event->button() == Qt::LeftButton){

					m_dragtimer.restart();					

					m_leftpressed = true;
					m_midpressed = false;
					m_leftpressedx = event->x();
					m_leftpressedy = event->y();

					// 	m_leftpresseda: screen center latitude  during mouse press
					// 	m_leftpressedb: screen center longitude during mouse press
					m_gpview->getGlobeSphericals(m_gpview->width() / 2, m_gpview->height() / 2, m_leftpresseda, m_leftpressedb);

					if (m_globe->northPoleY() > 0){
						m_leftpressedb = pi - m_leftpressedb;
						m_leftpresseda = pi + m_leftpresseda;	 
					}
				}

				if ( e->type() == QEvent::MouseButtonPress && event->button() == Qt::MidButton){
					m_midpressed = true;
					m_leftpressed = false;
					m_midpressedy = event->y();
				}

				if ( e->type() == QEvent::MouseButtonPress && event->button() == Qt::RightButton){
					emit rmbRequest( event->x(), event->y() );
				}

				if ( e->type() == QEvent::MouseButtonRelease && event->button() == Qt::LeftButton){

					// show menu if mouse cursor position remains unchanged
					// the click takes less than 250 ms

					if( m_dragtimer.elapsed() <= 250 || m_leftpressedx == event->x() && m_leftpressedy == event->y() )
						emit lmbRequest( m_leftpressedx, m_leftpressedy );

					m_leftpressed = false;
				}

				if ( e->type() == QEvent::MouseButtonRelease && event->button() == Qt::MidButton){
					m_midpressed = false;
				}

				if ( e->type() == QEvent::MouseButtonRelease && event->button() == Qt::RightButton){
				}

				if (m_leftpressed == true){
					float radius = (float)(m_globe->getRadius());
					float deltax = (float)(event->x()-m_leftpressedx);
					float deltay = (float)(event->y()-m_leftpressedy);

					float direction = 1;

					if ( m_globe -> northPoleZ() > 0 ){	
						if (event->y() < ( m_globe->northPoleY() + m_gpview->height()/2))
							direction = -1;
					}
					else{	
						if (event->y() > (-m_globe->northPoleY() + m_gpview->height()/2))
							direction = -1;
					}

					m_globe->rotateTo(180/pi*(float)(-m_leftpressedb) + 90*deltay/radius, 
						-180/pi*(float)(m_leftpresseda) + 90*direction*deltax/radius);

					m_gpview->repaint();
				}


				if (m_midpressed == true){
					int eventy = event->y();
					int dy = m_midpressedy - eventy;
					m_midpressed = eventy;
					m_gpview->zoomViewBy((int)(2 * dy / 3));
					m_gpview->repaint();
				}

			}
			else {
				m_leftpressed = false;

				dirx = (int)(3 * event->x()/(m_gpview->width()))-1;
				if (dirx > 1) dirx = 1;
				if (dirx < -1) dirx = -1;

				diry = (int)(3 * event->y()/(m_gpview->height()))-1;
				if (diry > 1) diry = 1;
				if (diry < -1) diry = -1;

				if ((event->button() == Qt::LeftButton) && ( e->type() == QEvent::MouseButtonPress )){

					m_globe->rotateBy(-(m_gpview->getMoveStep())*(float)(diry), -(m_gpview->getMoveStep())*(float)(dirx));
					m_gpview->repaint();
				}				
			}				

			m_gpview->setCursor(arrowcur[dirx+1][diry+1]);
			return true;
		}

		else {
			if ( e->type() == QEvent::Wheel ){
				QWheelEvent* wheelevt = static_cast<QWheelEvent*>(e);
				m_gpview->zoomViewBy((int)(-wheelevt->delta()/3));
				return TRUE;
			}
			else
				return FALSE;
		}
}
