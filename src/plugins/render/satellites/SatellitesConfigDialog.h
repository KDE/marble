//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_SATELLITESCONFIGDIALOG_H
#define MARBLE_SATELLITESCONFIGDIALOG_H

#include <QDialog>
#include <QTreeView>
#include <QListWidget>
#include <QListWidgetItem>

namespace Ui {
    class SatellitesConfigDialog;
}

namespace Marble {

class SatellitesConfigAbstractItem;
class SatellitesConfigNodeItem;

class SatellitesConfigDialog : public QDialog
{
    Q_OBJECT

public:
    enum {
        IsLoadedRole    = Qt::UserRole + 1
    };

    SatellitesConfigDialog( QWidget *parent = 0 );
    ~SatellitesConfigDialog();

    void update();

    void setUserDataSources( const QStringList &sources );
    QStringList userDataSources() const;
    void setUserDataSourceLoaded( const QString& source, bool loaded );

    SatellitesConfigAbstractItem* addSatelliteItem( const QString &body,
                                                    const QString &category,
                                                    const QString &title,
                                                    const QString &id,
                                                    const QString &url = QString() );
    SatellitesConfigAbstractItem* addTLESatelliteItem( const QString &category,
                                                       const QString &title,
                                                       const QString &url );

    Ui::SatellitesConfigDialog* configWidget();

Q_SIGNALS:
    void dataSourcesReloadRequested();
    void userDataSourceAdded( const QString &source );
    void userDataSourceRemoved( const QString &source );
    void userDataSourcesChanged();
    void activatePluginClicked();

public Q_SLOTS:
    void setDialogActive( bool active );

protected:
    void initialize();
    void setupSatellitesTab();
    void setupDataSourcesTab();
    SatellitesConfigNodeItem* getSatellitesCategoryItem(
        const QString &body,
        const QString &category,
        bool create = false );
    SatellitesConfigNodeItem* getSatellitesBodyItem(
        const QString &body,
        bool create = false );
    void expandTreeView();

protected Q_SLOTS:
    void reloadDataSources();
    void addDataSource();
    void openDataSource();
    void removeSelectedDataSource();
    void updateButtonState();

private:
    QString translation( const QString &from ) const;

    QStringList m_userDataSources;
    Ui::SatellitesConfigDialog *m_configWidget;
    QMap<QString, QString> m_translations;
};

} // namespace Marble

#endif // MARBLE_SATELLITESCONFIGDIALOG_H

