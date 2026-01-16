#include "ui/FindReplaceDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QTextDocument>
#include <QTextCursor>

FindReplaceDialog::FindReplaceDialog(QTextEdit *editor, QWidget *parent)
    : QDialog(parent), m_editor(editor)
{
    setWindowTitle("Buscar y Reemplazar");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // Quitar ?

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Find
    QHBoxLayout *h1 = new QHBoxLayout();
    h1->addWidget(new QLabel("Buscar:"));
    m_txtFind = new QLineEdit();
    h1->addWidget(m_txtFind);
    mainLayout->addLayout(h1);

    // Replace
    QHBoxLayout *h2 = new QHBoxLayout();
    h2->addWidget(new QLabel("Reemplazar:"));
    m_txtReplace = new QLineEdit();
    h2->addWidget(m_txtReplace);
    mainLayout->addLayout(h2);

    // Options
    m_chkCase = new QCheckBox("Coincidir Mayús/Minús");
    mainLayout->addWidget(m_chkCase);

    // Buttons
    QHBoxLayout *h3 = new QHBoxLayout();
    QPushButton *btnFind = new QPushButton("Buscar Sig.");
    QPushButton *btnRep = new QPushButton("Reemplazar");
    QPushButton *btnAll = new QPushButton("Reemplazar Todo");
    
    connect(btnFind, &QPushButton::clicked, this, &FindReplaceDialog::findNext);
    connect(btnRep, &QPushButton::clicked, this, &FindReplaceDialog::replace);
    connect(btnAll, &QPushButton::clicked, this, &FindReplaceDialog::replaceAll);

    h3->addWidget(btnFind);
    h3->addWidget(btnRep);
    h3->addWidget(btnAll);
    mainLayout->addLayout(h3);
}

void FindReplaceDialog::findNext()
{
    QString searchString = m_txtFind->text();
    if (searchString.isEmpty()) return;

    QTextDocument::FindFlags flags;
    if (m_chkCase->isChecked()) flags |= QTextDocument::FindCaseSensitively;

    if (!m_editor->find(searchString, flags)) {
        // Wrap around
        m_editor->moveCursor(QTextCursor::Start);
        if (!m_editor->find(searchString, flags)) {
            showMessage("No se encontraron más coincidencias.");
        }
    }
}

void FindReplaceDialog::replace()
{
    QTextCursor cursor = m_editor->textCursor();
    if (cursor.hasSelection() && cursor.selectedText() == m_txtFind->text()) {
        cursor.insertText(m_txtReplace->text());
        findNext();
    } else {
        findNext();
    }
}

void FindReplaceDialog::replaceAll()
{
    QString searchString = m_txtFind->text();
    QString replaceString = m_txtReplace->text();
    if (searchString.isEmpty()) return;

    QTextDocument::FindFlags flags;
    if (m_chkCase->isChecked()) flags |= QTextDocument::FindCaseSensitively;

    m_editor->moveCursor(QTextCursor::Start);
    int count = 0;
    while (m_editor->find(searchString, flags)) {
        m_editor->textCursor().insertText(replaceString);
        count++;
    }
    showMessage(QString("Se reemplazaron %1 ocurrencias.").arg(count));
}

void FindReplaceDialog::showMessage(const QString &msg) {
    QMessageBox::information(this, "Info", msg);
}