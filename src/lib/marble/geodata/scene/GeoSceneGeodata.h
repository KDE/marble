//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef MARBLE_GEOSCENEGEODATA_H
#define MARBLE_GEOSCENEGEODATA_H

#include <geodata_export.h>
#include "GeoSceneAbstractDataset.h"
#include "GeoDocument.h"

#include <QPen>
#include <QBrush>

namespace Marble
{

class GEODATA_EXPORT GeoSceneGeodata : public GeoSceneAbstractDataset
{
 public:
    explicit GeoSceneGeodata( const QString& name );
    virtual ~GeoSceneGeodata();
    virtual const char* nodeType() const;

    bool operator==( const GeoSceneGeodata &other ) const;

    QString property() const;
    void setProperty( const QString& property );

    QString sourceFile() const;
    void setSourceFile( const QString& sourceFile );

    QString colorize() const;
    void setColorize( const QString& colorize );

    qreal alpha() const;
    void setAlpha(qreal alpha);

    QPen pen() const;
    void setPen( const QPen& pen );

    QBrush brush() const;
    void setBrush( const QBrush& brush );

    int renderOrder() const;
    void setRenderOrder( int renderOrder );

    QVector<QColor> colors() const;
    void setColors(const QVector<QColor> &colors);

 private:
    QString m_property;
    QString m_sourceFile;
    QString m_colorize;
    qreal   m_alpha;
    QPen    m_pen;
    QBrush  m_brush;
    int m_renderOrder;
    QVector<QColor> m_colors;
};

}


#endif
