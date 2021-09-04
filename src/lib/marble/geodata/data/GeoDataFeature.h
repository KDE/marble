// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Murad Tagirov <tmurad@gmail.com>
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef MARBLE_GEODATAFEATURE_H
#define MARBLE_GEODATAFEATURE_H

#include "GeoDataObject.h"

#include "geodata_export.h"

namespace Marble
{

// forward define all features we can find.
class GeoDataRegion;
class GeoDataAbstractView;

class GeoDataStyle;
class GeoDataStyleMap;

class GeoDataExtendedData;

class GeoDataTimeSpan;
class GeoDataTimeStamp;

class GeoDataFeaturePrivate;

class GeoDataSnippet;

/**
 * @short A base class for all geodata features
 *
 * GeoDataFeature is the base class for most geodata classes that
 * correspond to places on a map. It is never instantiated by itself,
 * but is always used as part of a derived class.
 *
 * @see GeoDataPlacemark
 * @see GeoDataContainer
 */
class GEODATA_EXPORT GeoDataFeature : public GeoDataObject
{
 public:
    GeoDataFeature();
    /// Create a new GeoDataFeature with @p name as its name.
    explicit GeoDataFeature( const QString& name );

    GeoDataFeature( const GeoDataFeature& other );

    ~GeoDataFeature() override;

    GeoDataFeature& operator=( const GeoDataFeature& other );

    bool operator==(const GeoDataFeature &other) const;
    inline bool operator!=(const GeoDataFeature &other) const { return !(*this == other); }

    EnumFeatureId featureId() const;

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

    /**
     * @brief A short description of the feature.
     *
     * HTML markup is not supported.
     * @todo When the Snippet is not supplied, the first lines of description should be used.
     * @return The name of this feature
     */
    GeoDataSnippet snippet() const;
    /**
     * @brief Set a new name for this feature
     * @param value  the new name
     */
    void setSnippet( const GeoDataSnippet &value );

    /// Return the address of the feature
    QString address() const;
    /// Set the address of this feature to @p value.
    void setAddress( const QString &value);

    /// Return the phone number of the feature
    QString phoneNumber() const;
    /// Set the phone number of this feature to @p value.
    void setPhoneNumber( const QString &value );

    /// Return the text description of the feature.
    QString description() const;
    /// Set the description of this feature to @p value.
    void setDescription( const QString &value );

    /**
     * @brief test if the description is CDATA or not
     * CDATA allows for special characters to be included in XML and also allows
     * for other XML formats to be embedded in the XML without interfering with
     * parser namespace.
     * @return @c true if the description should be treated as CDATA
     *         @c false if the description is a plain string
     */
    bool descriptionIsCDATA() const;
    /// Set the description to be CDATA See: @see descriptionIsCDATA()
    void setDescriptionCDATA( bool cdata );

    /// Get the Abstract view of the feature
    const GeoDataAbstractView *abstractView() const;
    GeoDataAbstractView *abstractView();
    /// Set the abstract view of the feature
    void setAbstractView( GeoDataAbstractView *abstractView );

    /// Return the styleUrl of the feature.
    QString styleUrl() const;
    /// Set the styleUrl of this feature to @p value.
    void setStyleUrl( const QString &value );

    /// Return whether this feature is visible or not
    bool isVisible() const;

    /// Return whether this feature is visible or not in the context of its parenting
    bool isGloballyVisible() const;

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
     * Return the timespan of the feature.
     */
    const GeoDataTimeSpan& timeSpan() const;
    GeoDataTimeSpan& timeSpan();

    /**
     * Set the timespan of the feature.
     * @param timeSpan new of timespan.
     */
    void setTimeSpan( const GeoDataTimeSpan &timeSpan );

    /**
     * Return the timestamp of the feature.
     */
    const GeoDataTimeStamp& timeStamp() const;
    GeoDataTimeStamp& timeStamp();

    /**
     * Set the timestamp of the feature.
     * @param timeStamp new of the timestamp.
     */
    void setTimeStamp( const GeoDataTimeStamp &timeStamp );

    /**
     * Return the style assigned to the placemark, or a default style if none has been set
     */
    QSharedPointer<const GeoDataStyle> style() const;
    /**
     * Return the style assigned to the placemark with setStyle (can be 0)
     */
    QSharedPointer<const GeoDataStyle> customStyle() const;
    /**
     * Sets the style of the placemark.
     * @param  style  the new style to be used.
     */
    void setStyle( const QSharedPointer<GeoDataStyle> &style );

    /**
     * Return the ExtendedData assigned to the feature.
     */
    GeoDataExtendedData& extendedData();
    const GeoDataExtendedData& extendedData() const;

    /**
     * Sets the ExtendedData of the feature.
     * @param  extendedData  the new ExtendedData to be used.
     */
    void setExtendedData( const GeoDataExtendedData& extendedData );

    /**
     * Return the region assigned to the placemark.
     */
    const GeoDataRegion& region() const;
    GeoDataRegion& region();
    /**
     * @brief Sets the region of the placemark.
     * @param region new value for the region
     *
     * The feature is only shown when the region if active.
     */
    void setRegion( const GeoDataRegion& region );

    /**
     * Return the role of the placemark.
     *
     * FIXME: describe roles here!
     */
    const QString role() const;
    /**
     * Sets the role of the placemark.
     * @param  role  the new role to be used.
     */
    void setRole( const QString &role );

    /**
     * @brief Return the popularity index of the placemark.
     *
     * The popularity index is a value which describes at which zoom
     * level the placemark will be shown.
     */
    int zoomLevel() const;
    /**
     * Sets the popularity @p index of the placemark.
     * @param  index  the new index to be used.
     */
    void setZoomLevel( int index );

    /**
     * Return the popularity of the feature.
     */
    qint64 popularity() const;
    /**
     * Sets the @p popularity of the feature.
     * @param  popularity  the new popularity value
     */
    void setPopularity( qint64 popularity );

    /**
     * Return a pointer to a GeoDataStyleMap object which represents the styleMap
     * of this feature. A styleMap is simply a QMap<QString,QString> which can connect
     * two styles with a keyword. This can be used to have a highlighted and a
     * normal style.
     * @see GeoDataStyleMap
     */
    const GeoDataStyleMap* styleMap() const;
    /**
     * Sets the styleMap of the feature
     */
    void setStyleMap( const GeoDataStyleMap* map );

    /// Duplicate into another equal instance
    virtual GeoDataFeature * clone() const = 0;


    /// Serialize the contents of the feature to @p stream.
    void pack( QDataStream& stream ) const override;
    /// Unserialize the contents of the feature from @p stream.
    void unpack( QDataStream& stream ) override;

 protected:
    // the d-pointer needs to be protected to be accessible from derived classes
    GeoDataFeaturePrivate* const d_ptr;
    explicit GeoDataFeature(GeoDataFeaturePrivate* dd);
    GeoDataFeature(const GeoDataFeature& other, GeoDataFeaturePrivate* dd);

    bool equals( const GeoDataFeature &other ) const;
    using GeoDataObject::equals;

 private:
    Q_DECLARE_PRIVATE(GeoDataFeature)
};

}

#endif
