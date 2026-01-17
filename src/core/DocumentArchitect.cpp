#include "core/DocumentArchitect.h"
#include "core/VaultManager.h"
#include <QSaveFile>
#include <QTextStream>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

DocumentArchitect::DocumentArchitect(VaultManager *vault, QObject *parent)
    : QObject(parent), m_vault(vault), m_isModified(false)
{
}

QString DocumentArchitect::loadDocument(const QString &fileName)
{
    QString fullPath = m_vault->vaultPath() + "/" + fileName;
    QFileInfo info(fullPath);
    
    // --- FILE GUARD (Protección Archivos Gigantes) ---
    qint64 sizeMB = info.size() / (1024 * 1024);
    if (sizeMB > 5) { // Limite flexible de 5MB
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(nullptr, "Archivo Pesado", 
            QString("El archivo '%1' pesa %2 MB.\nAbrirlo podría ralentizar la aplicación.\n¿Desea continuar?").arg(fileName).arg(sizeMB),
            QMessageBox::Yes | QMessageBox::No);
            
        if (reply == QMessageBox::No) return QString();
    }

    m_currentFile = fileName;
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
