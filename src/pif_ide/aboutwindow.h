#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QMainWindow>

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
};

#endif // ABOUTWINDOW_H
