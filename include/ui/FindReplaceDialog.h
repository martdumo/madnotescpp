#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QTextEdit>

class FindReplaceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FindReplaceDialog(QTextEdit *editor, QWidget *parent = nullptr);

public slots:
    void findNext();
    void replace();
    void replaceAll();

private:
    QTextEdit *m_editor;
    QLineEdit *m_txtFind;
    QLineEdit *m_txtReplace;
    QCheckBox *m_chkCase;
    
    void showMessage(const QString &msg);
};

#endif // FINDREPLACEDIALOG_H