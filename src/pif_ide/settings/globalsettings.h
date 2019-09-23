#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QObject>
#include <QString>
#include <QStringList>

class GlobalSettings
{
public:
    static int selectedTheme;

    static QString aboutSSheet;
    static QString editorSSheet;
    static QString settingsSSheet;
    static QString titlebarSSheet;

    static QStringList cmdlPifc;
    static QStringList cmdlCComp;
    static QStringList cmdlCppComp;
    static QStringList cmdlJavaComp;
    static QStringList cmdlJavaExec;

private:
    GlobalSettings();
};

#endif // GLOBALSETTINGS_H
