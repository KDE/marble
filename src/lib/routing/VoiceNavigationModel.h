//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_VOICENAVIGATIONMODEL_H
#define MARBLE_VOICENAVIGATIONMODEL_H

#include "marble_export.h"
#include "Route.h"
#include "Maneuver.h"
#include "PositionTracking.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QFileInfo>

namespace Marble
{

class VoiceNavigationModelPrivate;

class MARBLE_EXPORT VoiceNavigationModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString speaker READ speaker WRITE setSpeaker NOTIFY speakerChanged)
    Q_PROPERTY(bool isSpeakerEnabled READ isSpeakerEnabled WRITE setSpeakerEnabled NOTIFY isSpeakerEnabledChanged )
    Q_PROPERTY(QString instruction READ instruction NOTIFY instructionChanged)
    Q_PROPERTY(QString preview READ preview NOTIFY previewChanged)

public:
    /** Constructor */
    explicit VoiceNavigationModel( QObject *parent = 0 );

    /** Destructor */
    ~VoiceNavigationModel();

// Initialization
    QString speaker() const;

    void setSpeaker( const QString &speaker );

    bool isSpeakerEnabled() const;

    void setSpeakerEnabled( bool enabled );

    void reset();

public Q_SLOTS:
// Continuous updates
    void handleTrackingStatusChange( PositionProviderStatus status );

    void update( const Route &route, qreal distanceManuever, qreal distanceTarget, bool deviated );

// Turn instructions
    QString preview() const;

    QString instruction() const;

Q_SIGNALS:
    void speakerChanged();

    void isSpeakerEnabledChanged();

    void instructionChanged();

    void previewChanged();

private:
    VoiceNavigationModelPrivate* const d;
    friend class VoiceNavigationModelPrivate;
};

}

#endif // MARBLE_VOICENAVIGATIONMODEL_H
