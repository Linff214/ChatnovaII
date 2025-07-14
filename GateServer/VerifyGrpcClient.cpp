#include "VerifyGrpcClient.h"
#include "ConfigMgr.h"
// 创建gRPC 连接池

    RPConPool::RPConPool(size_t poolSize, std::string host, std::string port)
        : poolSize_(poolSize), host_(host), port_(port), b_stop_(false) {
        for (size_t i = 0; i < poolSize_; ++i) {

            std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
                grpc::InsecureChannelCredentials());

            connections_.push(VarifyService::NewStub(channel));
        }
    }

    RPConPool::~RPConPool() {
        std::lock_guard<std::mutex> lock(mutex_); 
        Close();
        while (!connections_.empty()) {
            connections_.pop();
        }
    }

    std::unique_ptr<VarifyService::Stub> RPConPool::getConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] {
            // 返回 true，说明条件满足，唤醒线程继续向下执行
        //  返回 false，则线程挂起、释放 mutex_，等待 cond_.notify_one() 唤醒
            if (b_stop_) {
                return true;// 只要关闭标志被设置，就不需要再等连接了，直接退出
            }
            return !connections_.empty();//如果返回false就会先将线程挂起，并进行解锁
            });
        //如果停止则直接返回空指针
        if (b_stop_) {
            return  nullptr;// 如果关闭了，就不再返回连接，避免非法使用
        }
        auto context = std::move(connections_.front());
        connections_.pop();
        return context;
    }

    void RPConPool::returnConnection(std::unique_ptr<VarifyService::Stub> context) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (b_stop_) {
            return;
        }
        connections_.push(std::move(context));
        cond_.notify_one();
    }
    //通知挂起的线程，不用再挂起等待了
    void RPConPool::Close() {
        b_stop_ = true;
        cond_.notify_all();
    }

VerifyGrpcClient::VerifyGrpcClient() {
    auto& gCfgMgr = ConfigMgr::Inst();
    std::string host = gCfgMgr["VarifyServer"]["Host"];
    std::string port = gCfgMgr["VarifyServer"]["Port"];
    pool_.reset(new RPConPool(5, host, port));
}