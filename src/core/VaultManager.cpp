#include "core/VaultManager.h"
#include <QDir>
#include <QFile>
#include <QTextStream>

VaultManager::VaultManager(const QString &vaultPath)
    : m_vaultPath(vaultPath)
{
}

QStringList VaultManager::listNotes() const
{
    QDir vaultDir(m_vaultPath);
    QStringList filters;
    filters << "*.html" << "*.md" << "*.txt" << "*.rtf";
    vaultDir.setNameFilters(filters);
    return vaultDir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
}

QString VaultManager::readNote(const QString &fileName) const
{
    QFile file(m_vaultPath + "/" + fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // In a real app, handle this error properly
        return QString();
    }
    QTextStream in(&file);
    return in.readAll();
}

bool VaultManager::writeNote(const QString &fileName, const QString &content) const
{
    // For now, simple write. Blueprint mentions "Atomic Save", which is more complex.
    QFile file(m_vaultPath + "/" + fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    out << content;
    return true;
}

QString VaultManager::vaultPath() const
{
    return m_vaultPath;
}

void VaultManager::setVaultPath(const QString &path)
{
    m_vaultPath = path;
}
