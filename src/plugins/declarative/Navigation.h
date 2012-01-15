//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_NAVIGATION_H
#define MARBLE_DECLARATIVE_NAVIGATION_H

#include <QtCore/QObject>
#include <QtDeclarative/QtDeclarative>

class QAbstractItemModel;

class MarbleWidget;
class NavigationPrivate;

class Navigation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool guidanceModeEnabled READ guidanceModeEnabled WRITE setGuidanceModeEnabled NOTIFY guidanceModeEnabledChanged)
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool soundEnabled READ soundEnabled WRITE setSoundEnabled NOTIFY soundEnabledChanged)
    Q_PROPERTY(QString speaker READ speaker WRITE setSpeaker NOTIFY speakerChanged)
    Q_PROPERTY(QString nextInstructionText READ nextInstructionText NOTIFY nextInstructionTextChanged)
    Q_PROPERTY(QString nextRoad READ nextRoad NOTIFY nextRoadChanged)
    Q_PROPERTY(QString nextInstructionImage READ nextInstructionImage NOTIFY nextInstructionImageChanged)
    Q_PROPERTY(qreal nextInstructionDistance READ nextInstructionDistance NOTIFY nextInstructionDistanceChanged)
    Q_PROPERTY(QString voiceNavigationAnnouncement READ voiceNavigationAnnouncement NOTIFY voiceNavigationAnnouncementChanged)

public:
    explicit Navigation( QObject* parent = 0 );

    ~Navigation();

    void setMarbleWidget( MarbleWidget* widget );

    bool guidanceModeEnabled() const;

    void setGuidanceModeEnabled( bool enabled );

    bool muted() const;

    void setMuted(bool enabled);

    QString nextInstructionText() const;

    QString nextRoad() const;

    QString nextInstructionImage() const;

    qreal nextInstructionDistance() const;

    QString voiceNavigationAnnouncement() const;

    QString speaker() const;

    void setSpeaker( const QString &speaker );

    bool soundEnabled() const;

    void setSoundEnabled( bool soundEnabled );

Q_SIGNALS:
    void guidanceModeEnabledChanged();

    void mutedChanged(bool arg);

    void nextInstructionTextChanged();

    void nextInstructionImageChanged();

    void nextInstructionDistanceChanged();

    void nextRoadChanged();

    void voiceNavigationAnnouncementChanged();

    void soundEnabledChanged();

    void speakerChanged();

private Q_SLOTS:
    void update();

private:
    NavigationPrivate* const d;
};

#endif
