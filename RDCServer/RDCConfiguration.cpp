#include "RDCConfiguration.h"

#include <QCoreApplication>
#include <QSettings>
#include <QString>
#include <QFile>

RDCConfiguration* RDCConfiguration::config = nullptr;

RDCConfiguration::RDCConfiguration() : m_pSettings(new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                                                                 QCoreApplication::organizationName(),
                                                                 QCoreApplication::applicationName()))
{
    this->initDefaultValues();
}

RDCConfiguration* RDCConfiguration::standardConfiguration(void)
{
    if(RDCConfiguration::config == nullptr)
        RDCConfiguration::config = new RDCConfiguration();
    return RDCConfiguration::config;
}

void RDCConfiguration::initDefaultValues(void)
{
    QFile file(this->m_pSettings->fileName());

    if(!file.exists())
    {
        //初始化配置
        this->m_pSettings->setValue(QString("ListenPort"), QString("9999"));
    }
    return ;
}

const QVariant RDCConfiguration::valueForKey(const char* key) const
{
    return this->m_pSettings->value(QString(key));
}

void RDCConfiguration::setValueForKey(const QVariant& value, const char* key)
{
    return this->m_pSettings->setValue(QString(key), value);
}

