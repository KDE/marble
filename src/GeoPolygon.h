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
#ifndef GEOPOLYGON_H
#define GEOPOLYGON_H

#include <QtCore/QObject>
#include <QtCore/QVector>

#include "quaternion.h"
#include "GeoPoint.h"

#include <QtCore/QDebug>

/*
	GeoPoint defines the nodes in a polyLine 
*/

class GeoPolygon : public GeoPoint::Vector 
{
 public:
    GeoPolygon();
    ~GeoPolygon();

    int getNum() const { return m_Num; }
    bool getClosed() const { return m_closed; }
    void setClosed(bool closed){ m_closed = closed; }

    void setNum(int num){ m_Num = num; }

    bool getDateLine() const { return m_Crossed; }
    void setDateLine(bool crossed){ m_Crossed = crossed; }

    void setBoundary(int, int, int, int);
    GeoPoint::Vector getBoundary() const { return m_boundary; } 

    void displayBoundary(){
	Quaternion  q;
	float       lon;
	float       lat;

#if 0
	m_boundary.at(0).geoCoordinates(lon, lat);
	qDebug() << "Boundary:" << lon << ", " << lat << " Size: " << m_boundary.size();
	m_boundary.at(1).geoCoordinates(lon, lat);
	qDebug() << "Boundary:" << lon << ", " << lat;
	m_boundary.at(2).geoCoordinates(lon, lat);
	qDebug() << "Boundary:" << lon << ", " << lat;
	m_boundary.at(3).geoCoordinates(lon, lat);
	qDebug() << "Boundary:" << lon << ", " << lat;
	m_boundary.at(4).geoCoordinates(lon, lat);
#endif
	qDebug() << "Boundary:" << lon << ", " << lat;
	//		qDebug() << "Boundary:" << m_x0 << ", " << m_y0 << ", " << m_x1 << ", " << m_y1;
    }

    // Type definitions
    typedef QVector<GeoPolygon *> PtrVector;

 private:	
    bool m_Crossed;
    bool m_closed;

    GeoPoint::Vector m_boundary;

    int m_x0, m_y0, m_x1, m_y1;
    int m_Num;
};


class PntMap : public GeoPolygon::PtrVector
{
 public:
    PntMap();
    ~PntMap();

    void load(const QString &);
};


#endif // GEOPOLYGON_H
