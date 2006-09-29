#include <QDebug>
#include "katlasviewinputhandler.h"
#include "katlasview.h"
#include "quaternion.h"
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

	m_pressed = false;
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

			QMouseEvent* event = dynamic_cast<QMouseEvent*>(e);			
			QRegion activeRegion = m_gpview->getActiveRegion();

			dirx = 0; diry = 0;

			if ((activeRegion.contains(event->pos()))){
				if ( e->type() == QEvent::MouseButtonDblClick){
					qDebug("check");					
				}

				if ( e->type() == QEvent::MouseButtonRelease && event->button() == Qt::LeftButton){
					m_pressed = false;
				}


				if ( e->type() == QEvent::MouseButtonPress && event->button() == Qt::LeftButton){

					m_pressed = true;
					m_pressedx = event->x();
					m_pressedy = event->y();

					// 	m_presseda: screen center latitude  during mouse press
					// 	m_pressedb: screen center longitude during mouse press
					m_gpview->getGlobeSphericals(m_gpview->width() / 2, m_gpview->height() / 2, m_presseda, m_pressedb);

					if (m_globe->northPoleY() > 0){
						m_pressedb = pi - m_pressedb;
						m_presseda = pi + m_presseda;	 
					}
				}

				if (m_pressed == true){
					float radius = (float)(m_globe->getRadius());
					float deltax = (float)(event->x()-m_pressedx);
					float deltay = (float)(event->y()-m_pressedy);

					float direction = 1;

					if ( m_globe -> northPoleZ() > 0 ){	
						if (event->y() < ( m_globe->northPoleY() + m_gpview->height()/2))
							direction = -1;
					}
					else{	
						if (event->y() > (-m_globe->northPoleY() + m_gpview->height()/2))
							direction = -1;
					}

					m_globe->rotateTo(180/pi*(float)(-m_pressedb) + 90*deltay/radius, 
						180/pi*(float)(m_presseda) + 90*direction*deltax/radius);

					m_gpview->repaint();
				}
			}
			else {
				m_pressed = false;

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
				QWheelEvent* wheelevt = dynamic_cast<QWheelEvent*>(e);
				m_gpview->zoomViewBy((int)(-wheelevt->delta()/3));
				return TRUE;
			}
			else
				return FALSE;
		}
}
