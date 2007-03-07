#include "katlascrosshair.h"
#include "katlascrosshair.moc"
#include <QtCore/QDebug>

KAtlasCrossHair::KAtlasCrossHair(QObject* parent) : QObject(parent) {
	m_enabled = false;
}

void KAtlasCrossHair::paintCrossHair( QPainter* painter, int width, int height){

	if ( m_enabled == true ){ 
		int centerx = width / 2;
		int centery = height / 2;
		int halfsize = 5;

		painter->setPen(QColor(Qt::white));
		painter->drawLine( centerx - halfsize, centery, centerx + halfsize , centery );
		painter->drawLine( centerx, centery - halfsize, centerx, centery + halfsize );
	}	
}
