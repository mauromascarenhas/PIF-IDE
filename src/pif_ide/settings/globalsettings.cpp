#include "globalsettings.h"

int GlobalSettings::selectedTheme = 0;

QString GlobalSettings::aboutSSheet = NULL;
QString GlobalSettings::editorSSheet = NULL;
QString GlobalSettings::settingsSSheet = NULL;
QString GlobalSettings::titlebarSSheet = NULL;

QStringList GlobalSettings::cmdlPifc = QStringList() << "-f" << "-l" << QObject::tr("$lang") << QObject::tr("$source")
                                                     << "-o" << QObject::tr("$object");
QStringList GlobalSettings::cmdlCComp = QStringList() << QObject::tr("$source") << "-o" << QObject::tr("$executable");
QStringList GlobalSettings::cmdlCppComp = QStringList() << QObject::tr("$source") << "-o" << QObject::tr("$executable");
QStringList GlobalSettings::cmdlJavaComp = QStringList() << QObject::tr("$source");
QStringList GlobalSettings::cmdlJavaExec = QStringList() << "-cp" << QObject::tr("$wdir") << QObject::tr("$executable");

GlobalSettings::GlobalSettings()
{

}
