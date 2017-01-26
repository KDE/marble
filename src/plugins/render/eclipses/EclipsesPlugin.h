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
#include "DialogConfigurationInterface.h"

#include <QHash>

class QMenu;

namespace Ui {
    class EclipsesConfigDialog;
    class EclipsesReminderDialog;
}

namespace Marble
{

class MarbleWidget;
class EclipsesModel;
class EclipsesItem;
class EclipsesBrowserDialog;

/**
 * @brief This plugin displays solar eclipses.
 *
 * It utilizes Gerhard Holtcamps eclsolar class to render nice
 * visualizations of eclipse events on earth.
 */
class EclipsesPlugin : public RenderPlugin,
                       public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.EclipsesPlugin")

    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )
    MARBLE_PLUGIN( EclipsesPlugin )

public:
    EclipsesPlugin();
    explicit EclipsesPlugin( const MarbleModel *marbleModel );
    ~EclipsesPlugin() override;

    // this is the implementation of the RenderPlugin interface
    // see RenderPlugin.h for a description

    QStringList backendTypes() const override;
    QString renderPolicy() const override;
    QStringList renderPosition() const override;
    QString name() const override;
    QString nameId() const override;
    QString guiString() const override;
    QString version() const override;
    QString description() const override;
    QString copyrightYears() const override;
    QVector<PluginAuthor> pluginAuthors() const override;
    QIcon icon() const override;
    RenderPlugin::RenderType renderType() const override;
    QList<QActionGroup*>* actionGroups() const override;
    QDialog *configDialog() override;

    void initialize() override;
    bool isInitialized() const override;

    bool render( GeoPainter *painter,
                 ViewportParams *viewport,
                 const QString &renderPos,
                 GeoSceneLayer *layer ) override;

    QHash<QString, QVariant> settings() const override;
    void setSettings( const QHash<QString, QVariant> &settings ) override;

protected:
    bool eventFilter( QObject *object, QEvent *e ) override;

private Q_SLOTS:
    void readSettings();
    void writeSettings();
    void updateSettings();

    /**
     * @brief Update list of eclipses for the current year
     *
     * This calculates the list of eclipses for the year the marble clock
     * is set to.
     */
    void updateEclipses();

    /**
     * @brief Show an eclipse event on the marble map
     *
     * @param year The year the eclipse event happens
     * @param index The index of the eclipse in this year
     *
     * Shows the eclipse with index @p index in year @p year by setting
     * the marble clock to the time of the eclipse's maximum.
     */
    void showEclipse( int year, int index );

    /**
     * @brief Show an eclipse event selected from the menu
     *
     * @param action The menu items action
     *
     * Shows the eclipse the menu item given by @p action refers to.
     * The eclipse's index is stored in the actions data field while the
     * year is taken from the action's text.
     */
    void showEclipseFromMenu( QAction *action );

    /**
     * @brief Update menu item state
     *
     * Updates the state of the plugin's menu items. They will be disabled for
     * non earth themes since we only support eclipse events on earth.
     */
    void updateMenuItemState();

private:
    bool renderItem( GeoPainter *painter, EclipsesItem *item ) const;

private:
    bool m_isInitialized;

    MarbleWidget *m_marbleWidget;

    EclipsesModel *m_model;
    QList<QActionGroup*> m_actionGroups;
    QActionGroup *m_eclipsesActionGroup;
    QHash<QString, QVariant> m_settings;
    QAction *m_eclipsesMenuAction;
    QMenu *m_eclipsesListMenu;
    int m_menuYear;

    // dialogs
    QDialog *m_configDialog;
    Ui::EclipsesConfigDialog *m_configWidget;
    EclipsesBrowserDialog *m_browserDialog;
    QDialog *m_reminderDialog;
    Ui::EclipsesReminderDialog *m_reminderWidget;
};

}

#endif // MARBLE_ECLIPSESPLUGIN_H
