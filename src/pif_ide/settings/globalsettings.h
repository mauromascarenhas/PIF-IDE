#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QString>

class GlobalSettings
{
public:
    static int selectedTheme;

    static QString aboutSSheet;
    static QString editorSSheet;
    static QString settingsSSheet;
    static QString titlebarSSheet;

private:
    GlobalSettings();
};

#endif // GLOBALSETTINGS_H
