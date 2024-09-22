/*
    This file is part of the Ofi Labs X2 project.

    SPDX-FileCopyrightText: 2010 Ariya Hidayat <ariya.hidayat@gmail.com>
    SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef OFILABS_KINETICMODEL
#define OFILABS_KINETICMODEL

#include <QObject>
#include <QPointF>
#include <QScopedPointer>

class KineticModelPrivate;

class KineticModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int duration READ duration WRITE setDuration)
    Q_PROPERTY(QPointF position READ position NOTIFY positionChanged)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval)

public:
    explicit KineticModel(QObject *parent = nullptr);
    ~KineticModel() override;

    int duration() const;
    QPointF position() const;
    int updateInterval() const;
    bool hasVelocity() const;

public Q_SLOTS:
    void setDuration(int ms);
    void setPosition(const QPointF &position);
    void setPosition(qreal posX, qreal posY);
    void setHeading(qreal heading);
    void jumpToPosition(const QPointF &position);
    void jumpToPosition(qreal posX, qreal posY);
    void setUpdateInterval(int ms);
    void stop();
    void start();

Q_SIGNALS:
    void positionChanged(qreal lon, qreal lat);
    void headingChanged(qreal heading);
    void finished();

private Q_SLOTS:
    void update();

private:
    QScopedPointer<KineticModelPrivate> d_ptr;
    Q_DECLARE_PRIVATE(KineticModel)
    Q_DISABLE_COPY(KineticModel);
};

#endif
