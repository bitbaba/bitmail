#include <QTime>
#include <QDebug>
#include <json/json.h>

#include "threadgetmessage.h"

ThreadGetMessage::ThreadGetMessage(BitMail * instPtr, unsigned int checkInterval)
{
    m_refInst = instPtr;
    m_checkInterval = checkInterval;
}

ThreadGetMessage::~ThreadGetMessage()
{

}

void ThreadGetMessage::run()
{
    while(true){
        std::vector<std::string> vecMsg;
        m_refInst->RecvMsg(vecMsg, GetMessage_Callback, this);
        for (std::vector<std::string>::iterator it = vecMsg.begin()
             ; it != vecMsg.end()
             ; it++){
            emit MessageGot(QString::fromStdString(*it));
        }
        // have a cup of tea :)
        QThread::sleep(m_checkInterval);
    }
    return ;
}

int ThreadGetMessage::GetMessage_Callback(const std::string & sJsonParam, void *userdata)
{
    ThreadGetMessage * pThis = (ThreadGetMessage *)userdata;
    (void )pThis;
    Json::Value jsonParam;
    Json::Reader reader;
    reader.parse(sJsonParam, jsonParam);
    (void)userdata;
    if (jsonParam.empty()) {
        return 0;
    }
    if (!jsonParam.isObject()){
        return 0;
    }
    std::string msg;
    if (!jsonParam.isMember("msg")){
        return 0;
    }
    msg = jsonParam["msg"].asString();
    if (msg == "StrangerRequest"){
        Json::Value jsonMsgParam;
        if (!jsonParam.isMember("param")){
            return 0;
        }
        jsonMsgParam = jsonParam["param"];
        std::string sCertId = jsonMsgParam["certId"].asString();
        std::string sEmail = jsonMsgParam["email"].asString();
        std::string sCommonName = jsonMsgParam["commonName"].asString();
        std::string sCertPem = jsonMsgParam["cert"].asString();
        (void)sCertId; (void)sEmail; (void)sCommonName;(void)sCertPem;
        /*TODO: sigal/slot is async way, so this first "hi" message will be ignored!*/
        emit pThis->StrangerRequest(QString::fromStdString(jsonMsgParam.toStyledString()));
    }
    return 0;
}

