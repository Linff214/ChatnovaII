#pragma once
#include "Singleton.h"
#include <functional>
#include <map>
#include "const.h"
//请求处理逻辑调度中心（或叫路由分发器）。
class HttpConnection;//前向声明，避免头文件循环引用；适用于
                     //只使用指针或引用（比如 std::shared_ptr<HttpConnection>）的场合。

//这是一个类型别名，定义了一个函数对象类型 HttpHandler，其含义是：
//一个接受 std::shared_ptr<HttpConnection> 参数、返回 void 的函数或函数对象。
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;
class LogicSystem :public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem() {};
    bool HandleGet(std::string, std::shared_ptr<HttpConnection>);//在收到 GET 请求时查找并调用对应的处理器,参数里是url和智能指针
    bool HandlePost(std::string, std::shared_ptr<HttpConnection>);
    void RegGet(std::string, HttpHandler handler);//注册一个 GET 路由处理函数,参数里是url和处理函数
    void RegPost(std::string, HttpHandler handler);
    
private:
    LogicSystem();
    static void SendJson(std::shared_ptr<HttpConnection> connection, const Json::Value& root);
    std::map<std::string, HttpHandler> _post_handlers;//post请求处理的集合和get请求的集合
    std::map<std::string, HttpHandler> _get_handlers;
};


