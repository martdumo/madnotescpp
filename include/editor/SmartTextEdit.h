#ifndef SMARTTEXTEDIT_H
#define SMARTTEXTEDIT_H

#include <QTextEdit>
#include "editor/CustomTextDocument.h"

class SmartTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit SmartTextEdit(QWidget *parent = nullptr);

    // Zoom Logic
    void zoomIn(int range = 1);
    void zoomOut(int range = 1);

signals:
    void wikiLinkActivated(const QString &targetName);

protected:
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override; // Captura Ctrl+Scroll

private:
    QString linkAtPosition(const QPoint &pos);
};

#endif // SMARTTEXTEDIT_H
