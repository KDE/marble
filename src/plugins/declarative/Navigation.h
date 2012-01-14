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

namespace Marble
{

namespace Declarative
{

class MarbleWidget;
class NavigationPrivate;

class Navigation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool guidanceModeEnabled READ guidanceModeEnabled WRITE setGuidanceModeEnabled NOTIFY guidanceModeEnabledChanged)
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(QString nextInstructionText READ nextInstructionText NOTIFY nextInstructionTextChanged)
    Q_PROPERTY(QString nextRoad READ nextRoad NOTIFY nextRoadChanged)
    Q_PROPERTY(QString nextInstructionImage READ nextInstructionImage NOTIFY nextInstructionImageChanged)
    Q_PROPERTY(qreal nextInstructionDistance READ nextInstructionDistance NOTIFY nextInstructionDistanceChanged)

public:
    explicit Navigation( QObject* parent = 0 );

    ~Navigation();

    void setMarbleWidget( Marble::Declarative::MarbleWidget* widget );

    bool guidanceModeEnabled() const;

    void setGuidanceModeEnabled( bool enabled );

    bool muted() const;

    void setMuted(bool enabled);

    QString nextInstructionText() const;

    QString nextRoad() const;

    QString nextInstructionImage() const;

    qreal nextInstructionDistance() const;

Q_SIGNALS:
    void guidanceModeEnabledChanged();

    void mutedChanged(bool arg);

    void nextInstructionTextChanged();

    void nextInstructionImageChanged();

    void nextInstructionDistanceChanged();

    void nextRoadChanged();

private Q_SLOTS:
    void update();

private:
    NavigationPrivate* const d;
};

}

}

#endif
