#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QDir>
#include <QList>
#include <QFile>
#include <QProcess>
#include <QShortcut>
#include <QFileDialog>
#include <QMessageBox>
#include <QMainWindow>
#include <QKeySequence>

#include "nmainwindow.h"
#include "highlighter/highlighter.h"

namespace Ui {
class EditorWindow;
}

class EditorWindow : public NMainWindow
{
    Q_OBJECT

public:
    explicit EditorWindow(QWidget *parent = nullptr);
    ~EditorWindow();

private:
    Ui::EditorWindow *ui;

private:
    void compileProject();
    void runProject();

    void setupEditor();
    void createShortcuts();

    QFile currentFile;
    QList<QShortcut *> shortCuts;

    Highlighter *highlighter;

private slots:
    void openFile();
    void saveFile();
    void newFile();

    void sendUserInput();
};

#endif // EDITORWINDOW_H
