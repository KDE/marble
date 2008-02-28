/*
    Copyright (C) 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
    Copyright (C) 2007 Murad Tagirov <tmurad@gmail.com>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef GeoDataDocument_h
#define GeoDataDocument_h

#include <QHash>
#include <QVector>

#include "GeoDataContainer.h"
#include "GeoDocument.h"

class GeoDataFolder;
class GeoDataStyle;

/**
 * @short A container for Features, Styles and in the future Schemas.
 *
 * A GeoDataDocument is a container for features, styles, and
 * schemas. This element is required if your KML file uses schemas or
 * shared styles. It is recommended that all Styles be defined in a
 * Document, each with an id, and then later referenced by a
 * styleUrl for a given Feature or StyleMap.
 */
class GeoDataDocument : public GeoDocument,
                        public GeoDataContainer {
public:
    GeoDataDocument();
    ~GeoDataDocument();

    virtual bool isGeoDataDocument() const { return true; }

    /**
     * @brief Add a folder to the document
     * @param style the new folder
     */
    void addFolder(GeoDataFolder*);

    /**
     * @brief A convenience function that returns all folders in the document.
     * @return A QVector of GeoDataFolder*
     *
     * @see GeoDataFolder
     */
    const QVector<GeoDataFolder*> folders() const;

    /**
     * @brief Add a style to the style storage
     * @param style  the new style
     */
    void addStyle(GeoDataStyle*);

    /**
     * @brief Return a style in the style storage
     * @param styleId  the id of the style
     */
    const GeoDataStyle* style(const QString& styleId) const;

private:
    QHash<QString, GeoDataStyle*> m_styleHash;
    QVector<GeoDataFolder*> m_folders;
};


#endif // GeoDataDocument_h
