//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef TILECREATOR_H
#define TILECREATOR_H


#include <QtCore/QObject>
#include <QtCore/QString>


class TileCreator : public QObject
{
    Q_OBJECT

 public: 
    TileCreator( const QString& prefix, const QString& installmap, 
                 const QString& dem, const QString& targetDir=QString() );

 Q_SIGNALS:
    void  progress( int value );

 public Q_SLOTS:
    bool createTiles();

 private:
    QString  m_prefix;
    QString  m_installmap;
    QString  m_dem;
    QString  m_targetDir;
};


#endif
