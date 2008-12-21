//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//
#ifndef PLACEMARKLOADER_H
#define PLACEMARKLOADER_H

#include <QtCore/QString>
#include <QtCore/QThread>

namespace Marble
{
class MarblePlacemarkModel;
class PlaceMarkContainer;

class PlaceMarkLoader : public QThread {
    Q_OBJECT
    public:
        PlaceMarkLoader( QObject* parent, const QString& file );

        void run();
    Q_SIGNALS:
        void placeMarksLoaded( PlaceMarkLoader*, PlaceMarkContainer * );
    private:
        bool loadFile( const QString& filename, PlaceMarkContainer* placeMarkContainer );
        void saveFile( const QString& filename, PlaceMarkContainer* placeMarkContainer );
        void importKml( const QString& filename, PlaceMarkContainer* placeMarkContainer );

        QString filepath;
};

} // namespace Marble
#endif // PLACEMARKLOADER_H
