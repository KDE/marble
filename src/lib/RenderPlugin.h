//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn <tackat@kde.org>
// Copyright 2008      Inge Wallin  <inge@lysator.liu.se>
// Copyright 2011,2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2012      Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef MARBLE_RENDERPLUGIN_H
#define MARBLE_RENDERPLUGIN_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/Qt>
#include <QtGui/QStandardItem>

#include "RenderPluginInterface.h"
#include "marble_export.h"


class QAction;
class QActionGroup;
class QStandardItem;

namespace Marble
{

class MarbleModel;
class RenderPluginModel;

/**
 * @brief The abstract class that creates a renderable item
 *
 * Renderable Plugins can be used to extend Marble's functionality:
 * They allow to draw stuff on top of the map / globe
 *
 */

class MARBLE_EXPORT RenderPlugin : public QObject, public RenderPluginInterface
{
    Q_OBJECT

    Q_PROPERTY ( QString name READ name CONSTANT )
    Q_PROPERTY ( QString nameId READ nameId CONSTANT )
    Q_PROPERTY ( QString version READ version CONSTANT )
    Q_PROPERTY ( QString description READ description CONSTANT )
    Q_PROPERTY ( bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged )
    Q_PROPERTY ( bool visible READ visible WRITE setVisible NOTIFY visibilityChanged )
    Q_PROPERTY ( bool userCheckable READ isUserCheckable WRITE setUserCheckable NOTIFY userCheckableChanged )

 public:
    /**
     * @brief A Type of plugin
     */
    enum RenderType {
        Unknown,
        Online
    };

    explicit RenderPlugin( const MarbleModel *marbleModel );
    virtual ~RenderPlugin();

    /**
     * @brief String that should be displayed in GUI
     *
     * Using a "&" you can suggest key shortcuts
     *
     * Example: "&Stars"
     *
     * @return string for gui usage
     */
    virtual QString guiString() const = 0;

    /**
     * @brief Creation a new instance of the plugin
     *
     * This method is used to create a new object of the current
     * plugin using the @p marbleModel given.
     *
     * @param marbleModel base model
     * @return new instance of current plugin
     * @note Typically this method is implemented with the help of the MARBLE_PLUGIN() macro.
     */
    virtual RenderPlugin *newInstance( const MarbleModel *marbleModel ) const = 0;

    /**
     * @brief Access to the MarbleModel
     *
     * Internal way to access the model of marble.
     * Can be used to interact with the main application
     *
     * @return marble model
     * @see MarbleModel
     */
    const MarbleModel* marbleModel() const;

    /**
     * @brief Getting all actions
     *
     * This method is used by the main window to get all of the actions that this
     * plugin defines. There is no guarantee where the main window will place the
     * actions but it will generally be in a Menu. The returned QList should
     * also contain all of the actions returned by @see toolbarActions().
     *
     * @return a list of grouped actions
     */
    virtual QList<QActionGroup*>*   actionGroups() const;

    /**
     * @brief Getting all actions which should be placed in the toolbar
     *
     * This method returns a subset of the actions returned by @see actions() which
     * are intended to be placed in a more prominent place such as a toolbar above
     * the Marble Widget. You are not guaranteed that they will be in an actual
     * toolbar but they will be visible and discoverable
     *
     * @return a list of grouped toolbar actions
     */
    virtual QList<QActionGroup*>*   toolbarActionGroups() const;

    /**
     * @brief is enabled
     *
     * This method indicates enableability of the plugin
     *
     * If plugin is enabled it going to be displayed in Marble Menu
     * as active action which can be @see setUserCheckable
     *
     * @return enableability of the plugin
     * @see setEnabled
     */
    bool    enabled() const;

    /**
     * @brief is visible
     *
     * This method indicates visibility of the plugin
     *
     * If plugin is visible you can see it on the map/globe
     *
     * @return visibility of the plugin
     * @see setVisible
     */
    bool    visible() const;

    /**
     * @brief is user checkable
     *
     * This method indicates user checkability of plugin's
     * action displayed in application menu
     *
     * Can control plugin visibility
     *
     * @warning User can do it only if @see enabled is true
     *
     * @return checkability of the plugin
     * @see setUserCheckable
     */
    bool    isUserCheckable() const;

