/*
    SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-FileCopyrightText: 2007 Murad Tagirov <tmurad@gmail.com>
    SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEODATADOCUMENT_H
#define MARBLE_GEODATADOCUMENT_H

#include <QMetaType>

#include "geodata_export.h"

#include "GeoDataContainer.h"
#include "GeoDocument.h"
#include "GeoDataStyle.h"

namespace Marble
{

enum DocumentRole {
    UnknownDocument,
    MapDocument,
    UserDocument,
    TrackingDocument,
    BookmarkDocument,
    SearchResultDocument
};


class GeoDataStyleMap;
class GeoDataNetworkLinkControl;
class GeoDataSchema;

class GeoDataDocumentPrivate;

/**
 * @short A container for Features, Styles and in the future Schemas.
 *
 * A GeoDataDocument is a container for features, styles, and
 * schemas. This element is required if your KML file uses schemas or
 * shared styles. It is recommended that all Styles be defined in a
 * Document, each with an id, and then later referenced by a
 * styleUrl for a given Feature or StyleMap.
 */
class GEODATA_EXPORT GeoDataDocument : public GeoDocument,
                                       public GeoDataContainer
{
public:
    GeoDataDocument();
    GeoDataDocument( const GeoDataDocument& other );
    ~GeoDataDocument() override;

    GeoDataDocument& operator=(const GeoDataDocument& other);

    bool operator==( const GeoDataDocument &other ) const;
    bool operator!=( const GeoDataDocument &other ) const;

    const char* nodeType() const override;

    GeoDataFeature * clone() const override;

    /// Provides type information for downcasting a GeoData
    bool isGeoDataDocument() const override { return true; }

    DocumentRole documentRole() const;
    void setDocumentRole( DocumentRole role );

    QString property() const;
    void setProperty( const QString& property );

    /**
     * @brief The filename of the document
     *
     * The filename of the document is used internally to identify the files.
     * it should never be empty as this could lead to potential collisions.
     *
     * @return The filename of this document
     */
    QString fileName() const;
    /**
     * @brief Set a new file name for this document
     * @param value  the new name
     */
    void setFileName( const QString &value );

    /**
     * @brief The URI relative paths should be resolved against
     */
    QString baseUri() const;

    /**
     * @brief Change the URI for resolving relative paths.
     * See https://tools.ietf.org/html/rfc3986#section-5
     */
    void setBaseUri( const QString &baseUri );

    /**
     * @brief the NetworkLinkControl of the file
     */
    GeoDataNetworkLinkControl networkLinkControl() const;

    /**
     * @brief set the NetworkLinkControl of the file
     */
    void setNetworkLinkControl( const GeoDataNetworkLinkControl &networkLinkControl );

    /**
     * @brief Add a style to the style storage
     * @param style  the new style
     */
    void addStyle(const GeoDataStyle::Ptr &style);

    /**
     * @brief Add a style to the style storage
     * @param styleId  the new style
     */
    void removeStyle( const QString& styleId );

    /**
     * @brief Return a style in the style storage
     * @param styleId  the id of the style
     */
    GeoDataStyle::Ptr style( const QString& styleId );
    GeoDataStyle::ConstPtr style( const QString& styleId ) const;

    /**
    * @brief dump a Vector of all styles
    */
    QList<GeoDataStyle::Ptr> styles();
    QList<GeoDataStyle::ConstPtr> styles() const;

    /**
    * @brief Add a stylemap to the stylemap storage
    * @param map  the new stylemap
    */
    void addStyleMap( const GeoDataStyleMap& map );

    /**
    * @brief remove stylemap from storage
    * @param mapId the styleId of the styleMap to be removed
    */
    void removeStyleMap( const QString& mapId );

    /**
     * @brief Return a style in the style storage
     * @param styleId  the id of the style
     */
    GeoDataStyleMap& styleMap( const QString& styleId );
    GeoDataStyleMap styleMap( const QString& styleId ) const;

    /**
    * @brief dump a Vector of all stylemaps
    */
    QList<GeoDataStyleMap> styleMaps() const;

    /**
     * @brief Add a schema to simplemap storage
     * @param schema  the new schema
     */
    void addSchema( const GeoDataSchema& schema );

    /**
     * @brief remove a schema from schema storage
     * @param schemaId  the of schema to be removed
     */
    void removeSchema( const QString& schemaId );

    /**
     * @brief Returns a schema with id = schemaId form schema storage
     * @param schemaId  The id of schema to be returned
     */
    GeoDataSchema schema( const QString& schemaId ) const;
    GeoDataSchema &schema( const QString& schemaId );

    /**
     * @brief dump a vector of all schemas
     */
    QList<GeoDataSchema> schemas() const;

    // Serialize the Placemark to @p stream
    void pack( QDataStream& stream ) const override;
    // Unserialize the Placemark from @p stream
    void unpack( QDataStream& stream ) override;

private:
    Q_DECLARE_PRIVATE(GeoDataDocument)
};

}
Q_DECLARE_METATYPE(Marble::GeoDataDocument*)
#endif
