#include "httpmgr.h"

HttpMgr::~HttpMgr() {

}

HttpMgr::HttpMgr() {
    //你需要对网络请求的返回做进一步处理，而槽函数就是响应“请求完成”这一事件的逻辑处理代码。
    //把槽和构造函数连起来,然后回到注册模块声明这个信号
    connect(this,&HttpMgr::sig_http_finish,this,&HttpMgr::slot_http_finish);
}

void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(data.length()));
    auto self = shared_from_this();
    //发送完请求后并不能马上收到对方的回包
    QNetworkReply * reply = _manager.post(request, data);
    //[]在回调触发之前httpmgr不能被删除，所以httpmgr在外部使用要用智能指针管理，内部要用shared_from_this()生成智能指针，二者共享引用计数
    QObject::connect(reply,&QNetworkReply::finished,[self,reply,req_id,mod](){
        //处理错误情况
        if(reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            //发送信号通知完成
            emit self->sig_http_finish(req_id,"",ErrorCodes::ERR_NETWORK,mod);
            reply->deleteLater();
            return;
        }
        //无错误
        QString res = reply->readAll();
        //发送信号通知完成，之后要定义一个槽函数，接受这个信号；
        emit self->sig_http_finish(req_id,res,ErrorCodes::SUCCESS,mod);
        reply->deleteLater();
        return;
    });
}

void HttpMgr::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod){
    if(mod == Modules::REGISTERMOD){
        emit sig_reg_mod_finish(id, res, err);
    }
    if(mod == Modules::RESETMOD){
        emit sig_reset_mod_finish(id, res, err);
    }
}
