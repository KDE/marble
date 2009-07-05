#ifndef GEOCUTE_VELOCITYPROVIDER_H
#define GEOCUTE_VELOCITYPROVIDER_H

#include "Provider.h"
#include "Velocity.h"



class QString;

namespace GeoCute {

class VelocityProvider : public Provider {
    Q_OBJECT
    
    public:
        VelocityProvider(const QString& service, const QString& path,
            QObject* parent = 0);
        ~VelocityProvider();
        Velocity velocity();
        
    signals:
        void velocityChanged(GeoCute::Velocity info);
        
    private:
        class Private;
        Private* const d;
        Q_PRIVATE_SLOT(d, void velocityChangedCall(QDBusMessage message))
};

}



#endif
