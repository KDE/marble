// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef JOBPARAMETERS_H
#define JOBPARAMETERS_H

#include <QDir>

class JobParameters
{
public:
    JobParameters();

    QDir base();

    void setBase(const QDir &dir);

    bool cacheData() const;

    void setCacheData(bool cache);

private:
    QDir m_base;

    bool m_cacheData;
};

#endif // JOBPARAMETERS_H
