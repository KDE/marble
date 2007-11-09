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
 * GeoDataFeature is the base class for most geodata classes that
 * correspond to places on a map. It is never instantiated by itself,
 * but is always used as part of a derived class.
 *
 * @see GeoDataPlaceMark
 * @see GeoDataContainer
 */

// FIXME: Later also add NetworkLink and Overlay

class GeoDataFeature : public GeoDataObject
{
 public:

    /**
     * @brief  A categorization of a placemark as defined by ...FIXME.
     */
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

    /// Return the address of the feature
    QString address() const;
    /// Set the address of this feature to @p value.
    void setAddress( const QString &value);

    /// Return the phone number of the feature
    QString phoneNumber() const;
    /// Set the phone number of this feature to @p value.
    void setPhoneNumber( const QString &value);

    /// Return the text description of the feature.
    QString description() const;
    /// Set the description of this feature to @p value.
    void setDescription( const QString &value);

    /// Return whether this feature is visible or not
    bool isVisible() const;
    /**
     * @brief Set a new value for visibility
     * @param value  new value for the visibility
     *
     * This function sets the visibility, i.e. whether this feature
     * should be shown or not.  This can be changed either from a GUI
     * or through some action of the program.
     */ 
    void setVisible( bool value );

    /**
     * Return the style assigned to the placemark.
     */
    GeoDataStyle* style() const;
    /**
     * Sets the style of the placemark.
     * @param  style  the new style to be used.
     */
    void setStyle( GeoDataStyle* style );

    /**
     * Return the symbol index of the placemark.
     */
    const GeoDataVisualCategory visualCategory() const; 
    /**
     * Sets the symbol @p index of the placemark.
     * @param  category  the new category to be used.
     */
    void setVisualCategory( GeoDataVisualCategory category );

    /**
     * Return the role of the placemark.
     *
     * FIXME: describe roles here!
     */
    const QChar role() const;
    /**
     * Sets the role of the placemark.
     * @param  role  the new role to be used.
     */
    void setRole( const QChar &role );


    /**
     * @brief Return the popularity index of the placemark.
     *
     * The popularity index is a value which describes at which zoom
     * level the placemark will be shown.
     */
    const int popularityIndex() const;
    /**
     * Sets the popularity @p index of the placemark.
     * @param  index  the new index to be used.
     */
    void setPopularityIndex( int index );

    /**
     * Return the popularity of the placemark.
     */
    const qint64 popularity() const;
    /**
     * Sets the @p popularity of the placemark.
     * @param  popularity  the new popularity value
     */
    void setPopularity( qint64 popularity );

    // ----------------------------------------------------------------
    // The following functions are use for painting, and mostly for placemarks.

    /**
     * Return the symbol size of the feature in pixels.
     */
    const QSize symbolSize() const;

    /**
     * Return the symbol of the feature as a pixmap.
     *
     * Note that the pixmaps are shared between all features with the
     * same visual category
     */
    const QPixmap symbolPixmap() const;

    /**
     * Return the label font of the placemark.
     */
    static void resetDefaultStyles();


    /// Serialize the contents of the feature to @p stream.
    virtual void pack( QDataStream& stream ) const;
    /// Unserialize the contents of the feature from @p stream.
    virtual void unpack( QDataStream& stream );

 protected:
    /// Create a new GeoDataFeature with @p name as its name.
    GeoDataFeature( const QString& name );

    /// The visual category of this feature.  @see GeoDataVisualCategory
    GeoDataVisualCategory  m_visualCategory;

 private:
    static void initializeDefaultStyles();

 private:
    QString     m_name;         // Name of the feature. Is shown on screen
    QString     m_description;  // A longer textual description
    QString     m_address;      // The address.  Optional
    QString     m_phoneNumber;  // Phone         Optional
    qint64      m_popularity;   // Population(!)
    int         m_popularityIndex; // Index of population

    bool        m_visible;      // True if this feature should be shown.

    QChar       m_role;

    GeoDataStyleSelector* m_style;

    // Static members
    static GeoDataStyle* s_defaultStyle[GeoDataFeature::LastIndex];
    static bool          s_defaultStyleInitialized;
};

#endif // GEODATAFEATURE_H
