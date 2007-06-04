//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef GEOPOLYGON_H
#define GEOPOLYGON_H

#include <QtCore/QObject>
#include <QtCore/QVector>

#include "Quaternion.h"
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

    int  getNum() const { return m_num; }
    bool getClosed() const { return m_closed; }
    void setClosed(bool closed){ m_closed = closed; }

    void setNum(int num){ m_num = num; }

    bool getDateLine() const { return m_crossed; }
    void setDateLine(bool crossed){ m_crossed = crossed; }

    void setBoundary(int, int, int, int);
    GeoPoint::Vector getBoundary() const { return m_boundary; } 

    void displayBoundary(){
	Quaternion  q;
	float       lon;
	float       lat;

	m_boundary.at(0).geoCoordinates(lon, lat);
	qDebug() << "Boundary:" << lon << ", " << lat << " Size: " << m_boundary.size();
	m_boundary.at(1).geoCoordinates(lon, lat);
	qDebug() << "Boundary:" << lon << ", " << lat;
	m_boundary.at(2).geoCoordinates(lon, lat);
	qDebug() << "Boundary:" << lon << ", " << lat;
	m_boundary.at(3).geoCoordinates(lon, lat);
	qDebug() << "Boundary:" << lon << ", " << lat;
	m_boundary.at(4).geoCoordinates(lon, lat);
	qDebug() << "Boundary:" << lon << ", " << lat;
	//		qDebug() << "Boundary:" << m_x0 << ", " << m_y0 << ", " << m_x1 << ", " << m_y1;
    }

    // Type definitions
    typedef QVector<GeoPolygon *> PtrVector;

 private:	
    bool  m_crossed;
    bool  m_closed;

    GeoPoint::Vector  m_boundary;

    int   m_x0;
    int   m_y0;
    int   m_x1;
    int   m_y1;
    int   m_num;
};


/*
 * A PntMap is a collection of GeoPolygons, i.e. a complete map of vectors.
 *
 * FIXME: Rename it (into GeoPolygonMap?)
 */

class PntMap : public GeoPolygon::PtrVector
{
 public:
    PntMap();
    ~PntMap();

    void load(const QString &);
};


#endif // GEOPOLYGON_H
