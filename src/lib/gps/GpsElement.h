//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//
#ifndef GPSELEMENT_H
#define GPSELEMENT_H

#include <QString>
#include <QUrl>
#include <QVector>

/*! \brief common details of every gps element
 *
 * This class is just to keep all of the common parts of the gps data
 * together.
 */
class GpsElement
{
 public:
    //!default constructor, set all pointers to 0
    GpsElement(): m_name(0), m_gpsComment(0), m_description(0),
                  m_source(0), m_link(0), m_number(0){}
    ~GpsElement();

    void setName( const QString &name );
    void setGpsComment( const QString &comment );
    void setDescription( const QString &Description );
    void setSource( const QString &source );
    //!add a previously verified link
    void addLink( const QString &link );
    void setNumber( int number );
 private:
    QString *m_name;
    QString *m_gpsComment;
    QString *m_description;
    /*!\brief source of the GPS data
     * 
     * Quoted from the GPX schema:
     * Source of data. Included to give user some idea of reliability
     * and accuracy of data.
     */
    QString *m_source;
    //!link to external info about the track
    QVector<QUrl> *m_link;
    //!not aplicable for waypoint
    int *m_number;
};

#endif //GPSELEMENT_H
