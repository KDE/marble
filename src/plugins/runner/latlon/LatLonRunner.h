// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Henry de Valence <hdevalence@gmail.com>

#ifndef MARBLE_LATLONRUNNER_H
#define MARBLE_LATLONRUNNER_H

#include "SearchRunner.h"

#include <QString>

namespace Marble
{

class LatLonRunner : public SearchRunner
{
    Q_OBJECT
public:
    explicit LatLonRunner(QObject *parent = nullptr);
    ~LatLonRunner() override;
    void search( const QString &searchTerm, const GeoDataLatLonBox &preferred ) override;

};

}

#endif
