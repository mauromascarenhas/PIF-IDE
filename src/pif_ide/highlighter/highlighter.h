#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QVector>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QSyntaxHighlighter>

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = nullptr);

    void createRules(bool darkTheme);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat quotationFormat;
    //QTextCharFormat identifierFormat;
    QTextCharFormat booleanFormat;
};

#endif // HIGHLIGHTER_H
