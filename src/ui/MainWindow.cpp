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
#include <QSettings>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_findDialog(nullptr)
{
    setWindowTitle("MadNotesCpp");
    resize(1400, 900);
    setWindowIcon(QIcon(":/resources/app.png"));
    
    // 1. CARGAR CONFIGURACIÓN (Último Vault)
    QSettings settings("MadNotes", "Session");
    QString lastVault = settings.value("LastVault").toString();
    QString defaultVault = qApp->applicationDirPath() + "/vault";
    
    QString vaultPath = (lastVault.isEmpty() || !QDir(lastVault).exists()) ? defaultVault : lastVault;
    QDir vaultDir(vaultPath);
    if (!vaultDir.exists()) vaultDir.mkpath(".");

    m_vaultManager = new VaultManager(vaultPath);
    m_architect = new DocumentArchitect(m_vaultManager, this);

    setupUI();
    
    // 2. RESTAURAR ÚLTIMO ARCHIVO
    QString lastFile = settings.value("LastFile").toString();
    
    if (!lastFile.isEmpty() && m_vaultManager->listNotes().contains(lastFile, Qt::CaseInsensitive)) {
        // Carga directa sin esperar señales
        QString content = m_architect->loadDocument(lastFile);
        m_editor->setHtml(content);
        setWindowTitle("MadNotesCpp - " + lastFile);
        
        // Sincronizar lista visualmente
        QMetaObject::invokeMethod(this, [this, lastFile](){
             auto items = m_fileListWidget->findItems(lastFile, Qt::MatchExactly);
             if (!items.isEmpty()) {
                 bool old = m_fileListWidget->blockSignals(true);
                 m_fileListWidget->setCurrentItem(items.first());
                 m_fileListWidget->blockSignals(old);
             }
        }, Qt::QueuedConnection);
    } else {
        // Si no hay archivo previo, abrir vacío (o crear uno nuevo automático si prefieres)
        m_editor->clear();
    }
}

MainWindow::~MainWindow() {}

// --- GUARDAR SESIÓN AL CERRAR ---
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!checkSaveModified()) {
        event->ignore();
        return;
    }

    QSettings settings("MadNotes", "Session");
    settings.setValue("LastVault", m_vaultManager->vaultPath());
    settings.setValue("LastFile", m_architect->currentFile());
    
    event->accept();
}

