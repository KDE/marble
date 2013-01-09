//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_ECLIPSESPLUGIN_H
#define MARBLE_ECLIPSESPLUGIN_H

#include "RenderPlugin.h"

#include <QtCore/QObject>
#include <QMenu>
#include <QActionGroup>

namespace Ui {
    class EclipsesConfigDialog;
    class EclipsesListDialog;
    class EclipsesReminderDialog;
}

namespace Marble
{

class MarbleWidget;
class MarbleClock;
class EclipsesModel;

/**
 * @brief This plugin displays solar eclipses.
 *
 */
class EclipsesPlugin : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( EclipsesPlugin )

public:
    EclipsesPlugin();
    explicit EclipsesPlugin( const MarbleModel *marbleModel );
    virtual ~EclipsesPlugin();
    
    QStringList backendTypes() const;
    QString renderPolicy() const;
    QStringList renderPosition() const;
    QString name() const;
    QString nameId() const;
    QString guiString() const;
    QString version() const;
    QString description() const;
    QString copyrightYears() const;
    QList<PluginAuthor> pluginAuthors() const;
    QIcon icon() const;
    RenderPlugin::RenderType renderType() const;
    QList<QActionGroup*>* actionGroups() const;
    QDialog *configDialog();

    void initialize();
    bool isInitialized() const;

    bool render( GeoPainter *painter,
                 ViewportParams *viewport,
                 const QString &renderPos,
                 GeoSceneLayer *layer );

    QHash<QString, QVariant> settings() const;
    void setSettings( const QHash<QString, QVariant> &settings );

protected:
    bool eventFilter( QObject *object, QEvent *e );

private Q_SLOTS:
    void readSettings();
    void writeSettings();
    void updateSettings();
    void updateEclipses();
    void updateMenuItems();

    void updateListDialogForYear( int year );
    void updateListDialogButtons();
    void showSelectedEclipse();
    void showEclipse( int index );

private:
    bool m_isInitialized;

    MarbleWidget *m_marbleWidget;
    MarbleClock *m_clock;

    EclipsesModel *m_model;
    QList<QActionGroup*> m_actionGroups;
    QActionGroup *m_eclipsesActionGroup;
    QHash<QString, QVariant> m_settings;
    QAction *m_eclipsesMenuAction;
    QMenu *m_eclipsesListMenu;

    QDialog *m_configDialog;
    Ui::EclipsesConfigDialog *m_configWidget;
    QDialog *m_listDialog;
    Ui::EclipsesListDialog *m_listWidget;
    QDialog *m_reminderDialog;
    Ui::EclipsesReminderDialog *m_reminderWidget;
};

}

#endif // MARBLE_ECLIPSESPLUGIN_H
