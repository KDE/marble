//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Torsten Rahn <rahn@kde.org>
//

#ifndef MARBLE_LOCALDATABASERUNNER_H
#define MARBLE_LOCALDATABASERUNNER_H

#include "MarbleAbstractRunner.h"

#include <QtCore/QString>

namespace Marble
{

class LocalDatabaseRunner : public MarbleAbstractRunner
{
    Q_OBJECT
public:
    LocalDatabaseRunner(QObject *parent = 0);
    ~LocalDatabaseRunner();
    GeoDataFeature::GeoDataVisualCategory category() const;
    virtual void search( const QString &searchTerm );

};

}

#endif
