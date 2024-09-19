// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Spencer Brown <spencerbrown991@gmail.com>
//

#ifndef NOTESMODEL_H
#define NOTESMODEL_H

#include "AbstractDataPluginModel.h"

namespace Marble
{

class NotesModel : public AbstractDataPluginModel
{
    Q_OBJECT

public:
    explicit NotesModel(const MarbleModel *marbleModel, QObject *parent = nullptr);

protected:
    void getAdditionalItems(const GeoDataLatLonAltBox &box, qint32 number = 10) override;
    void parseFile(const QByteArray &file) override;
};
}

#endif
