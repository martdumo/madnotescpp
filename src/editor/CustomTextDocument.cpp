#include "editor/CustomTextDocument.h"
#include <QImage>
#include <QVariant>
#include <QFileInfo>

CustomTextDocument::CustomTextDocument(QObject *parent)
    : QTextDocument(parent)
{
}

QVariant CustomTextDocument::loadResource(int type, const QUrl &name)
{
    if (type == QTextDocument::ImageResource) {
        // Handle local file paths
        if (name.isLocalFile()) {
            QImage image(name.toLocalFile());
            if (!image.isNull()) {
                return image;
            }
        }
    }
    // Fallback to default implementation for other resources (e.g., qrc)
    return QTextDocument::loadResource(type, name);
}
