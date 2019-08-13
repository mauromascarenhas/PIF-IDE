#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QDir>
#include <QUrl>
#include <QScreen>
#include <QSettings>
#include <QMainWindow>
#include <QMessageBox>
#include <QDesktopServices>

#include "nmainwindow.h"

namespace Ui {
class AboutWindow;
}

class AboutWindow : public NMainWindow
{
    Q_OBJECT

public:
    explicit AboutWindow(QWidget *parent = nullptr);
    ~AboutWindow();

private:
    Ui::AboutWindow *ui;

    void loadCompilerData();

private slots:
    void openOnlineHelp();
    void openOnlineSamples();
    void openSourceSamples();
    void openNSOSL();
};

#endif // ABOUTWINDOW_H
