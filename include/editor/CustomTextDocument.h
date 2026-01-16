#ifndef CUSTOMTEXTDOCUMENT_H
#define CUSTOMTEXTDOCUMENT_H

#include <QTextDocument>
#include <QUrl>

class CustomTextDocument : public QTextDocument
{
    Q_OBJECT

public:
    explicit CustomTextDocument(QObject *parent = nullptr);

protected:
    QVariant loadResource(int type, const QUrl &name) override;
};

#endif // CUSTOMTEXTDOCUMENT_H
