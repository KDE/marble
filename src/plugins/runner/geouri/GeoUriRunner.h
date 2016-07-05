//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016  Friedrich W. H. Kossebau <kossebau@kde.org>
//

#ifndef MARBLE_GEOURIRUNNER_H
#define MARBLE_GEOURIRUNNER_H

#include "SearchRunner.h"

namespace Marble
{

class GeoUriRunner : public SearchRunner
{
    Q_OBJECT

public:
    explicit GeoUriRunner(QObject *parent = nullptr);
    ~GeoUriRunner() override;

    void search(const QString &searchTerm, const GeoDataLatLonBox &preferred) override;
};

}

#endif
