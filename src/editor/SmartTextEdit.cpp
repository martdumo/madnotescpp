#include "editor/SmartTextEdit.h"
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QTextCursor>
#include <QTextBlock>
#include <QRegularExpression>
#include <QApplication>
#include <QDebug>
#include <QScrollBar>
#include <QDesktopServices>
#include <QUrl>
#include <QMimeData>
#include <QImageReader>
#include <QInputDialog>
#include <QShortcut> // CRÍTICO PARA ZOOM
#include <QKeySequence>

SmartTextEdit::SmartTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    setDocument(new CustomTextDocument(this));
    setAcceptRichText(true);
    setMouseTracking(true);
    setStyleSheet("selection-background-color: #3d59a1;");

    // --- ZOOM TOKYOVAULT STYLE (QShortcuts) ---
    // Ctrl + Plus
    QShortcut *zoomIn1 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Plus), this);
    connect(zoomIn1, &QShortcut::activated, [this](){ zoomIn(2); });

    // Ctrl + Equal (Para laptops donde + es Shift+=)
    QShortcut *zoomIn2 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Equal), this);
    connect(zoomIn2, &QShortcut::activated, [this](){ zoomIn(2); });

    // Ctrl + Minus
    QShortcut *zoomOut1 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus), this);
    connect(zoomOut1, &QShortcut::activated, [this](){ zoomOut(2); });

    // Ctrl + 0 (Reset)
    QShortcut *zoomReset = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_0), this);
    connect(zoomReset, &QShortcut::activated, [this](){
        QFont f = font();
        f.setPointSize(12); // Default size
        setFont(f);
    });
}

QString SmartTextEdit::linkAtPosition(const QPoint &pos)
{
    QTextCursor cursor = cursorForPosition(pos);
    QTextBlock block = cursor.block();
    QString text = block.text();
    int posInBlock = cursor.positionInBlock();

    static QRegularExpression regex("\\[\\[(.*?)\\]\\]");
    QRegularExpressionMatchIterator i = regex.globalMatch(text);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        if (match.capturedStart() <= posInBlock && match.capturedEnd() >= posInBlock) {
            return match.captured(1); 
        }
    }
    return QString();
}

void SmartTextEdit::mouseMoveEvent(QMouseEvent *e)
{
    QString wiki = linkAtPosition(e->pos());
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
        QString wiki = linkAtPosition(e->pos());
        if (!wiki.isEmpty()) {
            emit wikiLinkActivated(wiki);
            return; 
        }
        QString anchor = anchorAt(e->pos());
        if (!anchor.isEmpty()) {
            QDesktopServices::openUrl(QUrl(anchor));
            return;
        }
    }
    QTextEdit::mouseReleaseEvent(e);
}

// --- ZOOM LOGIC ---
void SmartTextEdit::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        const int delta = e->angleDelta().y();
        if (delta > 0) {
            zoomIn(2); // Zoom más agresivo (2 puntos)
        } else {
            zoomOut(2);
        }
        e->accept();
    } else {
        QTextEdit::wheelEvent(e);
    }
}

void SmartTextEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
    QTextEdit::mouseDoubleClickEvent(e);
}

// --- PASTE LOGIC (SPEED MODE) ---
void SmartTextEdit::insertFromMimeData(const QMimeData *source)
{
    // 1. Imagen Pura (Portapapeles directo) -> Procesar Localmente
    if (source->hasImage()) {
        QImage img = qvariant_cast<QImage>(source->imageData());
        if (!img.isNull()) {
            processImage(img);
            return;
        }
    }

    // 2. HTML/Texto -> Pegado Nativo Rápido
    // Quitamos la lógica de descarga de red. 
    // Si el HTML tiene imágenes remotas, Qt mostrará un placeholder vacío rápido.
    // El usuario prefiere velocidad.
    QTextEdit::insertFromMimeData(source);
}

void SmartTextEdit::processImage(const QImage &img)
{
    static int pImgCount = 0;
    QString urlName = QString("pasted_img_%1.png").arg(pImgCount++);
    document()->addResource(QTextDocument::ImageResource, QUrl(urlName), img);
    
    QImage visible = img;
    // Auto-scale inicial solo para que no sea gigante, luego el usuario redimensiona
    if (visible.width() > 800) visible = visible.scaledToWidth(800, Qt::SmoothTransformation);
    
    QTextImageFormat fmt;
    fmt.setName(urlName);
    fmt.setWidth(visible.width());
    fmt.setHeight(visible.height());
    textCursor().insertImage(fmt);
    textCursor().insertBlock();
}

// --- RESIZE CONTEXT MENU ---
void SmartTextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    QPoint mousePos = e->pos();
    QTextCursor cursor = cursorForPosition(mousePos);
    QTextCharFormat fmt = cursor.charFormat();
    
    if (fmt.isImageFormat()) {
        QMenu *menu = createStandardContextMenu();
        menu->addSeparator();
        QMenu *resizeMenu = menu->addMenu("🖼️ Redimensionar Imagen");
        
        QTextImageFormat imgFmt = fmt.toImageFormat();
        QUrl imgUrl(imgFmt.name());

        auto resizeFn = [this, imgUrl](double s, bool full){ 
            resizeImage(imgUrl, s, full); 
        };

        resizeMenu->addAction("Pequeño (25%)", [=](){ resizeFn(0.25, false); });
        resizeMenu->addAction("Mediano (50%)", [=](){ resizeFn(0.50, false); });
        resizeMenu->addAction("Grande (75%)", [=](){ resizeFn(0.75, false); });
        resizeMenu->addAction("Original (100%)", [=](){ resizeFn(1.0, false); });
        resizeMenu->addSeparator();
        resizeMenu->addAction("↔ Ancho Completo", [=](){ resizeFn(1.0, true); });

        menu->exec(e->globalPos());
        delete menu;
        return;
    }
    QTextEdit::contextMenuEvent(e);
}

void SmartTextEdit::resizeImage(const QUrl &url, double scale, bool fullWidth)
{
    QTextCursor docCursor(document());
    
    while (!docCursor.atEnd()) {
        QTextCharFormat fmt = docCursor.charFormat();
        if (fmt.isImageFormat() && fmt.toImageFormat().name() == url.toString()) {
            QVariant resource = document()->resource(QTextDocument::ImageResource, url);
            QImage origImg = resource.value<QImage>();
            
            if (!origImg.isNull()) {
                QTextImageFormat newFmt = fmt.toImageFormat();
                int newW = fullWidth ? (viewport()->width() - 40) : (origImg.width() * scale);
                int newH = fullWidth ? ((double)newW / origImg.width() * origImg.height()) : (origImg.height() * scale);
                
                newFmt.setWidth(newW);
                newFmt.setHeight(newH);
                
                QTextCursor updateCursor = docCursor;
                updateCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                updateCursor.setCharFormat(newFmt);
            }
            break; 
        }
        docCursor.movePosition(QTextCursor::NextCharacter);
    }
}