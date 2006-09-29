//
// C++ Interface: pntmap
//
// Description: 
//
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PNTMAP_H
#define PNTMAP_H

#include <QObject>
#include <QVector>

#include "geopoint.h"
#include "quaternion.h"

#include <QDebug>

/*
	GeoPoint defines the nodes in a polyLine 
*/

class PntPolyLine : public GeoPoint::Vector {
public:
	PntPolyLine();
	~PntPolyLine();
	int getNum() const { return m_Num; }
	bool getClosed() const { return m_closed; }
	void setClosed(bool closed){ m_closed = closed; }
	void setNum(int num){ m_Num = num; }

	bool getDateLine() const { return m_Crossed; }
	void setDateLine(bool crossed){ m_Crossed = crossed; }

	void setBoundary(int, int, int, int);
	GeoPoint::Vector getBoundary() const { return m_boundary; } 

	void displayBoundary(){
		qDebug() << "Boundary:" << m_boundary.at(0).getLng() << ", " << m_boundary.at(0).getLat() << " Size: " << m_boundary.size();
		qDebug() << "Boundary:" << m_boundary.at(1).getLng() << ", " << m_boundary.at(1).getLat();
		qDebug() << "Boundary:" << m_boundary.at(2).getLng() << ", " << m_boundary.at(2).getLat();
		qDebug() << "Boundary:" << m_boundary.at(3).getLng() << ", " << m_boundary.at(3).getLat();
		qDebug() << "Boundary:" << m_boundary.at(4).getLng() << ", " << m_boundary.at(4).getLat();
//		qDebug() << "Boundary:" << m_x0 << ", " << m_y0 << ", " << m_x1 << ", " << m_y1;
	}

	// Type definitions
	typedef QVector<PntPolyLine *> PtrVector;

private:	
	bool m_Crossed;
	bool m_closed;

	GeoPoint::Vector m_boundary;

	int m_x0, m_y0, m_x1, m_y1;
	int m_Num;
};

class PntMap : public PntPolyLine::PtrVector {
public:
	PntMap();
	~PntMap();
	void load(const QString &);
};

#endif // PNTMAP_H
