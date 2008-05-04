/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Simon Schmeißer <mail_to_wrt@gmx.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef GEOSHAPE_H
#define GEOSHAPE_H

#include <KoShape.h>

#define GEOSHAPEID "GeoShape"

class KoImageData;
class KoImageCollection;
class KUrl;
class MarbleMap;

class GeoShape : public KoShape
{
public:
    explicit GeoShape();
    virtual ~GeoShape();

    // reimplemented
    virtual void paint( QPainter& painter, const KoViewConverter& converter );
    // reimplemented
    virtual void saveOdf( KoShapeSavingContext & context ) const;
    // reimplemented
    virtual bool loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context );

    /// Load data from a file - data will be saved in odf - the referenced file is not modified
    bool loadFromUrl( KUrl & );

    /// reimplemented
    void init(QMap<QString, KoDataCenter *>  dataCenterMap);
    
    MarbleMap* marbleMap();

private:
    /// reimplemented from KoShape
    virtual KoShape * cloneShape() const;

    KoImageCollection *m_imageCollection;
    KoImageData *m_imageData;
    
    MarbleMap *m_marbleMap;
    
};


#endif
