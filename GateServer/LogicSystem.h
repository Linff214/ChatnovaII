#pragma once
#include "Singleton.h"
#include <functional>
#include <map>
#include "const.h"
//�������߼��������ģ����·�ɷַ�������
class HttpConnection;//ǰ������������ͷ�ļ�ѭ�����ã�������
                     //ֻʹ��ָ������ã����� std::shared_ptr<HttpConnection>���ĳ��ϡ�

//����һ�����ͱ�����������һ�������������� HttpHandler���京���ǣ�
//һ������ std::shared_ptr<HttpConnection> ���������� void �ĺ�����������
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;
class LogicSystem :public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem() {};
    bool HandleGet(std::string, std::shared_ptr<HttpConnection>);//���յ� GET ����ʱ���Ҳ����ö�Ӧ�Ĵ�����,��������url������ָ��
    bool HandlePost(std::string, std::shared_ptr<HttpConnection>);
    void RegGet(std::string, HttpHandler handler);//ע��һ�� GET ·�ɴ�����,��������url�ʹ�����
    void RegPost(std::string, HttpHandler handler);
    
private:
    LogicSystem();
    static void SendJson(std::shared_ptr<HttpConnection> connection, const Json::Value& root);
    std::map<std::string, HttpHandler> _post_handlers;//post������ļ��Ϻ�get����ļ���
    std::map<std::string, HttpHandler> _get_handlers;
};


