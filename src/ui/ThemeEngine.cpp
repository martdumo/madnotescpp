#include "ui/ThemeEngine.h"
#include <QApplication>
#include <QFile>
#include <QString>

void ThemeEngine::apply() {
    QString style = R"(
/*
 * Tokyo Night Theme - inspired by the VSCode theme
 */
QWidget {
    background-color: #1a1b26;
    color: #a9b1d6;
    font-family: "Segoe UI", "Cantarell", "Fira Sans", "Droid Sans", "Helvetica Neue", sans-serif;
    font-size: 10pt;
}

QMainWindow {
    background-color: #1a1b26;
}

QTextEdit, QPlainTextEdit {
    background-color: #24283b;
    color: #a9b1d6;
    border: 1px solid #414868;
    border-radius: 4px;
    padding: 8px;
}

QMenuBar {
    background-color: #1a1b26;
    color: #a9b1d6;
}

QMenuBar::item {
    background: transparent;
    padding: 4px 8px;
}

QMenuBar::item:selected {
    background-color: #414868;
}

QMenu {
    background-color: #24283b;
    border: 1px solid #414868;
}

QMenu::item:selected {
    background-color: #414868;
}

QToolBar {
    background-color: #1a1b26;
    border: none;
    padding: 2px;
}

QToolButton {
    background-color: transparent;
    padding: 6px;
    border-radius: 4px;
}

QToolButton:hover {
    background-color: #414868;
}

QToolButton:pressed {
    background-color: #292e42;
}

QStatusBar {
    background-color: #1a1b26;
    color: #a9b1d6;
}

QSplitter::handle {
    background-color: #414868;
}

QSplitter::handle:horizontal {
    width: 1px;
}

QSplitter::handle:vertical {
    height: 1px;
}
    )";
    qApp->setStyleSheet(style);
}
