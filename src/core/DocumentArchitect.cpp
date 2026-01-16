#include "core/DocumentArchitect.h"
#include "core/VaultManager.h"
#include <QSaveFile>
#include <QTextStream>

DocumentArchitect::DocumentArchitect(VaultManager *vault, QObject *parent)
    : QObject(parent), m_vault(vault), m_isModified(false)
{
}

QString DocumentArchitect::loadDocument(const QString &fileName)
{
    m_currentFile = fileName;
    // Aquí podríamos convertir MD a HTML si fuera necesario.
    // Por ahora, asumimos que VaultManager devuelve el contenido crudo.
    m_rawBuffer = m_vault->readNote(fileName); 
    m_isModified = false;
    return m_rawBuffer;
}

#include <QSaveFile>
#include <QTextStream>

bool DocumentArchitect::saveDocument(const QString &fileName, const QString &htmlContent)
{
    if (!m_vault) {
        emit errorOccurred("VaultManager no inicializado.");
        return false;
    }

    m_rawBuffer = htmlContent;

    QString targetPath = m_vault->vaultPath() + "/" + fileName;
    QSaveFile saveFile(targetPath);

    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred("No se pudo abrir el archivo de guardado: " + saveFile.errorString());
        return false;
    }

    QTextStream out(&saveFile);
    out << m_rawBuffer;

    if (!saveFile.commit()) {
        emit errorOccurred("Fallo crítico al finalizar el guardado seguro: " + saveFile.errorString());
        return false;
    }

    m_isModified = false;
    m_currentFile = fileName;
    emit documentSaved(fileName);
    return true;
}
