#ifndef VAULTMANAGER_H
#define VAULTMANAGER_H

#include <QString>
#include <QStringList>
#include <QMap>

class VaultManager
{
public:
    VaultManager(const QString &vaultPath);

    QStringList listNotes() const;
    QString readNote(const QString &fileName) const;
    bool writeNote(const QString &fileName, const QString &content) const;
    
    QString vaultPath() const { return m_vaultPath; }
    void setVaultPath(const QString &path);

    // Full Text Search
    void refreshCache();
    QStringList searchFiles(const QString &query);

    // Config (Home File)
    void setHomeFile(const QString &fileName);
    QString getHomeFile() const;

private:
    QString m_vaultPath;
    QMap<QString, QString> m_contentCache; // NombreArchivo -> Contenido (minusculas para search)
};

#endif // VAULTMANAGER_H
