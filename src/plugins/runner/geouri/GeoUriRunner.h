// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
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
