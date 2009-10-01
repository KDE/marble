// Copyright 2009  Jens-Michael Hoffmann <jmho@c-xx.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.

#ifndef MARBLE_DOWNLOAD_POLICY
#define MARBLE_DOWNLOAD_POLICY

#include <QtCore/QString>
#include <QtCore/QStringList>

namespace Marble
{

enum DownloadUsage { DownloadBulk, DownloadBrowse };

class DownloadPolicyKey
{
    friend bool operator<( DownloadPolicyKey const & lhs, DownloadPolicyKey const & rhs );

 public:
    DownloadPolicyKey();
    DownloadPolicyKey( const QStringList & hostNames, const DownloadUsage usage );
    DownloadPolicyKey( const QString & hostName, const DownloadUsage usage );

    QStringList hostNames() const;
    void setHostNames( const QStringList & hostNames );

    DownloadUsage usage() const;
    void setUsage( DownloadUsage const usage );

    /// FIXME: take usage into account
    bool matches( QString const & hostName ) const;

 private:
    QStringList m_hostNames;
    DownloadUsage m_usage;
};

bool operator<( DownloadPolicyKey const & lhs, DownloadPolicyKey const & rhs );


class DownloadPolicy
{
 public:
    DownloadPolicy();
    explicit DownloadPolicy( const DownloadPolicyKey & key );

    int maximumConnections() const;
    void setMaximumConnections( const int );

    DownloadPolicyKey key() const;

 private:
    DownloadPolicyKey m_key;
    int m_maximumConnections;
};

}

#endif
