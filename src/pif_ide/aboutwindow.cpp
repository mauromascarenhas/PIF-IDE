#include "aboutwindow.h"
#include "ui_aboutwindow.h"

AboutWindow::AboutWindow(QWidget *parent) :
    NMainWindow(parent),
    ui(new Ui::AboutWindow)
{
    ui->setupUi(this);

    // Sets the custom Widgets on the parent Class
    // Otherwise, the window resizing feature will not work
    NMainWindow::setCustomWidgets(ui->centralWidget, ui->statusBar);
    NMainWindow::setMaximizeButtonEnabled(false);

    //TODO: Load compiler data
}

AboutWindow::~AboutWindow()
{
    delete ui;
}
