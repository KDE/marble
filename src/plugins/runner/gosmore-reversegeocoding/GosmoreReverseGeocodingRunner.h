//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#ifndef MARBLE_GOSMOREREVERSEGEOCODINGRUNNER_H
#define MARBLE_GOSMOREREVERSEGEOCODINGRUNNER_H

#include "ReverseGeocodingRunner.h"

namespace Marble
{

class GosmoreRunnerPrivate;

class GosmoreRunner : public ReverseGeocodingRunner
{
public:
    explicit GosmoreRunner(QObject *parent = nullptr);

    ~GosmoreRunner() override;

    // Overriding MarbleAbstractRunner
    void reverseGeocoding( const GeoDataCoordinates &coordinates ) override;

private:
    GosmoreRunnerPrivate* const d;
};

}

#endif
