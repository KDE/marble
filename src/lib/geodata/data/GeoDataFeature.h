//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef GEODATAFEATURE_H
#define GEODATAFEATURE_H


#include <QtCore/QString>
#include <QtCore/QSize>
#include <QtGui/QPixmap>
#include <QtGui/QFont>
#include <QtGui/QColor>

#include "GeoDataObject.h"

class GeoDataStyle;
class GeoDataStyleSelector;


/**
 * @short A base class for all geodata features
 *
 * GeoDataFeature is the base class for all geodata classes. It is
 * never instantiated by itself, but is always used as part of a
 * derived object.
 */

class GeoDataFeature : public GeoDataObject
{
 public:

    enum GeoDataVisualCategory{  None,
                       Default,
                       Unknown,
                       SmallCity,           // The order of the cities needs 
                       SmallCountyCapital,  // to stay fixed as the algorithms
                       SmallStateCapital,   // rely on that.
                       SmallNationCapital,
                       MediumCity,
                       MediumCountyCapital,
                       MediumStateCapital,
                       MediumNationCapital,
                       BigCity,
                       BigCountyCapital,
                       BigStateCapital,
                       BigNationCapital,
                       LargeCity,
                       LargeCountyCapital,
                       LargeStateCapital,
                       LargeNationCapital,
                       Mountain,
                       Volcano,
                       GeographicPole,
                       MagneticPole,
                       ShipWreck,
                       AirPort,
                       Continent,
                       Ocean,
                       LastIndex // Important: Make sure that this is always the last 
                                 // item and just use it to specify the array size
                   };

    GeoDataFeature();
    virtual ~GeoDataFeature();

    QString name() const;
    void setName( const QString &value );

    QString address() const;
    void setAddress( const QString &value);

    QString phoneNumber() const;
    void setPhoneNumber( const QString &value);

    QString description() const;
    void setDescription( const QString &value);

    bool isVisible() const;
    void setVisible( bool value );

    /**
     * Returns the style assigned to the place mark.
     */
    GeoDataStyle* style() const;

    /**
     * Sets the style of the place mark.
     */
    void setStyle( GeoDataStyle* style );

    /**
     * Returns the symbol index of the place mark.
     */
    const GeoDataFeature::GeoDataVisualCategory visualCategory() const; 

    /**
     * Sets the symbol @p index of the place mark.
     */
    void setVisualCategory( GeoDataFeature::GeoDataVisualCategory index );

    /**
     * Returns the role of the place mark.
     *
     * TODO: describe roles here!
     */
    const QChar role() const;

    /**
     * Sets the role of the place mark.
     */
    void setRole( const QChar &role );


    /**
     * Returns the popularity index of the place mark.
     *
     * The popularity index is a value which describes
     * at which zoom level the place mark will be shown.
     */
    const int popularityIndex() const;

    /**
     * Sets the popularity @p index of the place mark.
     */
    void setPopularityIndex( int index );

    /**
     * Returns the popularity of the place mark.
     */
    const qint64 popularity() const;

    /**
     * Sets the @p popularity of the place mark.
     */
    void setPopularity( qint64 popularity );

    /**
     * Returns the symbol size of the place mark.
     */
    const QSize symbolSize() const;

    /**
     * Returns the symbol of the place mark.
     */
    const QPixmap symbolPixmap() const;

    /**
     * Returns the label font of the place mark.
     */
    static void resetDefaultStyles();


    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

 protected:
    GeoDataFeature( const QString& name );

    GeoDataVisualCategory m_visualCategory;

 private:

    QString     m_name;
    QString     m_address;
    QString     m_phoneNumber;
    QString     m_description;
    bool        m_visible;

    GeoDataStyleSelector* m_style;

    QChar m_role;

    qint64 m_popularity;

    int m_popularityIndex;

    static GeoDataStyle* s_defaultStyle[GeoDataFeature::LastIndex];

    static bool s_defaultStyleInitialized;
    static void initializeDefaultStyles();
};

#endif // GEODATAFEATURE_H
