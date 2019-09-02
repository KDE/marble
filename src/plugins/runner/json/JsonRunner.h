/*
    This file is part of the Marble Virtual Globe.

    This program is free software licensed under the GNU LGPL. You can
    find a copy of this license in LICENSE.txt in the top directory of
    the source code.

    Copyright 2013 Ander Pijoan <ander.pijoan@deusto.es>
    Copyright 2019 John Zaitseff <J.Zaitseff@zap.org.au>
*/

#ifndef MARBLE_JSONRUNNER_H
#define MARBLE_JSONRUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class JsonRunner : public ParsingRunner
{
    Q_OBJECT
public:
    explicit JsonRunner(QObject *parent = nullptr);
    ~JsonRunner() override;
    GeoDataDocument* parseFile(const QString &fileName, DocumentRole role, QString& error) override;
};

}

#endif // MARBLE_JSONRUNNER_H
