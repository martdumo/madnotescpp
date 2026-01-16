#include "editor/SmartTextEdit.h"
#include <QMouseEvent>
#include <QTextCursor>
#include <QTextBlock>
#include <QRegularExpression>
#include <QApplication>
#include <QDebug>
#include <QScrollBar>
#include <QDesktopServices>
#include <QUrl>

SmartTextEdit::SmartTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    setDocument(new CustomTextDocument(this));
    setAcceptRichText(true);
    setMouseTracking(true); // CRITICAL for hover effects
    setStyleSheet("selection-background-color: #3d59a1;");
}

QString SmartTextEdit::linkAtPosition(const QPoint &pos)
{
    QTextCursor cursor = cursorForPosition(pos);
    QTextBlock block = cursor.block();
    QString text = block.text();
    int posInBlock = cursor.positionInBlock();

    // Regex must match exactly [[...]]
    static QRegularExpression regex("\\[\\[(.*?)\\]\\]");
    QRegularExpressionMatchIterator i = regex.globalMatch(text);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        // Check strict bounds
        if (match.capturedStart() <= posInBlock && match.capturedEnd() >= posInBlock) {
            return match.captured(1);
        }
    }
    return QString();
}

void SmartTextEdit::mouseMoveEvent(QMouseEvent *e)
{
    // 1. Verificar WikiLink [[...]]
    QString wiki = linkAtPosition(e->pos());

    // 2. Verificar Link HTML Standard <a href...>
    QString anchor = anchorAt(e->pos());

    if (!wiki.isEmpty() || !anchor.isEmpty()) {
        viewport()->setCursor(Qt::PointingHandCursor);
    } else {
        viewport()->setCursor(Qt::IBeamCursor);
    }
    QTextEdit::mouseMoveEvent(e);
}

void SmartTextEdit::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        // A. Prioridad: WikiLinks Internos
        QString wiki = linkAtPosition(e->pos());
        if (!wiki.isEmpty()) {
            emit wikiLinkActivated(wiki);
            return;
        }

        // B. HTML Links Externos (http://...)
        QString anchor = anchorAt(e->pos());
        if (!anchor.isEmpty()) {
            QDesktopServices::openUrl(QUrl(anchor));
            return;
        }
    }

    // Si no es link, comportamiento normal (mover cursor)
    QTextEdit::mouseReleaseEvent(e);
}

void SmartTextEdit::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->angleDelta().y() > 0) zoomIn(1);
        else zoomOut(1);
        e->accept();
    } else {
        QTextEdit::wheelEvent(e);
    }
}

void SmartTextEdit::zoomIn(int range) {
    QFont f = font();
    f.setPointSize(f.pointSize() + range);
    setFont(f);
}

void SmartTextEdit::zoomOut(int range) {
    QFont f = font();
    int newSize = f.pointSize() - range;
    if (newSize > 0) f.setPointSize(newSize);
    setFont(f);
}
