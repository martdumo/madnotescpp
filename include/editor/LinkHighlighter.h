#ifndef LINKHIGHLIGHTER_H
#define LINKHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class LinkHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit LinkHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    QTextCharFormat m_linkFormat;
    QRegularExpression m_wikiLinkPattern;
};

#endif // LINKHIGHLIGHTER_H
