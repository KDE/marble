//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#ifndef MARBLE_GEOPOLYGON_H
#define MARBLE_GEOPOLYGON_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QVector>
#include "marble_export.h"

#include "GeoDataCoordinates.h"

namespace Marble
{

/*
    NOTE: USAGE OF THIS CLASS IS DEPRECATED
          Use GeoDataLineString, GeoDataLinearRing OR GeoDataPolygon instead!

	GeoDataPoint defines the nodes in a polyLine 
*/

class MARBLE_EXPORT GeoPolygon : public GeoDataCoordinates::Vector
{
 public:
    GeoPolygon();
    virtual ~GeoPolygon();

    /**
     * @brief enum used to specify how a polyline crosses the IDL
     *
     * "None" means that the polyline doesn't cross the 
     * International Dateline (IDL). 
     *
     * "Odd" means that the polyline crosses the IDL. The number 
     * of times that the IDL is being crossed is odd. As a result
     * the polyline covers the whole range of longitude and the 
     * feature described by the polyline contains one of the poles 
     * (example: Antarctica).  
     * International Dateline (IDL). 
     * "Even" means that each time the polyline crosses the IDL it 
     * also returns back to the original side later on by crossing
     * the IDL again (example: Russia).
     */

    enum DateLineCrossing{None, Odd, Even};

    int  getIndex() const { return m_index; }
    bool getClosed() const { return m_closed; }
    void setClosed( bool closed ){ m_closed = closed; }

    void setIndex( int index ){ m_index = index; }

    int getDateLine() const { return m_dateLineCrossing; }
    void setDateLine( int dateLineCrossing ){ m_dateLineCrossing = dateLineCrossing; }

    void setBoundary( qreal, qreal, qreal, qreal );
    GeoDataCoordinates::PtrVector getBoundary() const { return m_boundary; }

    void displayBoundary();

    // Type definitions
    typedef QVector<GeoPolygon *> PtrVector;

//    QString m_sourceFileName;

 private:
    int   m_dateLineCrossing;
    bool  m_closed;

    GeoDataCoordinates::PtrVector  m_boundary;

    qreal  m_lonLeft;
    qreal  m_latTop;
    qreal  m_lonRight;
    qreal  m_latBottom;
    int     m_index;
};


/*
 * A PntMap is a collection of GeoPolygons, i.e. a complete map of vectors.
 *
 * FIXME: Rename it (into GeoPolygonMap?)
 */

class PntMapLoader;

class MARBLE_EXPORT PntMap : public QObject,
                              public GeoPolygon::PtrVector
{
    Q_OBJECT
 public:
    PntMap();
    ~PntMap();

    bool isInitialized() const;

    void load( const QString & );

 Q_SIGNALS:
    void initialized();

 private Q_SLOTS:
    void setInitialized( bool );

 private:
    bool m_isInitialized;
    PntMapLoader* m_loader;

    Q_DISABLE_COPY( PntMap )
};

class MARBLE_EXPORT PntMapLoader : public QThread
{
    Q_OBJECT
    public:
        PntMapLoader( PntMap* parent, const QString& filename );

        void run();
    Q_SIGNALS:
        void pntMapLoaded( bool );

    private:
        PntMap *m_parent;
        QString m_filename;
};

}

#endif