    /**
     * @brief Settings of the plugin
     *
     * Settings is the map (hash table) of plugin's settings
     * This method is called to determine the current settings of the plugin
     * for serialization, e.g. when closing the application.
     *
     * @return plugin's settings
     * @see setSettings
     */
    virtual QHash<QString,QVariant> settings() const;

    /**
     * @brief Set the settings of the plugin
     *
     * Usually this is called at startup to restore saved settings.
     *
     * @param new plugin's settings
     * @see settings
     */
    virtual void setSettings( const QHash<QString,QVariant> &settings );

    /**
     * @brief Render type of the plugin
     *
     * Function for returning the type of plugin this is for.
     * This affects where in the menu tree the action() is placed.
     *
     * @see RenderType
     * @return: The type of render plugin this is
     */
    virtual RenderType renderType() const;
    virtual QString runtimeTrace() const;

 public Q_SLOTS:
    /**
     * @brief settting enabled
     *
     * If @p enabled = true, plugin will be enabled
     *
     * If plugin is enabled it will be possible to show/hide it
     * from menu (access from UI)
     *
     * @param enabled plugin's enabled state
     * @see enabled
     */
    void    setEnabled( bool enabled );

    /**
     * @brief settting visible
     *
     * If @p visible = true, plugin will be visible
     *
     * @param visible visibility of the plugin
     * @see visible
     */
    void    setVisible( bool visible );

    /**
     * @brief settting user checkable
     *
     * If @p isUserCheckable = true, user will get an
     * option to control visibility in application menu
     *
     * @param isUserCheckable user checkability of the plugin
     * @see isUserCheckable
     */
    void    setUserCheckable(bool isUserCheckable);

    /**
     * @brief Passes an empty set of settings to the plugin
     *
     * Well behaving plugins restore their settings to default values as a result of calling this method.
     *
     */
    void    restoreDefaultSettings();

    /**
     * @brief Full list of the settings keys
     *
     * This method should be used to get all possible
     * settings' keys for the plugin's settings
     *
     * @return list with the keys of settings
     */
    QStringList settingKeys();

    /**
     * @brief Change setting key's values
     * @param key setting key
     * @param value new value
     *
     * This method applies @p value for the @p key
     *
     * @return successfully changed or not
     */
    bool setSetting( const QString & key, const QVariant & value );

    /**
     * @brief Getting setting value from the settings
     * @param key setting's key index
     *
     * This method should be used to get current value of @p key
     * in settings hash table
     *
     * @return setting value
     */
    QVariant setting( const QString & key );

    /**
     * @brief Plugin's menu action
     *
     * The action is checkable and controls the visibility of the plugin.
     *
     * @return action, displayed in menu
     */
    QAction *action() const;

 Q_SIGNALS:
    /**
     * This signal is emitted if the visibility is changed with @see setVisible
     */
    void visibilityChanged( bool visible, const QString &nameId );

    /**
     * This signal is emitted if the enabled property is changed with @see setEnabled
     */
    void enabledChanged( bool enable );

    /**
     * This signal is emitted if the user checkable property is changed with @see setUserCheckable
     */
    void userCheckableChanged(bool isUserCheckable);

    /**
     * This signal is emitted if the settings of the RenderPlugin changed.
     */
    void settingsChanged( QString nameId );

    /**
     * This signal is emitted if the actions that the plugin supports change in
     * any way
     */
    void actionGroupsChanged();

    /**
     * This signal is emitted if an update of the view is needed. If available with the
     * @p dirtyRegion which is the region the view will change in. If dirtyRegion.isEmpty() returns
     * true, the whole viewport has to be repainted.
     */
    void repaintNeeded( QRegion dirtyRegion = QRegion() );

 protected:
    bool eventFilter( QObject *, QEvent * );

 private:
    friend class RenderPluginModel;

    QStandardItem *item();

    void applyItemState();
    void retrieveItemState();

 private:
    Q_DISABLE_COPY( RenderPlugin )
    class Private;
    Private * const d;
};

#define MARBLE_PLUGIN(T) public:\
    virtual RenderPlugin* newInstance( const MarbleModel *marbleModel ) const { return new T( marbleModel ); }
}

#endif
