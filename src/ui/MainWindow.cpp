#include "ui/MainWindow.h"
#include "ui/FindReplaceDialog.h"
#include "editor/SmartTextEdit.h"
#include "core/VaultManager.h"
#include "core/DocumentArchitect.h"

#include <QSplitter>
#include <QListWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QStatusBar>
#include <QFile>
#include <QDir>
#include <QDesktopServices> 
#include <QMenuBar>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontComboBox>
#include <QSpinBox>
#include <QTextList>
#include <QImageReader> // NUEVO

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_findDialog(nullptr)
{
    QString vaultPath = qApp->applicationDirPath() + "/vault";
    QDir vaultDir(vaultPath);
    if (!vaultDir.exists()) vaultDir.mkpath(".");

    m_vaultManager = new VaultManager(vaultPath);
    m_architect = new DocumentArchitect(m_vaultManager, this);

    setWindowTitle("MadNotesCpp (Pro Edition)");
    resize(1400, 900);
    
    setupUI();
    
    // Load home file if exists
    QString home = m_vaultManager->getHomeFile();
    if (!home.isEmpty()) {
        auto items = m_fileListWidget->findItems(home, Qt::MatchExactly);
        if (!items.isEmpty()) m_fileListWidget->setCurrentItem(items.first());
    }
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    // Create splitter and editor first so menus/toolbars can reference m_editor
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_editor = new SmartTextEdit(m_mainSplitter);
    m_editor->setReadOnly(false);

    // Now safe to build menus and toolbar that reference m_editor
    setupMenus();
    setupToolbar();

    // Left Panel
    QWidget *leftPanel = new QWidget(m_mainSplitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    m_filterLineEdit = new QLineEdit(leftPanel);
    m_filterLineEdit->setPlaceholderText("🔍 Buscar en todo el vault...");
    connect(m_filterLineEdit, &QLineEdit::textChanged, this, &MainWindow::filterNotes);

    m_fileListWidget = new QListWidget(leftPanel);
    m_fileListWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    // Context Menu: Set as Home
    QAction *actSetHome = new QAction("🏠 Establecer como Home", m_fileListWidget);
    connect(actSetHome, &QAction::triggered, this, &MainWindow::setAsHome);
    m_fileListWidget->addAction(actSetHome);

    connect(m_fileListWidget, &QListWidget::currentItemChanged, this, &MainWindow::onFileSelected);

    leftLayout->addWidget(m_filterLineEdit);
    leftLayout->addWidget(m_fileListWidget); 
    
    // WIKILINKS Logic (Updated with Safe Check)
    connect(m_editor, &SmartTextEdit::wikiLinkActivated, this, [this](const QString &target){
        QString targetFile = target;
        if (!targetFile.endsWith(".html")) targetFile += ".html";
        
        if (m_vaultManager->listNotes().contains(targetFile, Qt::CaseInsensitive)) {
             // Navegacion normal
             if (!checkSaveModified()) return;
             auto items = m_fileListWidget->findItems(targetFile, Qt::MatchExactly);
             if (!items.isEmpty()) m_fileListWidget->setCurrentItem(items.first());
        } else {
            auto reply = QMessageBox::question(this, "Crear Nota", 
                "La nota '" + target + "' no existe. ¿Crearla?", QMessageBox::Yes | QMessageBox::No);
                
            if (reply == QMessageBox::Yes) {
                if (!checkSaveModified()) return;
                m_vaultManager->writeNote(targetFile, "<h1>" + target + "</h1><p>Vinculado.</p>");
                loadFileList(); 
                auto items = m_fileListWidget->findItems(targetFile, Qt::MatchExactly);
                if (!items.isEmpty()) m_fileListWidget->setCurrentItem(items.first());
            }
        }
    });

    m_mainSplitter->addWidget(leftPanel);
    m_mainSplitter->addWidget(m_editor);
    m_mainSplitter->setSizes({300, 1100});
    setCentralWidget(m_mainSplitter);
    
    loadFileList();
}

void MainWindow::setupMenus() {
    QMenu *mFile = menuBar()->addMenu("&Archivo");
    mFile->addAction("Nuevo", this, &MainWindow::createNewNote, QKeySequence::New);
    mFile->addAction("Guardar", this, &MainWindow::saveCurrentDocument, QKeySequence::Save);
    mFile->addAction("Abrir Maletín (Vault)...", this, &MainWindow::openVault);
    mFile->addSeparator();
    mFile->addAction("Salir", this, &MainWindow::close);

    QMenu *mEdit = menuBar()->addMenu("&Edición");
    mEdit->addAction("Deshacer", m_editor, &QTextEdit::undo, QKeySequence::Undo);
    mEdit->addAction("Rehacer", m_editor, &QTextEdit::redo, QKeySequence::Redo);
    mEdit->addSeparator();
    mEdit->addAction("Buscar y Reemplazar...", this, &MainWindow::showFindReplace, QKeySequence::Find);

    // Zoom Actions
    mEdit->addSeparator();
    QAction *zoomIn = mEdit->addAction("Zoom In", m_editor, [this](){ m_editor->zoomIn(); });
    zoomIn->setShortcut(QKeySequence::ZoomIn);
    QAction *zoomOut = mEdit->addAction("Zoom Out", m_editor, [this](){ m_editor->zoomOut(); });
    zoomOut->setShortcut(QKeySequence::ZoomOut);

    QMenu *mInsert = menuBar()->addMenu("&Insertar");
    mInsert->addAction("Imagen...", this, &MainWindow::insertImage);
    mInsert->addAction("Tabla...", this, &MainWindow::insertTable);
    mInsert->addAction("Hipervínculo...", this, &MainWindow::insertLink);
}

void MainWindow::setupToolbar() {
    QToolBar *tb = addToolBar("Format");
    tb->setMovable(false);
    
    // Core Actions
    tb->addAction("➕ New", this, &MainWindow::createNewNote);
    tb->addAction("💾 Save", this, &MainWindow::saveCurrentDocument);
    tb->addAction("🗑️ Del", this, &MainWindow::deleteCurrentNote);
    tb->addSeparator();
    
    // Font
    m_fontCombo = new QFontComboBox(tb);
    connect(m_fontCombo, &QFontComboBox::currentFontChanged, this, &MainWindow::setFontFamily);
    tb->addWidget(m_fontCombo);
    
    m_sizeSpin = new QSpinBox(tb);
    m_sizeSpin->setValue(12);
    connect(m_sizeSpin, &QSpinBox::valueChanged, this, &MainWindow::setFontSize);
    tb->addWidget(m_sizeSpin);
    
    tb->addSeparator();
    tb->addAction("𝐁", this, &MainWindow::toggleBold);
    tb->addAction("𝐼", this, &MainWindow::toggleItalic);
    tb->addAction("U̲", this, &MainWindow::toggleUnderline);
    
    tb->addSeparator();
    tb->addAction("🎨", this, &MainWindow::setTextColor);
    tb->addAction("🖍️", this, &MainWindow::setHighlightColor);
    
    tb->addSeparator();
    tb->addAction("⇤", this, &MainWindow::setAlignLeft);
    tb->addAction("⇺", this, &MainWindow::setAlignCenter);
    tb->addAction("⇥", this, &MainWindow::setAlignRight);
    tb->addAction("≣", this, &MainWindow::setAlignJustify);
    
    tb->addSeparator();
    tb->addAction("•List", this, &MainWindow::setListBullet);
    tb->addAction("1.List", this, &MainWindow::setListOrdered);
}

// --- LOGIC ---

void MainWindow::filterNotes(const QString &text) {
    m_fileListWidget->clear();
    // Usa la nueva busqueda Full Text del VaultManager
    QStringList results = m_vaultManager->searchFiles(text);
    m_fileListWidget->addItems(results);
}

void MainWindow::openVault() {
    if (!checkSaveModified()) return;
    QString dir = QFileDialog::getExistingDirectory(this, "Seleccionar Carpeta Vault");
    if (!dir.isEmpty()) {
        m_vaultManager->setVaultPath(dir);
        loadFileList();
        m_editor->clear();
        setWindowTitle("MadNotesCpp - " + dir);
    }
}

void MainWindow::setAsHome() {
    QListWidgetItem *item = m_fileListWidget->currentItem();
    if (item) {
        m_vaultManager->setHomeFile(item->text());
        statusBar()->showMessage("Home establecido: " + item->text(), 3000);
    }
}

bool MainWindow::checkSaveModified() {
    if (m_architect->isModified()) {
        auto r = QMessageBox::question(this, "Guardar Cambios", 
            "El archivo actual ha sido modificado. ¿Desea guardarlo?", 
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (r == QMessageBox::Cancel) return false;
        if (r == QMessageBox::Yes) saveCurrentDocument();
    }
    return true;
}

// --- FORMATTING SLOTS ---
void MainWindow::setFontFamily(const QFont &f) { m_editor->setCurrentFont(f); }
void MainWindow::setFontSize(int s) { m_editor->setFontPointSize(s); }
void MainWindow::toggleBold() {
    QTextCharFormat fmt = m_editor->currentCharFormat();
    fmt.setFontWeight(fmt.fontWeight() == QFont::Bold ? QFont::Normal : QFont::Bold);
    m_editor->mergeCurrentCharFormat(fmt);
}
void MainWindow::toggleItalic() {
    QTextCharFormat fmt = m_editor->currentCharFormat();
    fmt.setFontItalic(!fmt.fontItalic());
    m_editor->setCurrentCharFormat(fmt);
}
void MainWindow::toggleUnderline() {
    QTextCharFormat fmt = m_editor->currentCharFormat();
    fmt.setFontUnderline(!fmt.fontUnderline());
    m_editor->setCurrentCharFormat(fmt);
}
void MainWindow::setTextColor() {
    QColor c = QColorDialog::getColor(m_editor->currentCharFormat().foreground().color(), this);
    if (c.isValid()) {
        QTextCharFormat fmt = m_editor->currentCharFormat();
        fmt.setForeground(c);
        m_editor->setCurrentCharFormat(fmt);
    }
}
void MainWindow::setHighlightColor() {
    QColor c = QColorDialog::getColor(m_editor->currentCharFormat().background().color(), this);
    if (c.isValid()) {
        QTextCharFormat fmt = m_editor->currentCharFormat();
        fmt.setBackground(c);
        m_editor->setCurrentCharFormat(fmt);
    }
}
void MainWindow::setAlignLeft() { m_editor->setAlignment(Qt::AlignLeft); }
void MainWindow::setAlignCenter() { m_editor->setAlignment(Qt::AlignCenter); }
void MainWindow::setAlignRight() { m_editor->setAlignment(Qt::AlignRight); }
void MainWindow::setAlignJustify() { m_editor->setAlignment(Qt::AlignJustify); }

void MainWindow::setListBullet() {
    m_editor->textCursor().createList(QTextListFormat::ListDisc);
}
void MainWindow::setListOrdered() {
    m_editor->textCursor().createList(QTextListFormat::ListDecimal);
}

// --- INSERT SLOTS ---
void MainWindow::insertImage() {
    QString path = QFileDialog::getOpenFileName(this, "Insertar Imagen", "", "Images (*.png *.jpg *.bmp)");
    if (!path.isEmpty()) {
        // Auto-Resize logic (max height ~10 lines, approx 200px)
        QImage img(path);
        if (!img.isNull()) {
            if (img.height() > 200) {
                img = img.scaledToHeight(200, Qt::SmoothTransformation);
            }
            // Add resource to document and insert
            m_editor->document()->addResource(QTextDocument::ImageResource, QUrl(path), img);
            m_editor->textCursor().insertImage(path);
        }
    }
}
void MainWindow::insertTable() {
    bool ok;
    int cols = QInputDialog::getInt(this, "Tabla", "Columnas:", 3, 1, 10, 1, &ok);
    if (ok) {
        int rows = QInputDialog::getInt(this, "Tabla", "Filas:", 2, 1, 50, 1, &ok);
        if (ok) {
            QTextTableFormat fmt;
            fmt.setCellPadding(5);
            fmt.setBorder(1);
            m_editor->textCursor().insertTable(rows, cols, fmt);
        }
    }
}
void MainWindow::insertLink() {
    bool ok;
    QString url = QInputDialog::getText(this, "Link", "URL o [[Nota]]:", QLineEdit::Normal, "", &ok);
    if (ok && !url.isEmpty()) {
        m_editor->textCursor().insertHtml("<a href='" + url + "'>" + url + "</a>");
    }
}
void MainWindow::showFindReplace() {
    if (!m_findDialog) m_findDialog = new FindReplaceDialog(m_editor, this);
    m_findDialog->show();
    m_findDialog->raise();
}

// --- REST OF CORE LOGIC (Load/Save/Delete) ---
// (Estas funciones se mantienen casi igual, solo añadiendo checkSaveModified al borrar o crear)

void MainWindow::onFileSelected(QListWidgetItem *item) {
    if (!item) return;
    if (m_architect->currentFile() == item->text()) return; // Ya cargado
    if (!checkSaveModified()) {
        // Revert selection logic needed here ideally, but simple for now
        return; 
    }
    
    QString content = m_architect->loadDocument(item->text());
    bool old = m_editor->blockSignals(true);
    m_editor->setHtml(content);
    m_editor->blockSignals(old);
}

void MainWindow::createNewNote() {
    if (!checkSaveModified()) return;
    bool ok;
    QString text = QInputDialog::getText(this, "Nueva Nota", "Nombre:", QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty()) {
        QString fn = text + ".html";
        m_vaultManager->writeNote(fn, "");
        m_vaultManager->refreshCache(); // Actualizar cache de busqueda
        loadFileList();
        auto items = m_fileListWidget->findItems(fn, Qt::MatchExactly);
        if (!items.isEmpty()) m_fileListWidget->setCurrentItem(items.first());
    }
}

void MainWindow::saveCurrentDocument() {
    QString cur = m_architect->currentFile();
    if (cur.isEmpty()) { createNewNote(); return; }
    m_architect->saveDocument(cur, m_editor->toHtml());
    m_vaultManager->refreshCache(); // Importante para que el buscador encuentre lo nuevo
}

void MainWindow::deleteCurrentNote() {
    QListWidgetItem *item = m_fileListWidget->currentItem();
    if (!item) return;
    auto rep = QMessageBox::question(this, "Borrar", "¿Borrar " + item->text() + "?", QMessageBox::Yes|QMessageBox::No);
    if (rep == QMessageBox::Yes) {
        QFile::remove(m_vaultManager->vaultPath() + "/" + item->text());
        m_vaultManager->refreshCache();
        loadFileList();
        m_editor->clear();
    }
}

void MainWindow::loadFileList() {
    m_fileListWidget->clear();
    // Cache already refreshed inside VaultManager
    m_fileListWidget->addItems(m_vaultManager->listNotes());
}