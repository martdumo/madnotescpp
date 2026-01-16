#ifndef VAULTMANAGER_H
#define VAULTMANAGER_H

#include <QString>
#include <QStringList>

class VaultManager
{
public:
    VaultManager(const QString &vaultPath);

    QStringList listNotes() const;
    QString readNote(const QString &fileName) const;
    bool writeNote(const QString &fileName, const QString &content) const;

    QString vaultPath() const;
    void setVaultPath(const QString &path);

private:
    QString m_vaultPath;
};

#endif // VAULTMANAGER_H
