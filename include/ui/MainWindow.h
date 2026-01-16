#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>

// Forward declarations
class SmartTextEdit;
class QSplitter;
class QListWidget;
class QLineEdit;
class QToolBar;
class VaultManager;
class DocumentArchitect;
class QListWidgetItem;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onFileSelected(QListWidgetItem *item);
    
    // Acciones Toolbar
    void createNewNote();
    void saveCurrentDocument();
    void deleteCurrentNote();

    // Filtro
    void triggerFilter();

    // Navegación
    void handleAnchorClicked(const QUrl &url);

private:
    void setupUI();
    void loadFileList();

    // Core Logic
    VaultManager *m_vaultManager;
    DocumentArchitect *m_architect;

    // UI Components
    QSplitter *m_mainSplitter;
    QListWidget *m_fileListWidget;
    QLineEdit *m_filterLineEdit;
    SmartTextEdit *m_editor;
    QToolBar *m_mainToolBar;
    
    // Cache para búsqueda rápida y debounce
    QStringList m_allNotesCache;
    QTimer *m_searchTimer;
};

#endif // MAINWINDOW_H
