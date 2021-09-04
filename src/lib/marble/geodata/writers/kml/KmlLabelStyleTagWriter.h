// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Constantin Mihalache <mihalache.c94@gmail.com>

#ifndef KMLLABELSTYLETAGWRITER_H
#define KMLLABELSTYLETAGWRITER_H

#include "KmlColorStyleTagWriter.h"
#include "GeoTagWriter.h"

namespace Marble
{

class KmlLabelStyleTagWriter : public KmlColorStyleTagWriter
{
public:
    KmlLabelStyleTagWriter();

protected:
    bool writeMid( const GeoNode *node, GeoWriter &writer ) const override;
    bool isEmpty( const GeoNode *node ) const override;
    QColor defaultColor() const override;
};

}
#endif // KMLLABELSTYLETAGWRITER_H
