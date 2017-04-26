#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QObject>
#include <QTextEdit>

class TextEdit : public QTextEdit
{
    Q_OBJECT
public:
    TextEdit(QWidget * parent = NULL);
    ~TextEdit();
protected:
    bool canInsertFromMimeData( const QMimeData *source ) const;
    void insertFromMimeData( const QMimeData *source );
};

#endif // TEXTEDIT_H
