#ifndef SMARTTEXTEDIT_H
#define SMARTTEXTEDIT_H

#include <QTextEdit>
#include "editor/CustomTextDocument.h"

class SmartTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit SmartTextEdit(QWidget *parent = nullptr);
};

#endif // SMARTTEXTEDIT_H
