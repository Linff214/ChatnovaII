// ChatServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "LogicSystem.h"
#include <csignal>
#include <thread>
#include <mutex>
#include "AsioIOServicePool.h"
#include "CServer.h"
#include "ConfigMgr.h"
#include "RedisMgr.h"
//#include "ChatServiceImpl.h"

 using namespace std;
bool bstop = false;
std::condition_variable cond_quit;//条件变量
std::mutex mutex_quit;//互斥量


int main()
{
	try {
		auto& cfg = ConfigMgr::Inst();
		auto pool = AsioIOServicePool::GetInstance();
		boost::asio::io_context  io_context;//上下文服务，收发数据或异步处理
		boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
		//设置信号集关注sigINT和SIGTERM
		signals.async_wait([&io_context, pool](auto, auto) {
			io_context.stop();
			pool->Stop();
			});
		auto port_str = cfg["SelfServer"]["Port"];
		CServer s(io_context, atoi(port_str.c_str()));
		io_context.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << endl;
	}

}

