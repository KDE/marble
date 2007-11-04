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
 * derived class.
 *
 * @see GeoDataPlaceMark
 * @see GeoDataContainer
 */

// FIXME: Later also add NetworkLink and Overlay

class GeoDataFeature : public GeoDataObject
{
 public:

    enum GeoDataVisualCategory {
        None,
        Default,
        Unknown,

        // The order of the cities needs to stay fixed as the
        // algorithms rely on that.
        SmallCity,
        SmallCountyCapital,  
        SmallStateCapital,   
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

        // Other types of features
        Mountain,
        Volcano,
        GeographicPole,
        MagneticPole,
        ShipWreck,
        AirPort,
        Continent,
        Ocean,

        // Important: Make sure that this is always the last 
        // item and just use it to specify the array size
        LastIndex
    };

    GeoDataFeature();
    virtual ~GeoDataFeature();

    /**
     * @brief The name of the feature
     *
     * The name of the feature should be a short string. It is often
     * shown directly on the map and need therefore not take up much
     * space.
     *
     * @return The name of this feature
     */
    QString name() const;
    /**
     * @brief Set a new name for this feature
     * @param value  the new name
     */
    void setName( const QString &value );

    QString address() const;
    void setAddress( const QString &value);

    QString phoneNumber() const;
    void setPhoneNumber( const QString &value);

    QString description() const;
    void setDescription( const QString &value);

    /// Return whether this feature is visible or not
    bool isVisible() const;
    /**
     * @brief Set a new value for visibility
     * @param value  new value for the visibilty
     *
     * This function sets the visibility, i.e. whether this feature
     * should be shown or not.  This can be changed either from a GUI
     * or through some action of the program.
     */ 
    void setVisible( bool value );

    /**
     * Returns the style assigned to the placemark.
     */
    GeoDataStyle* style() const;

    /**
     * Sets the style of the placemark.
     */
    void setStyle( GeoDataStyle* style );

    /**
     * Returns the symbol index of the placemark.
     */
    const GeoDataFeature::GeoDataVisualCategory visualCategory() const; 

    /**
     * Sets the symbol @p index of the placemark.
     */
    void setVisualCategory( GeoDataFeature::GeoDataVisualCategory index );

    /**
     * Returns the role of the placemark.
     *
     * TODO: describe roles here!
     */
    const QChar role() const;

    /**
     * Sets the role of the placemark.
     */
    void setRole( const QChar &role );


    /**
     * Returns the popularity index of the placemark.
     *
     * The popularity index is a value which describes
     * at which zoom level the placemark will be shown.
     */
    const int popularityIndex() const;

    /**
     * Sets the popularity @p index of the placemark.
     */
    void setPopularityIndex( int index );

    /**
     * Returns the popularity of the placemark.
     */
    const qint64 popularity() const;

    /**
     * Sets the @p popularity of the placemark.
     */
    void setPopularity( qint64 popularity );

    /**
     * Returns the symbol size of the placemark.
     */
    const QSize symbolSize() const;

    /**
     * Returns the symbol of the placemark.
     */
    const QPixmap symbolPixmap() const;

    /**
     * Returns the label font of the placemark.
     */
    static void resetDefaultStyles();


    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

 protected:
    GeoDataFeature( const QString& name );

    GeoDataVisualCategory m_visualCategory;

 private:
    static void initializeDefaultStyles();

 private:
    QString     m_name;
    QString     m_description;
    QString     m_address;
    QString     m_phoneNumber;

    bool        m_visible;      // True if this feature should be shown.

    GeoDataStyleSelector* m_style;

    QChar m_role;

    qint64  m_popularity;
    int     m_popularityIndex;

    static GeoDataStyle* s_defaultStyle[GeoDataFeature::LastIndex];
    static bool          s_defaultStyleInitialized;
};

#endif // GEODATAFEATURE_H
