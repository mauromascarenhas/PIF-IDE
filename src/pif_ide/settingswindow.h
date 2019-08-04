#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QSettings>
#include <QMainWindow>

#include "nmainwindow.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public NMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private:
    Ui::SettingsWindow *ui;

    void loadSettings();

protected:
    void showEvent(QShowEvent *event);
};

#endif // SETTINGSWINDOW_H
