// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef WIKIPEDIAMODEL_H
#define WIKIPEDIAMODEL_H

#include "AbstractDataPluginModel.h"

#include <QIcon>

namespace Marble
{

class MarbleWidget;

const quint32 numberOfArticlesPerFetch = 7;

class WikipediaModel : public AbstractDataPluginModel
{
    Q_OBJECT

public:
    explicit WikipediaModel(const MarbleModel *marbleModel, QObject *parent = nullptr);
    ~WikipediaModel() override;

    void setShowThumbnail(bool show);

    void setMarbleWidget(MarbleWidget *widget);

protected:
    /**
     * Generates the download url for the description file from the web service depending on
     * the @p box surrounding the view and the @p number of files to show.
     **/
    void getAdditionalItems(const GeoDataLatLonAltBox &box, qint32 number = 10) override;

    /**
     * The reimplementation has to parse the @p file and should generate widgets. This widgets
     * have to be scheduled to downloadItemData or could be directly added to the list,
     * depending on if they have to download information to be shown.
     **/
    void parseFile(const QByteArray &file) override;

private:
    MarbleWidget *m_marbleWidget = nullptr;
    QIcon m_wikipediaIcon;
    QString m_languageCode;

    bool m_showThumbnail;
};

}

#endif // WIKIPEDIAMODEL_H
