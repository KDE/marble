//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_DATAMIGRATION_H
#define MARBLE_DATAMIGRATION_H

#include <QtCore/QObject>

namespace Marble
{

class DataMigration : public QObject
{
    Q_OBJECT

 public:
    explicit DataMigration( QObject *parent );
    virtual ~DataMigration();

 public Q_SLOTS:
    void exec();

 private:
    static void moveFiles( const QString& source, const QString& target );
};

}

#endif // MARBLE_DATAMIGRATION_H
