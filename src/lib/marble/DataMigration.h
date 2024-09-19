// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_DATAMIGRATION_H
#define MARBLE_DATAMIGRATION_H

#include "marble_export.h"
#include <QObject>

namespace Marble
{

class MARBLE_EXPORT DataMigration : public QObject
{
    Q_OBJECT

public:
    explicit DataMigration(QObject *parent);
    ~DataMigration() override;

public Q_SLOTS:
    void exec();

private:
    static void moveFiles(const QString &source, const QString &target);
};

}

#endif // MARBLE_DATAMIGRATION_H
