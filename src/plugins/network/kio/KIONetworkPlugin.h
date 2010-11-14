//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Pino Toscano <pino@kde.org>
//


#ifndef KIONETWORKPLUGIN_H
#define KIONETWORKPLUGIN_H

#include "NetworkPlugin.h"
#include "HttpJob.h"

class KJob;

class KIOHttpJob : public Marble::HttpJob
{
    Q_OBJECT

 public:
    KIOHttpJob( const QUrl &source, const QString &destination, const QString &id );
    virtual ~KIOHttpJob();

 public Q_SLOTS:
    virtual void execute();

 private Q_SLOTS:
    void slotCanceled( KJob *job );
    void slotFinished( KJob *job );
    void slotInfoMessage( KJob *job, const QString &plainText, const QString &richText = QString() );
    void slotWarning( KJob *job, const QString &plainText, const QString &richText = QString() );

 private:
    KJob *m_job;
};


class KIONetworkPlugin: public Marble::NetworkPlugin
{
    Q_OBJECT

 public:
    KIONetworkPlugin();
    virtual ~KIONetworkPlugin();

    virtual QString name() const;
    virtual QString nameId() const;
    virtual QString guiString() const;
    virtual QString description() const;
    virtual QIcon icon() const;
    virtual void initialize();
    virtual bool isInitialized() const;
    virtual NetworkPlugin * newInstance() const;
    virtual Marble::HttpJob *createJob( const QUrl &source, const QString &destination, const QString &id );
};

#endif // KIONETWORKPLUGIN_H
