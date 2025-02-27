// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mihail Ivchenko <ematirov@gmail.com>
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef MARBLE_TOURWIDGET_H
#define MARBLE_TOURWIDGET_H

#include "marble_export.h"

#include <QWidget>

class QModelIndex;

namespace Marble
{

class GeoDataFeature;
class GeoDataContainer;
class MarbleWidget;

class TourWidgetPrivate;

class MARBLE_EXPORT TourWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TourWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~TourWidget() override;

    void setMarbleWidget(MarbleWidget *widget);
    bool openTour(const QString &filename);
    bool isPlaying() const;

    bool eventFilter(QObject *watched, QEvent *event) override;

public Q_SLOTS:
    void startPlaying();
    void pausePlaying();
    void togglePlaying();
    void stopPlaying();
    void handleSliderMove(int);
    /**
     * Highlights the item currently being played.
     */
    void setHighlightedItemIndex(int index);

Q_SIGNALS:
    void featureUpdated(GeoDataFeature *feature);
    void featureAdded(GeoDataContainer *parent, GeoDataFeature *feature, int row);
    void featureRemoved(const GeoDataFeature *feature);

private Q_SLOTS:
    void moveUp();
    void moveDown();
    void addFlyTo();
    void addWait();
    void addSoundCue();
    void addPlacemark();
    void addRemovePlacemark();
    void addChangePlacemark();
    void deleteSelected();
    void updateDuration();
    void finishAddingItem();
    void stopLooping();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Q_PRIVATE_SLOT(d, void openFile())
    Q_PRIVATE_SLOT(d, void createTour())
    Q_PRIVATE_SLOT(d, void saveTour())
    Q_PRIVATE_SLOT(d, void saveTourAs())
    Q_PRIVATE_SLOT(d, void captureTour())
    Q_PRIVATE_SLOT(d, void updateButtonsStates())
    Q_PRIVATE_SLOT(d, void mapCenterOn(const QModelIndex &index))
    Q_PRIVATE_SLOT(d, void handlePlaybackProgress(const double position))
    Q_DISABLE_COPY(TourWidget)

    void removeHighlight();
    TourWidgetPrivate *const d;
};

}

#endif
