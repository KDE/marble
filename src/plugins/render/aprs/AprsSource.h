// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#ifndef APRSSOURCE_H
#define APRSSOURCE_H

class QIODevice;
class QString;

namespace Marble
{

class AprsGatherer;

class AprsSource
{
public:
    explicit AprsSource(QIODevice *insocket = nullptr);
    virtual ~AprsSource();

    virtual QIODevice *openSocket() = 0;
    virtual QString sourceName() const = 0;

    QIODevice *socket();
    void setSocket(QIODevice *);

    // @brief Checks the results of an executed read() code.
    //
    // Checks the results of read() return code specified in @p
    // length from the socket created by this source to see if the
    // socket needs to be reopened.  If it needs to be reopened a
    // new socket will be placed into the @p socket pointer.
    // @p gatherer should be a pointer to the gatherer that called
    // the function and implements a thread-safe sleep().
    virtual void checkReadReturn(int length, QIODevice **socket, AprsGatherer *gatherer) = 0;

    // Specifies whether or not this type of source can directly
    // hear the transmitting station.  Only sources for devices
    // that are actually capable of receiving radio signals will
    // return true.
    virtual bool canDoDirect() const = 0;

private:
    QIODevice *m_socket = nullptr;
};

}

#endif /* APRSSOURCE_H */
