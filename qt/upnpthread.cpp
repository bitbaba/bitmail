#include "upnpthread.h"

#include <bitmailcore/bitmail.h>

UpnpThread::UpnpThread(BitMail * bitmail)
    : m_bitmail(bitmail)
{

}
UpnpThread::~UpnpThread()
{

}

void UpnpThread::run()
{
    if (m_bitmail){
        if(m_bitmail->MapBradExtPort()){
          emit done(true, QString::fromStdString(m_bitmail->GetBradExtUrl()));
        }else{
          emit done(false, "");
        }
    }
}
