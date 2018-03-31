//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
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
