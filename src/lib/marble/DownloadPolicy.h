// SPDX-FileCopyrightText: 2009 Jens-Michael Hoffmann <jmho@c-xx.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef MARBLE_DOWNLOADPOLICY_H
#define MARBLE_DOWNLOADPOLICY_H

#include <QString>
#include <QStringList>

#include "MarbleGlobal.h"

namespace Marble
{

class DownloadPolicyKey
{
    friend bool operator==(DownloadPolicyKey const &lhs, DownloadPolicyKey const &rhs);

public:
    DownloadPolicyKey();
    DownloadPolicyKey(const QStringList &hostNames, const DownloadUsage usage);
    DownloadPolicyKey(const QString &hostName, const DownloadUsage usage);

    QStringList hostNames() const;
    void setHostNames(const QStringList &hostNames);

    DownloadUsage usage() const;
    void setUsage(DownloadUsage const usage);

    bool matches(const QString &hostName, const DownloadUsage usage) const;

private:
    QStringList m_hostNames;
    DownloadUsage m_usage;
};

inline bool operator==(const DownloadPolicyKey &lhs, const DownloadPolicyKey &rhs)
{
    return lhs.m_hostNames == rhs.m_hostNames && lhs.m_usage == rhs.m_usage;
}

class DownloadPolicy
{
    friend bool operator==(const DownloadPolicy &lhs, const DownloadPolicy &rhs);

public:
    DownloadPolicy();
    explicit DownloadPolicy(const DownloadPolicyKey &key);

    int maximumConnections() const;
    void setMaximumConnections(const int);

    DownloadPolicyKey key() const;

private:
    DownloadPolicyKey m_key;
    int m_maximumConnections;
};

inline bool operator==(const DownloadPolicy &lhs, const DownloadPolicy &rhs)
{
    return lhs.m_key == rhs.m_key && lhs.m_maximumConnections == rhs.m_maximumConnections;
}

}

#endif
