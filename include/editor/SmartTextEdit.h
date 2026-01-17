#ifndef SMARTTEXTEDIT_H
#define SMARTTEXTEDIT_H

#include <QTextEdit>
#include "editor/CustomTextDocument.h"

class SmartTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit SmartTextEdit(QWidget *parent = nullptr);

signals:
    void wikiLinkActivated(const QString &targetName);

protected:
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override; 
    void contextMenuEvent(QContextMenuEvent *e) override;
    void wheelEvent(QWheelEvent *e) override; // Zoom Rueda
    
    // Eliminamos keyPressEvent manual, usaremos QShortcut en constructor
    
    void insertFromMimeData(const QMimeData *source) override;

private:
    QString linkAtPosition(const QPoint &pos);
    void processImage(const QImage &img);
    void resizeImage(const QUrl &url, double scale, bool fullWidth = false);
};

#endif // SMARTTEXTEDIT_H