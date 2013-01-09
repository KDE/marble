//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#ifndef MARBLE_LATLONRUNNER_H
#define MARBLE_LATLONRUNNER_H

#include "SearchRunner.h"

#include <QtCore/QString>

namespace Marble
{

class LatLonRunner : public SearchRunner
{
    Q_OBJECT
public:
    explicit LatLonRunner(QObject *parent = 0);
    ~LatLonRunner();
    virtual void search( const QString &searchTerm, const GeoDataLatLonAltBox &preferred );

};

}

#endif
