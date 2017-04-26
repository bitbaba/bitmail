#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QObject>
#include <QTextEdit>
#include <QImage>
#include <QFileInfo>
#include <QFile>

class TextEdit : public QTextEdit
{
    Q_OBJECT
public:
    TextEdit(QWidget * parent = NULL);
    ~TextEdit();
protected:
    bool canInsertFromMimeData( const QMimeData *source ) const;
    void insertFromMimeData( const QMimeData *source );
signals:
    void dropped(const QVariantList & );
};

#endif // TEXTEDIT_H
