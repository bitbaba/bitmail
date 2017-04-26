#include "textedit.h"
#include <QMimeData>
#include <QVariant>
#include <QVariantList>

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
    {/*Paste image from clipboard*/
        QImage image = qvariant_cast<QImage>(source->imageData());
        /*
        QTextCursor cursor = this->textCursor();
        QTextDocument *document = this->document();
        document->addResource(QTextDocument::ImageResource, QUrl("image"), image);
        cursor.insertImage("image");
        */
        QVariantList varlist;
        varlist.append(QVariant::fromValue(image));
        emit dropped(varlist);
        return ;
    }
    if (source->hasUrls())
    {/*drag files from shell*/
        QVariantList varlist;
        for (int i = 0; i < source->urls().length(); i++){
            QList<QUrl> list = source->urls();
            QFileInfo finfo(source->urls().at(i).toLocalFile());
            varlist.append(QVariant::fromValue(finfo));
        }
        if (varlist.size()){
            emit dropped(varlist);
        }
        return ;
    }
    QTextEdit::insertFromMimeData(source);
}
