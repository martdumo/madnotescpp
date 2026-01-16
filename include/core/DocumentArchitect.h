#ifndef DOCUMENTARCHITECT_H
#define DOCUMENTARCHITECT_H

#include <QObject>
#include <QString>

class VaultManager;

class DocumentArchitect : public QObject
{
    Q_OBJECT
public:
    explicit DocumentArchitect(VaultManager *vault, QObject *parent = nullptr);

    // Carga segura: Lee del vault y prepara el contenido para el editor
    QString loadDocument(const QString &fileName);

    // Guardado seguro: Recibe HTML del editor, sanitiza y guarda atómicamente
    bool saveDocument(const QString &fileName, const QString &htmlContent);

    // Getters
    QString currentFile() const { return m_currentFile; }
    bool isModified() const { return m_isModified; }

signals:
    void documentSaved(const QString &fileName);
    void errorOccurred(const QString &msg);

private:
    VaultManager *m_vault;
    QString m_currentFile;
    QString m_rawBuffer; // La fuente de la verdad (Buffer Dual)
    bool m_isModified;
};

#endif // DOCUMENTARCHITECT_H
