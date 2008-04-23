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

#include <QtCore/QUrl>
#include <QtCore/QVector>

/**
 * @brief common details of every gps element
 *
 * This class is just to keep all of the common parts of the gps data
 * together.
 */
class GpsElement
{
 public:
    /**
     * @brief constructor, set all pointers to 0;
     */
    GpsElement();
    
    /**
     * @brief destructor, delete all pointers
     */
    ~GpsElement();

    /**
     * @brief m_name setter
     */
    void setName( const QString &name );
    
    /**
     * @brief m_name getter
     * @return the name of this gps element or an empty string if one
     *         has not been provided
     */
    QString name();
    
    /**
     * @brief m_gpsComment setter
     */
    void setGpsComment( const QString &comment );
    
    /**
     * @brief m_gpsComment getter
     * @return a comment about this gps data or an empty string if one
     *         has not been provided
     */
    QString gpsComment();
    
    /**
     * @brief m_discription setter
     */
    void setDescription( const QString &description );
    
    /**
     * @brief m_discription getter
     * @return description about this particular gps data element or 
     *         an empty string if that information has not been 
     *         provided
     */
    QString description();
    
    /**
     * @brief m_source setter
     */
    void setSource( const QString &source );
    
    /**
     * @brief m_source getter
     * @return a string representation of the source of this data or 
     *         an empty string if that information is not known
     */
    QString source();
    
    /**
     * @brief add a previously verified link to m_links
     */
    void addLink( const QString &link );
    
    /**
     * @brief get all links
     * @return a vector containing all links or an empty vector
     */
    QVector<QUrl> links();
    
    /**
     * @brief m_number setter
     */
    void setNumber( int number );
    
    /**
     * @brief m_gpsComment setter
     * @return the number of this gps element or -1 if one has not
     *         been provided
     */
    int number();
    
 private:
    /**
     * @brief name used to identify this gps element.
     */
    QString *m_name;
    
    /**
     * @brief comment added to this element
     */
    QString *m_gpsComment;
    
    /**
     * @brief description about this gps element
     */
    QString *m_description;
    
    /*!\brief source of the GPS data
     * 
     * Quoted from the GPX schema:
     * Source of data. Included to give user some idea of reliability
     * and accuracy of data.
     */
    QString *m_source;
    
    /**
     * @brief collection of links to external information about this
     *        element. 
     * 
     * A Vector of valid links to external information
     */
    QVector<QUrl> *m_links;
    
    /**
     * @brief the number of the gps element
     * 
     * this referes to the number of this gps element in a sequence
     * of elements. this does not apply to waypoints
     */
    int *m_number;
};

#endif //GPSELEMENT_H
