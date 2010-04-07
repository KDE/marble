//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#ifndef DATAMIGRATION_H
#define DATAMIGRATION_H

#include<QtCore/QObject>

class DataMigration : public QObject
{
    Q_OBJECT

 public:
    explicit DataMigration( QObject *parent );
    virtual ~DataMigration();

 public slots:
    void exec();

 private:
    static void moveFiles( const QString& source, const QString& target );
};

#endif // MARBLE_DATAMIGRATION_H
