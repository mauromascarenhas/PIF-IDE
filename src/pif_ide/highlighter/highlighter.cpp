#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent):
    QSyntaxHighlighter (parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    const QString keywordPatterns[] = {
            QStringLiteral("\\bliteral\\b"), QStringLiteral("\\bnumérico\\b"), QStringLiteral("\\bbooleano\\b"),

            QStringLiteral("\\bprograma\\b"), QStringLiteral("\\bsenão\\b"), QStringLiteral("\\bse\\b"),
            QStringLiteral("\\bentão\\b"), QStringLiteral("\\bfim-se\\b"), QStringLiteral("\\bfaça\\b"),
            QStringLiteral("\\benquanto\\b"), QStringLiteral("\\bfim-enquanto\\b"), QStringLiteral("\\bfim-programa\\b"),

            QStringLiteral("\\bleia\\b"), QStringLiteral("\\bescreva\\b"),

            QStringLiteral("\\be\\b"), QStringLiteral("\\bou\\b"),
    };

    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    booleanFormat.setFontWeight(QFont::Bold);
    booleanFormat.setForeground(Qt::darkYellow);
    rule.pattern = QRegularExpression(QStringLiteral("\\b(verdadeiro|falso)\\b"));
    rule.format = booleanFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression(QStringLiteral("#[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);
}

void Highlighter::highlightBlock(const QString &text){
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);
}
