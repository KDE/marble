//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef TOURPLAYBACK_H
#define TOURPLAYBACK_H

#include <QObject>

#include "marble_export.h"

class QUrl;

namespace Marble
{

class MarbleWidget;
class GeoDataCoordinates;
class GeoDataTour;
class GeoDataPlacemark;
class GeoDataFeature;
class GeoDataContainer;
class PlaybackItem;

class TourPlaybackPrivate;

class MARBLE_EXPORT TourPlayback : public QObject
{
    Q_OBJECT
public:
    explicit TourPlayback(QObject *parent=0);
    ~TourPlayback();

    void setTour(GeoDataTour *tour);
    void setMarbleWidget( MarbleWidget *widget );

    /**
     * @brief setBaseUrl - sets base url for using in QWebView.
     */
    void setBaseUrl( const QUrl &baseUrl );

    /**
     * @brief baseUrl - gets base url which is using in QWebView.
     */
    QUrl baseUrl() const;

    /** Tour duration in seconds */
    double duration() const;
    bool isPlaying() const;

    void play();
    void pause();
    void stop();

    /**
     * Seek to the given timestamp (in seconds)
     * @param offset Target timestamp in seconds in the range 0..duration()
     */
    void seek( double offset );

    /** Size of main track (flyto, wait, tourcontrol primitives) **/
    int mainTrackSize();
    /**
     * Element of main track (flyto, wait, tourcontrol primitives)
     * @param i Position of element.
     */
    PlaybackItem* mainTrackItemAt( int i );

public Q_SLOTS:
    void updateTracks();
    void clearTracks();

Q_SIGNALS:
    void finished();
    void paused();
    void stopped();
    void progressChanged( double );
    void updated( GeoDataFeature* );
    void added( GeoDataContainer *parent, GeoDataFeature *feature, int row );
    void removed( const GeoDataFeature *feature  );
    void itemFinished( int index );

private Q_SLOTS:
    void stopTour();
    void showBalloon( GeoDataPlacemark* );
    void hideBalloon();
    void centerOn( const GeoDataCoordinates &coordinates );
    void handleFinishedItem( int index );

private:
    TourPlaybackPrivate * const d;
};

} // namespace Marble

#endif // TOURPLAYBACK_H
