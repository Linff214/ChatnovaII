#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <functional>
#include <QRegularExpression>
#include <memory>
#include <iostream>
#include <mutex>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonObject>
#include <QDir>
#include <QSettings>
#include "QStyle"

extern std::function<void(QWidget*)> repolish; //刷新属性函数
enum ReqId{
    ID_GET_VARIFY_CODE = 1001,//获取验证码
    ID_REG_USER = 1002,//注册用户
};
//module表示模块，比如注册模块
enum Modules{
    REGISTERMOD = 0,
};
enum ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1, //json解析失败
    ERR_NETWORK = 2,//网络错误
};
extern QString gate_url_prefix;//网关服务器的 URL 前缀,eg "http://localhost:8080";

#endif // GLOBAL_H
