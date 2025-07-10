#ifndef HTTPMGR_H
#define HTTPMGR_H
#include "singleton.h"
#include <Qstring>
#include <Qurl>
#include <QObject>
#include <QNetworkAccessManager>
#include<QJsonObject>

#include<QJsonDocument>
#include "global.h"

class HttpMgr:public QObject, public Singleton<HttpMgr>, public std::enable_shared_from_this<HttpMgr>{
    Q_OBJECT;
public:
    ~HttpMgr();
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);
private:
    friend class Singleton<HttpMgr>;
    HttpMgr();
    QNetworkAccessManager _manager;


//槽函数最好在信号之前写
private slots:
    //槽函数的参数要和信号的参数匹配，信号的参数可以多于槽函数的参数
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
signals:
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
};

#endif // HTTPMGR_H
