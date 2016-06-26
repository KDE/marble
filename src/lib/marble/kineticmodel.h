/*
  This file is part of the Ofi Labs X2 project.

  Copyright (C) 2010 Ariya Hidayat <ariya.hidayat@gmail.com>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef OFILABS_KINETICMODEL
#define OFILABS_KINETICMODEL

#include <QObject>
#include <QScopedPointer>
#include <QPointF>

class KineticModelPrivate;

class KineticModel: public QObject
{
    Q_OBJECT
    Q_PROPERTY(int duration READ duration WRITE setDuration)
    Q_PROPERTY(QPointF position READ position NOTIFY positionChanged)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval)

public:
    explicit KineticModel(QObject *parent = 0);
    ~KineticModel();

    int duration() const;
    QPointF position() const;
    int updateInterval() const;
    bool hasVelocity() const;

public Q_SLOTS:
    void setDuration(int ms);
    void setPosition(const QPointF& position);
    void setPosition(qreal posX, qreal posY);
    void jumpToPosition(const QPointF& position);
    void jumpToPosition(qreal posX, qreal posY);
    void setUpdateInterval(int ms);
    void stop();
    void start();

Q_SIGNALS:
    void positionChanged( qreal lon, qreal lat );
    void finished();

private Q_SLOTS:
    void update();

private:
    QScopedPointer<KineticModelPrivate> d_ptr;
    Q_DECLARE_PRIVATE(KineticModel);
    Q_DISABLE_COPY(KineticModel);
};

#endif
