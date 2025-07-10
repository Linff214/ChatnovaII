#ifndef SINGLETON_H
#define SINGLETON_H
#include <global.h>
//不是要访问 Singleton 的功能，而是希望 Singleton 帮忙创建并持有类型 T 的唯一实例。
//给任意类 T 加上一个“全局唯一 + 延迟构造 + 自动线程安全”的能力。
//类的static变量是一定要被初始化的，如果static的初始化不是一个模板类，就要在cpp里声明，否则就要在.h中
template <typename T>
class Singleton{
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator = (const Singleton<T>& st) =delete;
    static std::shared_ptr<T> _instance;
public:
    static std::shared_ptr<T> GetInstance(){
        static std::once_flag s_flag;
        std::call_once(s_flag,[&](){
            _instance = std::shared_ptr<T>(new T);
        });
        return _instance;
    }
    void PrintAddress(){
        std::cout << _instance.get() << std::endl;
    }
    ~Singleton() {
        std::cout << "this is singleton destruct" << std::endl;
    }

};
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;
#endif // SINGLETON_H
