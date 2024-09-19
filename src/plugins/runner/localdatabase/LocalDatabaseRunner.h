// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Torsten Rahn <rahn@kde.org>
//

#ifndef MARBLE_LOCALDATABASERUNNER_H
#define MARBLE_LOCALDATABASERUNNER_H

#include "SearchRunner.h"

namespace Marble
{

class LocalDatabaseRunner : public SearchRunner
{
    Q_OBJECT
public:
    explicit LocalDatabaseRunner(QObject *parent = nullptr);
    ~LocalDatabaseRunner() override;

    void search(const QString &searchTerm, const GeoDataLatLonBox &preferred) override;
};

}

#endif
