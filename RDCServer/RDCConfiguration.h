#ifndef RDCCONFIGURATION_H
#define RDCCONFIGURATION_H

#include <QVariant>

class QSettings;
class RDCConfiguration
{
public:
    static RDCConfiguration* standardConfiguration(void);

private:
    RDCConfiguration();
    void initDefaultValues(void);

public:
    const QVariant valueForKey(const char*) const;
    void setValueForKey(const QVariant &, const char*);

private:
    static RDCConfiguration* config;
    QSettings* m_pSettings;
};

#endif // RDCCONFIGURATION_H
