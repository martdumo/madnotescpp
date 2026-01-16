#include "ui/MainWindow.h"
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
#include <QTimer> // NUEVO

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QString vaultPath = qApp->applicationDirPath() + "/vault";
    QDir vaultDir(vaultPath);
    if (!vaultDir.exists()) vaultDir.mkpath(".");

    m_vaultManager = new VaultManager(vaultPath);
    m_architect = new DocumentArchitect(m_vaultManager, this);

    // Inicializar el timer de búsqueda
    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);
    m_searchTimer->setInterval(500); // 500ms de retardo
    connect(m_searchTimer, &QTimer::timeout, this, &MainWindow::triggerFilter);

    setWindowTitle("MadNotesCpp (Tokyo Edition)");
    resize(1200, 800);
    setupUI();
    loadFileList();
    
    connect(m_architect, &DocumentArchitect::documentSaved, this, [this](const QString &name){
        statusBar()->showMessage("Guardado: " + name, 3000);
        if (m_fileListWidget->findItems(name, Qt::MatchExactly).isEmpty()) {
            loadFileList();
        }
    });
    
    connect(m_architect, &DocumentArchitect::errorOccurred, this, [this](const QString &msg){
        QMessageBox::critical(this, "Error", msg);
    });
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    m_mainToolBar = addToolBar("Main Toolbar");
    m_mainToolBar->setMovable(false);

    QAction *actNew = m_mainToolBar->addAction("New");
    actNew->setShortcut(QKeySequence::New);
    connect(actNew, &QAction::triggered, this, &MainWindow::createNewNote);

    QAction *actSave = m_mainToolBar->addAction("Save");
    actSave->setShortcut(QKeySequence::Save);
    connect(actSave, &QAction::triggered, this, &MainWindow::saveCurrentDocument);

    m_mainToolBar->addSeparator();

    QAction *actDelete = m_mainToolBar->addAction("Delete");
    actDelete->setShortcut(QKeySequence::Delete);
    connect(actDelete, &QAction::triggered, this, &MainWindow::deleteCurrentNote);

    m_mainSplitter = new QSplitter(Qt::Horizontal, this);

    QWidget *leftPanel = new QWidget(m_mainSplitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(2);

    m_filterLineEdit = new QLineEdit(leftPanel);
    m_filterLineEdit->setPlaceholderText("Filter files...");
    // CONEXIÓN DEL FILTRO (ahora con debounce)
    connect(m_filterLineEdit, &QLineEdit::textChanged, this, [this](){
        m_searchTimer->start();
    });

    m_fileListWidget = new QListWidget(leftPanel);
    connect(m_fileListWidget, &QListWidget::currentItemChanged, this, &MainWindow::onFileSelected);

    leftLayout->addWidget(m_filterLineEdit);
    leftLayout->addWidget(m_fileListWidget);

    m_editor = new SmartTextEdit(m_mainSplitter);
    m_editor->setReadOnly(false);
    
    m_mainSplitter->addWidget(leftPanel);
    m_mainSplitter->addWidget(m_editor);
    m_mainSplitter->setSizes({250, 950});

    setCentralWidget(m_mainSplitter);
}

void MainWindow::loadFileList()
{
    m_fileListWidget->clear();
    m_allNotesCache = m_vaultManager->listNotes();
    m_fileListWidget->addItems(m_allNotesCache);
}

void MainWindow::triggerFilter()
{
    QString text = m_filterLineEdit->text();

    // Limpiar la lista actual
    m_fileListWidget->clear();

    if (text.isEmpty()) {
        // Si no hay texto de búsqueda, mostrar todos los archivos
        m_fileListWidget->addItems(m_allNotesCache);
        return;
    }

    // Filtrar y agregar solo los archivos que coincidan
    for (const QString &note : m_allNotesCache) {
        if (note.contains(text, Qt::CaseInsensitive)) {
            m_fileListWidget->addItem(note);
        }
    }
}

void MainWindow::onFileSelected(QListWidgetItem *item)
{
    if (!item) return;
    QString fileName = item->text();
    QString content = m_architect->loadDocument(fileName);
    bool oldState = m_editor->blockSignals(true);
    m_editor->setHtml(content);
    m_editor->blockSignals(oldState);
}

void MainWindow::handleAnchorClicked(const QUrl &url)
{
    if (url.scheme() == "model") {
        QString target = url.host();
        statusBar()->showMessage("Navegando a: " + target, 2000);
    } else {
        QDesktopServices::openUrl(url);
    }
}

void MainWindow::createNewNote()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Nueva Nota",
                                         "Nombre del archivo (sin extensión):", QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()) {
        QString fileName = text + ".html";
        if (m_vaultManager->writeNote(fileName, "")) {
            loadFileList();
            auto items = m_fileListWidget->findItems(fileName, Qt::MatchExactly);
            if (!items.isEmpty()) {
                m_fileListWidget->setCurrentItem(items.first());
            }
            m_editor->setFocus();
        } else {
            QMessageBox::warning(this, "Error", "No se pudo crear el archivo.");
        }
    }
}

void MainWindow::saveCurrentDocument()
{
    QString currentFile = m_architect->currentFile();
    if (currentFile.isEmpty()) {
        createNewNote();
        return;
    }
    m_architect->saveDocument(currentFile, m_editor->toHtml());
}

void MainWindow::deleteCurrentNote()
{
    QListWidgetItem *item = m_fileListWidget->currentItem();
    if (!item) return;

    QString fileName = item->text();
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmar Borrado",
                                  "¿Eliminar " + fileName + "?",
                                  QMessageBox::Yes|QMessageBox::No);
                                  
    if (reply == QMessageBox::Yes) {
        QFile file(m_vaultManager->vaultPath() + "/" + fileName);
        if (file.remove()) {
            m_editor->clear();
            loadFileList(); 
            m_allNotesCache.removeAll(fileName); // Actualizar cache
            statusBar()->showMessage("Eliminado.", 3000);
        } else {
            QMessageBox::critical(this, "Error", "No se pudo eliminar.");
        }
    }
}