//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLEPNTRUNNER_H
#define MARBLEPNTRUNNER_H

#include "MarbleAbstractRunner.h"

namespace Marble
{

class PntRunner : public MarbleAbstractRunner
{
    Q_OBJECT
public:
    explicit PntRunner(QObject *parent = 0);
    ~PntRunner();
    GeoDataFeature::GeoDataVisualCategory category() const;
    virtual void parseFile( const QString &fileName, DocumentRole role );

signals:

public slots:

};

}
#endif // MARBLEPNTRUNNER_H
