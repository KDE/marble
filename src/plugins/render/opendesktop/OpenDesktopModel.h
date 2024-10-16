// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef OPENDESKTOPMODEL_H
#define OPENDESKTOPMODEL_H

#include "AbstractDataPluginModel.h"

namespace Marble
{

class MarbleWidget;
class MarbleModel;

class OpenDesktopModel : public AbstractDataPluginModel
{
    Q_OBJECT

public:
    explicit OpenDesktopModel(const MarbleModel *marbleModel, QObject *parent = nullptr);
    ~OpenDesktopModel() override;

    void setMarbleWidget(MarbleWidget *widget);

protected:
    /**
     * Generates the download url for the description file from the web service depending on
     * the @p box surrounding the view and the @p number of files to show.
     **/
    void getAdditionalItems(const Marble::GeoDataLatLonAltBox &box, qint32 number = 10) override;
    /**
     * Parses the @p file which getAdditionalItems downloads and
     * prepares the data for usage.
     **/
    void parseFile(const QByteArray &file) override;

private:
    MarbleWidget *m_marbleWidget = nullptr;
};

}

#endif // OPENDESKTOPMODEL_H
