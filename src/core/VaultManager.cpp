#include "core/VaultManager.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

VaultManager::VaultManager(const QString &vaultPath)
    : m_vaultPath(vaultPath)
{
    refreshCache();
}

void VaultManager::setVaultPath(const QString &path) {
    m_vaultPath = path;
    refreshCache();
}

QStringList VaultManager::listNotes() const
{
    QDir vaultDir(m_vaultPath);
    QStringList filters;
    filters << "*.html" << "*.md" << "*.txt";
    return vaultDir.entryList(filters, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
}

QString VaultManager::readNote(const QString &fileName) const
{
    QFile file(m_vaultPath + "/" + fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return QString();
    QTextStream in(&file);
    return in.readAll();
}

bool VaultManager::writeNote(const QString &fileName, const QString &content) const
{
    QFile file(m_vaultPath + "/" + fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&file);
    out << content;
    return true;
}

void VaultManager::refreshCache() {
    m_contentCache.clear();
    QStringList files = listNotes();
    for (const QString &f : files) {
        QString content = readNote(f);
        m_contentCache.insert(f, content.toLower()); // Cacheamos en minúsculas para busqueda rapida
    }
}

QStringList VaultManager::searchFiles(const QString &query) {
    if (query.isEmpty()) return listNotes();
    
    QString q = query.toLower();
    QStringList results;
    
    // Buscamos en nombre Y contenido
    auto i = m_contentCache.constBegin();
    while (i != m_contentCache.constEnd()) {
        if (i.key().toLower().contains(q) || i.value().contains(q)) {
            results << i.key();
        }
        ++i;
    }
    return results;
}

void VaultManager::setHomeFile(const QString &fileName) {
    QJsonObject obj;
    obj["home"] = fileName;
    QFile file(m_vaultPath + "/config.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(obj).toJson());
    }
}

QString VaultManager::getHomeFile() const {
    QFile file(m_vaultPath + "/config.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        return doc.object()["home"].toString();
    }
    return QString();
}
