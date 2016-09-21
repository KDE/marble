//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef SERIALTRACK_H
#define SERIALTRACK_H

#include <QObject>

#include "PlaybackItem.h"

namespace Marble
{
class GeoDataCoordinates;

class SerialTrack : public QObject
{
    Q_OBJECT
public:
    SerialTrack();
    ~SerialTrack();

    void append( PlaybackItem* item );
    void play();
    void seek( double position );
    double duration() const;
    void clear();
    int size() const;
    PlaybackItem* at( int i );
    double currentPosition();

Q_SIGNALS:
    void centerOn( const GeoDataCoordinates &coordinates );
    void progressChanged( double );
    void finished();
    void paused();
    void itemFinished( int index );

public Q_SLOTS:
    void handleFinishedItem();
    void changeProgress( double );
    void pause();
    void stop();

private:
    QList<PlaybackItem*> m_items;
    int m_currentIndex;
    double m_finishedPosition;
    double m_currentPosition;
    bool m_paused;
};

}
#endif
