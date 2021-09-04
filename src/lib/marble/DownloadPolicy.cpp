// SPDX-FileCopyrightText: 2009 Jens-Michael Hoffmann <jmho@c-xx.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "DownloadPolicy.h"

namespace Marble
{

DownloadPolicyKey::DownloadPolicyKey()
    : m_hostNames(),
      m_usage( DownloadBrowse )
{
}

DownloadPolicyKey::DownloadPolicyKey( const QStringList & hostNames,
                                      const DownloadUsage usage )
    : m_hostNames( hostNames ),
      m_usage( usage )
{
}

DownloadPolicyKey::DownloadPolicyKey( const QString & hostName,
                                      const DownloadUsage usage )
    : m_hostNames( hostName ),
      m_usage( usage )
{
}

QStringList DownloadPolicyKey::hostNames() const
{
    return m_hostNames;
}

void DownloadPolicyKey::setHostNames( const QStringList & hostNames )
{
    m_hostNames = hostNames;
}

DownloadUsage DownloadPolicyKey::usage() const
{
    return m_usage;
}

void DownloadPolicyKey::setUsage( DownloadUsage const usage )
{
    m_usage = usage;
}

bool DownloadPolicyKey::matches( QString const & hostName, const DownloadUsage usage ) const
{
    return m_hostNames.contains( hostName ) && m_usage == usage;
}


DownloadPolicy::DownloadPolicy()
    : m_key(),
      m_maximumConnections( 1 )
{
}

DownloadPolicy::DownloadPolicy( const DownloadPolicyKey & key )
    : m_key( key ),
      m_maximumConnections( 1 )
{
}

int DownloadPolicy::maximumConnections() const
{
    return m_maximumConnections;
}

void DownloadPolicy::setMaximumConnections( const int n )
{
    m_maximumConnections = n;
}

DownloadPolicyKey DownloadPolicy::key() const
{
    return m_key;
}

}
