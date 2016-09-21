//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#ifndef OBJECTHANDLER_H
#define OBJECTHANDLER_H

#include <QList>
#include <QDebug>

#include "GeoDataDocument.h"

namespace Marble {

class BaseFilter
{
public:
    explicit BaseFilter(GeoDataDocument* document);
    virtual ~BaseFilter();

    const QVector<GeoDataPlacemark*> & placemarks() const;
    QVector<GeoDataPlacemark*> & placemarks();

    const GeoDataDocument* document() const;
    GeoDataDocument* document();

private:
    Q_DISABLE_COPY(BaseFilter)

    GeoDataDocument* m_document;
    QVector<GeoDataPlacemark*> m_placemarks;
};

}

#endif // OBJECTHANDLER_H
