#include "editor/LinkHighlighter.h"

LinkHighlighter::LinkHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    m_linkFormat.setForeground(QColor(41, 128, 185));
    m_linkFormat.setFontUnderline(true);
    m_linkFormat.setFontWeight(QFont::Medium);

    m_wikiLinkPattern = QRegularExpression(R"(\[\[([^\]]+)\]\])");
}

void LinkHighlighter::highlightBlock(const QString &text)
{
    QRegularExpressionMatchIterator matchIterator = m_wikiLinkPattern.globalMatch(text);

    while (matchIterator.hasNext())
    {
        QRegularExpressionMatch match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), m_linkFormat);
    }
}