void MainWindow::setupUI()
{
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_editor = new SmartTextEdit(m_mainSplitter);
    m_editor->setReadOnly(false);

    setupMenus();
    setupToolbar();

    // --- GLOBAL ZOOM SHORTCUTS (Solución Definitiva) ---
    // Al agregarlos a la ventana principal (this), funcionan globalmente
    QAction *actZoomIn = new QAction(this);
    actZoomIn->setShortcuts({QKeySequence::ZoomIn, QKeySequence(Qt::CTRL | Qt::Key_Equal)});
    connect(actZoomIn, &QAction::triggered, m_editor, [this](){ m_editor->zoomIn(2); });
    this->addAction(actZoomIn);

    QAction *actZoomOut = new QAction(this);
    actZoomOut->setShortcuts({QKeySequence::ZoomOut, QKeySequence(Qt::CTRL | Qt::Key_Minus)});
    connect(actZoomOut, &QAction::triggered, m_editor, [this](){ m_editor->zoomOut(2); });
    this->addAction(actZoomOut);
    
    QAction *actZoomReset = new QAction(this);
    actZoomReset->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(actZoomReset, &QAction::triggered, m_editor, [this](){ 
        QFont f = m_editor->font(); f.setPointSize(12); m_editor->setFont(f); 
    });
    this->addAction(actZoomReset);

    // --- UI PANELS ---
    QWidget *leftPanel = new QWidget(m_mainSplitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    
    m_filterLineEdit = new QLineEdit(leftPanel);
    m_filterLineEdit->setPlaceholderText("🔍 Buscar...");
    connect(m_filterLineEdit, &QLineEdit::textChanged, this, &MainWindow::filterNotes);

    m_fileListWidget = new QListWidget(leftPanel);
    m_fileListWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(m_fileListWidget, &QListWidget::currentItemChanged, this, &MainWindow::onFileSelected);

    leftLayout->addWidget(m_filterLineEdit);
    leftLayout->addWidget(m_fileListWidget);

    // LINKS
    connect(m_editor, &SmartTextEdit::wikiLinkActivated, this, [this](const QString &target){
        QString targetFile = target;
        if (!targetFile.endsWith(".html")) targetFile += ".html";
        
        if (m_vaultManager->listNotes().contains(targetFile, Qt::CaseInsensitive)) {
             if (!checkSaveModified()) return;
             QString content = m_architect->loadDocument(targetFile);
             m_editor->setHtml(content);
             auto items = m_fileListWidget->findItems(targetFile, Qt::MatchExactly);
             if (!items.isEmpty()) {
                 bool old = m_fileListWidget->blockSignals(true);
                 m_fileListWidget->setCurrentItem(items.first());
                 m_fileListWidget->blockSignals(old);
             }
        } else {
            auto reply = QMessageBox::question(this, "Crear Nota", 
                "Crear nota '" + target + "'?", QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                if (!checkSaveModified()) return;
                m_vaultManager->writeNote(targetFile, "<h1>" + target + "</h1><p>Vinculado.</p>");
                loadFileList(); 
                QString content = m_architect->loadDocument(targetFile);
                m_editor->setHtml(content);
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

void MainWindow::setAsHome() {
    QListWidgetItem *item = m_fileListWidget->currentItem();
    if (item) {
        QSettings settings("MadNotes", "MadNotesCpp");
        settings.setValue("HomeFile", item->text());
        statusBar()->showMessage("Home guardado: " + item->text(), 3000);
    }
}

// ... (RESTO DE FUNCIONES IGUALES: setupMenus, setupToolbar, formatting slots, etc) ...
// Copio las esenciales para compilación

void MainWindow::setupMenus() {
    QMenu *mFile = menuBar()->addMenu("&Archivo");
    mFile->addAction("Nuevo", this, &MainWindow::createNewNote, QKeySequence::New);
    mFile->addAction("Guardar", this, &MainWindow::saveCurrentDocument, QKeySequence::Save);
    mFile->addAction("Abrir Maletín...", this, &MainWindow::openVault);
    mFile->addSeparator();
    mFile->addAction("Salir", this, &MainWindow::close);

    QMenu *mEdit = menuBar()->addMenu("&Edición");
    mEdit->addAction("Deshacer", m_editor, &QTextEdit::undo, QKeySequence::Undo);
    mEdit->addAction("Rehacer", m_editor, &QTextEdit::redo, QKeySequence::Redo);
    mEdit->addAction("Buscar...", this, &MainWindow::showFindReplace, QKeySequence::Find);

    QMenu *mInsert = menuBar()->addMenu("&Insertar");
    mInsert->addAction("Imagen...", this, &MainWindow::insertImage);
    mInsert->addAction("Tabla...", this, &MainWindow::insertTable);
    mInsert->addAction("Hipervínculo...", this, &MainWindow::insertLink);
}

void MainWindow::setupToolbar() {
    QToolBar *tb = addToolBar("Format");
    tb->setMovable(false);
    tb->addAction("➕ New", this, &MainWindow::createNewNote);
    tb->addAction("💾 Save", this, &MainWindow::saveCurrentDocument);
    tb->addAction("🗑️ Del", this, &MainWindow::deleteCurrentNote);
    tb->addSeparator();
    
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

void MainWindow::filterNotes(const QString &text) {
    m_fileListWidget->clear();
    m_fileListWidget->addItems(m_vaultManager->searchFiles(text));
}

void MainWindow::openVault() {
    if (!checkSaveModified()) return;
    QString dir = QFileDialog::getExistingDirectory(this, "Seleccionar Carpeta Vault");
    if (!dir.isEmpty()) {
        // Guardamos inmediatamente al cambiar
        QSettings settings("MadNotes", "Session");
        settings.setValue("LastVault", dir);
        settings.setValue("LastFile", ""); // Reset archivo al cambiar vault
        
        m_vaultManager->setVaultPath(dir);
        loadFileList();
        m_editor->clear();
        setWindowTitle("MadNotesCpp - " + dir);
    }
}

bool MainWindow::checkSaveModified() {
    if (m_architect->isModified()) {
        auto r = QMessageBox::question(this, "Guardar Cambios", 
            "Archivo modificado. ¿Guardar?", 
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (r == QMessageBox::Cancel) return false;
        if (r == QMessageBox::Yes) saveCurrentDocument();
    }
    return true;
}

// FORMATTING & ACTIONS
void MainWindow::setFontFamily(const QFont &f) { m_editor->setCurrentFont(f); }
void MainWindow::setFontSize(int s) { m_editor->setFontPointSize(s); }
void MainWindow::toggleBold() { 
    QTextCharFormat fmt; fmt.setFontWeight(m_editor->fontWeight() == QFont::Bold ? QFont::Normal : QFont::Bold);
    m_editor->mergeCurrentCharFormat(fmt); 
}
void MainWindow::toggleItalic() { m_editor->setFontItalic(!m_editor->fontItalic()); }
void MainWindow::toggleUnderline() { m_editor->setFontUnderline(!m_editor->fontUnderline()); }
void MainWindow::setTextColor() {
    QColor c = QColorDialog::getColor(m_editor->textColor(), this);
    if (c.isValid()) m_editor->setTextColor(c);
}
void MainWindow::setHighlightColor() {
    QColor c = QColorDialog::getColor(m_editor->textBackgroundColor(), this);
    if (c.isValid()) m_editor->setTextBackgroundColor(c);
}
void MainWindow::setAlignLeft() { m_editor->setAlignment(Qt::AlignLeft); }
void MainWindow::setAlignCenter() { m_editor->setAlignment(Qt::AlignCenter); }
void MainWindow::setAlignRight() { m_editor->setAlignment(Qt::AlignRight); }
void MainWindow::setAlignJustify() { m_editor->setAlignment(Qt::AlignJustify); }
void MainWindow::setListBullet() { m_editor->textCursor().createList(QTextListFormat::ListDisc); }
void MainWindow::setListOrdered() { m_editor->textCursor().createList(QTextListFormat::ListDecimal); }

void MainWindow::insertImage() {
    QString path = QFileDialog::getOpenFileName(this, "Img", "", "Img (*.png *.jpg *.bmp)");
    if (!path.isEmpty()) {
        QImage img(path);
        if (!img.isNull()) {
             static int c = 0;
             QString n = QString("ins_%1.png").arg(c++);
             m_editor->document()->addResource(QTextDocument::ImageResource, QUrl(n), img);
             if (img.width()>600) img=img.scaledToWidth(600, Qt::SmoothTransformation);
             QTextImageFormat f; f.setName(n); f.setWidth(img.width()); f.setHeight(img.height());
             m_editor->textCursor().insertImage(f);
        }
    }
}
void MainWindow::insertTable() {
    bool ok; int c = QInputDialog::getInt(this,"T","Cols:",3,1,10,1,&ok);
    if(ok){ int r = QInputDialog::getInt(this,"T","Rows:",2,1,50,1,&ok);
    if(ok){ QTextTableFormat f; f.setBorder(1); m_editor->textCursor().insertTable(r,c,f); }}
}
void MainWindow::insertLink() {
    bool ok; QString u = QInputDialog::getText(this,"Link","URL:",QLineEdit::Normal,"",&ok);
    if(ok&&!u.isEmpty()) m_editor->textCursor().insertHtml("<a href='"+u+"'>"+u+"</a>");
}
void MainWindow::showFindReplace() {
    if (!m_findDialog) m_findDialog = new FindReplaceDialog(m_editor, this);
    m_findDialog->show(); m_findDialog->raise();
}
void MainWindow::onFileSelected(QListWidgetItem *i) {
    if (!i) return;
    if (m_architect->currentFile() == i->text()) return; 
    if (!checkSaveModified()) return; 
    QString c = m_architect->loadDocument(i->text());
    bool o = m_editor->blockSignals(true);
    m_editor->setHtml(c);
    m_editor->blockSignals(o);
}
void MainWindow::createNewNote() {
    if (!checkSaveModified()) return;
    bool ok; QString t = QInputDialog::getText(this,"New","Name:",QLineEdit::Normal,"",&ok);
    if (ok && !t.isEmpty()) {
        QString f = t + ".html"; m_vaultManager->writeNote(f,""); m_vaultManager->refreshCache(); loadFileList();
        auto i = m_fileListWidget->findItems(f,Qt::MatchExactly); if(!i.isEmpty()) m_fileListWidget->setCurrentItem(i.first());
    }
}
void MainWindow::saveCurrentDocument() {
    QString c = m_architect->currentFile();
    if(c.isEmpty()){createNewNote();return;}
    m_architect->saveDocument(c,m_editor->toHtml()); m_vaultManager->refreshCache();
}
void MainWindow::deleteCurrentNote() {
    QListWidgetItem *i = m_fileListWidget->currentItem(); if(!i)return;
    if(QMessageBox::question(this,"Del","Del "+i->text()+"?",QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes){
        QFile::remove(m_vaultManager->vaultPath()+"/"+i->text()); m_vaultManager->refreshCache(); loadFileList(); m_editor->clear();
    }
}
void MainWindow::loadFileList() {
    m_fileListWidget->clear();
    m_fileListWidget->addItems(m_vaultManager->listNotes());
}