/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#ifndef MARBLE_JSONRUNNER_H
#define MARBLE_JSONRUNNER_H

#include "MarbleAbstractRunner.h"

namespace Marble
{

class JsonRunner : public MarbleAbstractRunner
{
    Q_OBJECT
public:
    explicit JsonRunner(QObject *parent = 0);
    ~JsonRunner();
    GeoDataFeature::GeoDataVisualCategory category() const;
    virtual void parseFile( const QString &fileName, DocumentRole role );
};

}
#endif // MARBLE_JSONRUNNER_H
