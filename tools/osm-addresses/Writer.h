// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_WRITER_H
#define MARBLE_WRITER_H

#include "OsmPlacemark.h"
#include "OsmRegion.h"

#include <QObject>

namespace Marble
{

class Writer : public QObject
{
    Q_OBJECT

public:
    explicit Writer(QObject *parent = nullptr);

    virtual void addOsmRegion(const OsmRegion &region) = 0;

    virtual void addOsmPlacemark(const OsmPlacemark &placemark) = 0;
};

}

#endif // MARBLE_WRITER_H
