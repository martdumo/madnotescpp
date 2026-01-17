#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>

class SmartTextEdit;
class QSplitter;
class QListWidget;
class QLineEdit;
class QToolBar;
class VaultManager;
class DocumentArchitect;
class QListWidgetItem;
class FindReplaceDialog;
class QComboBox;
class QFontComboBox;
class QSpinBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // Core Actions
    void createNewNote();
    void saveCurrentDocument();
    void deleteCurrentNote();
    void openVault();
    void setAsHome();
    
    // Edit Actions
    void showFindReplace();
    void insertImage();
    void insertTable();
    void insertLink();
    
    // Formatting Actions
    void setFontFamily(const QFont &f);
    void setFontSize(int size);
    void toggleBold();
    void toggleItalic();
    void toggleUnderline();
    void setTextColor();
    void setHighlightColor();
    void setAlignLeft();
    void setAlignCenter();
    void setAlignRight();
    void setAlignJustify();
    void setListBullet();
    void setListOrdered();

    // Logic
    void onFileSelected(QListWidgetItem *item);
    void filterNotes(const QString &text);
    bool checkSaveModified(); // Retorna true si es seguro continuar

private:
    void setupUI();
    void setupMenus();
    void setupToolbar();
    void loadFileList();

    VaultManager *m_vaultManager;
    DocumentArchitect *m_architect;
    FindReplaceDialog *m_findDialog;

    QSplitter *m_mainSplitter;
    QListWidget *m_fileListWidget;
    QLineEdit *m_filterLineEdit;
    SmartTextEdit *m_editor;
    
    // Formatting Widgets
    QFontComboBox *m_fontCombo;
    QSpinBox *m_sizeSpin;
};

#endif // MAINWINDOW_H