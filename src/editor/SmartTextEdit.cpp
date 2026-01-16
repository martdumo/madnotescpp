#include "editor/SmartTextEdit.h"

SmartTextEdit::SmartTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    // Set the custom document with the overridden resource loader
    setDocument(new CustomTextDocument(this));

    // Basic setup from the blueprint/stylesheet
    setAcceptRichText(true);
}
