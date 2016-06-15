//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DECLARATIVE_NAVIGATION_H
#define MARBLE_DECLARATIVE_NAVIGATION_H

#include <QObject>
#include <QtQml/qqml.h>

class QAbstractItemModel;

class MarbleWidget;

namespace Marble {
class MarbleQuickItem;
class MarbleModel;
}

class NavigationPrivate;

class Navigation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MarbleWidget* map READ map WRITE setMap NOTIFY mapChanged)
    Q_PROPERTY(Marble::MarbleQuickItem * marbleQuickItem READ marbleQuickItem WRITE setMarbleQuickItem NOTIFY marbleQuickItemChanged)
    Q_PROPERTY(bool guidanceModeEnabled READ guidanceModeEnabled WRITE setGuidanceModeEnabled NOTIFY guidanceModeEnabledChanged)
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool soundEnabled READ soundEnabled WRITE setSoundEnabled NOTIFY soundEnabledChanged)
    Q_PROPERTY(QString speaker READ speaker WRITE setSpeaker NOTIFY speakerChanged)
    Q_PROPERTY(QString nextInstructionText READ nextInstructionText NOTIFY nextInstructionTextChanged)
    Q_PROPERTY(QString nextRoad READ nextRoad NOTIFY nextRoadChanged)
    Q_PROPERTY(QString nextInstructionImage READ nextInstructionImage NOTIFY nextInstructionImageChanged)
    Q_PROPERTY(qreal nextInstructionDistance READ nextInstructionDistance NOTIFY nextInstructionDistanceChanged)
    Q_PROPERTY(qreal destinationDistance READ destinationDistance NOTIFY destinationDistanceChanged)
    Q_PROPERTY(QString voiceNavigationAnnouncement READ voiceNavigationAnnouncement NOTIFY voiceNavigationAnnouncementChanged)
    Q_PROPERTY(bool deviated READ deviated NOTIFY deviationChanged)

public:
    explicit Navigation( QObject* parent = 0 );

    ~Navigation();

    MarbleWidget* map();

    void setMap( MarbleWidget* widget );

    bool guidanceModeEnabled() const;

    void setGuidanceModeEnabled( bool enabled );

    bool muted() const;

    void setMuted(bool enabled);

    QString nextInstructionText() const;

    QString nextRoad() const;

    QString nextInstructionImage() const;

    qreal nextInstructionDistance() const;

    qreal destinationDistance() const;

    QString voiceNavigationAnnouncement() const;

    QString speaker() const;

    void setSpeaker( const QString &speaker );

    bool soundEnabled() const;

    void setSoundEnabled( bool soundEnabled );

    bool deviated() const;

    Marble::MarbleQuickItem * marbleQuickItem() const;

    Q_INVOKABLE QPointF positionOnRoute() const;

    Q_INVOKABLE double screenAccuracy() const;

public Q_SLOTS:
    void setMarbleQuickItem(Marble::MarbleQuickItem * marbleQuickItem);

Q_SIGNALS:
    void mapChanged();

    void guidanceModeEnabledChanged();

    void mutedChanged(bool arg);

    void nextInstructionTextChanged();

    void nextInstructionImageChanged();

    void nextInstructionDistanceChanged();

    void destinationDistanceChanged();

    void nextRoadChanged();

    void voiceNavigationAnnouncementChanged();

    void soundEnabledChanged();

    void speakerChanged();

    void deviationChanged();

    void marbleQuickItemChanged(Marble::MarbleQuickItem * marbleQuickItem);

private Q_SLOTS:
    void update();

private:
    NavigationPrivate* const d;
};

#endif
