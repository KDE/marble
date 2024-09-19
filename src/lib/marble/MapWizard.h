// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef MARBLE_MAPWIZARD_H
#define MARBLE_MAPWIZARD_H

#include <QWizard>

#include "marble_export.h"

/**
 * @file
 * This file contains the header for MapWizard
 * @author Utku Aydın <utkuaydin34@gmail.com>
 */

class QNetworkReply;

namespace Marble
{

class GeoSceneDocument;

class MapWizardPrivate;

class MARBLE_EXPORT MapWizard : public QWizard
{
    Q_OBJECT

public:
    explicit MapWizard(QWidget *parent = nullptr);
    ~MapWizard() override;

    QStringList wmsServers() const;
    void setWmsServers(const QStringList &uris);

    QStringList wmtsServers() const;
    void setWmtsServers(const QStringList &uris);

    QStringList staticUrlServers() const;
    void setStaticUrlServers(const QStringList &uris);

    static QString createArchive(QWidget *parent, const QString &mapId);
    static void deleteArchive(const QString &mapId);

    // QWizard's functions
    void accept() override;
    bool validateCurrentPage() override;
    int nextId() const override;
    void cleanupPage(int id) override;

public Q_SLOTS:
    // WMS protocol
    void processCapabilitiesResults();
    void processSelectedLayerInformation();
    void processImageResults();

    void createWmsLegend();

    // Open file dialogs
    void querySourceImage();
    void queryPreviewImage();
    void queryLegendImage();

    // Other
    void setLineEditWms(const QString &text);
    void setLayerButtonsVisible(bool visible);
    void setSearchFieldVisible(bool visible);
    void showPreview();

    void updateSearchFilter(const QString &text);
    void updateListViewSelection();
    void updateBackdropCheckBox();
    void updateOwsServiceType();
    void chooseBackgroundColor();

private:
    Q_PRIVATE_SLOT(d, void pageEntered(int))

    GeoSceneDocument *createDocument();
    bool createFiles(const GeoSceneDocument *head);
    static QString createLegendHtml(const QString &image = QLatin1String("./legend/legend.png"));
    void createLegendFile(const QString &legendHtml);
    void createLegend();
    void downloadLegend(const QString &url);

    Q_DISABLE_COPY(MapWizard)
    MapWizardPrivate *const d;
};

}

#endif
