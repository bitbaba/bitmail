#include "textedit.h"
#include <QMimeData>

TextEdit::TextEdit(QWidget * parent)
    : QTextEdit(parent)
{

}

TextEdit::~TextEdit()
{

}

bool TextEdit::canInsertFromMimeData( const QMimeData *source ) const
{
    if (source->hasImage())
        return true;
    else if (source->hasUrls())
        return true;
    else
        return QTextEdit::canInsertFromMimeData(source);
}

void TextEdit::insertFromMimeData( const QMimeData *source )
{
    if (source->hasImage())
    {
        QImage image = qvariant_cast<QImage>(source->imageData());
        QTextCursor cursor = this->textCursor();
        QTextDocument *document = this->document();
        document->addResource(QTextDocument::ImageResource, QUrl("image"), image);
        cursor.insertImage("image");
    }
    if (source->hasUrls()){
        QImage image(source->urls().at(0).toLocalFile());
        QTextCursor cursor = this->textCursor();
        QTextDocument *document = this->document();
        document->addResource(QTextDocument::ImageResource, QUrl("image"), image);
        cursor.insertImage("image");
    }
}
