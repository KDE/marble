//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Spencer Brown <spencerbrown991@gmail.com>
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
    explicit NotesModel(const MarbleModel *marbleModel, QObject *parent = 0);

protected:
    void getAdditionalItems(const GeoDataLatLonAltBox& box, qint32 number = 10) override;
    void parseFile(const QByteArray& file) override;
};
}

#endif
