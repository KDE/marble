//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef PROGRESS_FLOAT_ITEM_H
#define PROGRESS_FLOAT_ITEM_H

#include "AbstractFloatItem.h"

#include <QMutex>
#include <QTimer>
#include <QIcon>

namespace Marble
{

/**
 * @brief A float item that shows a pie-chart progress
 * indicator when downloads are active
 */
class ProgressFloatItem  : public AbstractFloatItem
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.ProgressFloatItem")

    Q_INTERFACES( Marble::RenderPluginInterface )

    MARBLE_PLUGIN( ProgressFloatItem )

 public:
    explicit ProgressFloatItem( const MarbleModel *marbleModel = 0 );
    ~ProgressFloatItem () override;

    QStringList backendTypes() const override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon () const override;

    void initialize () override;

    bool isInitialized () const override;

    QPainterPath backgroundShape() const override;

    void paintContent( QPainter *painter ) override;

private Q_SLOTS:
    void removeProgressItem();

    void handleProgress( int active, int queued );

    void hideProgress();

    void show();

    void scheduleRepaint();

 private:
    Q_DISABLE_COPY( ProgressFloatItem )

    bool active() const;

    void setActive( bool active );

    bool m_isInitialized;

    int m_totalJobs;

    int m_completedJobs;

    qreal m_completed;

    QTimer m_progressHideTimer;

    QTimer m_progressShowTimer;

    QMutex m_jobMutex;

    bool m_active;

    QIcon m_icon;

    int m_fontSize;

    QTimer m_repaintTimer;
};

}

#endif

