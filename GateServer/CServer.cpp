#include "CServer.h"
#include "HttpConnection.h"
#include "AsioIOServicePool.h"
CServer::CServer(boost::asio::io_context& ioc, unsigned short& port):_ioc(ioc),
_acceptor(ioc,tcp::endpoint(tcp::v4(),port)) {

} 
void CServer::Start() {
	auto self = shared_from_this();
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(io_context);
	//让acceptor接受新连接后，交给HTTPConnection类管理
	_acceptor.async_accept(new_con->GetSocket(), [self, new_con](beast::error_code ec) {
		try {
			//出错放弃这个连接，继续监听其他链接
			if (ec) {
				self->Start();
				return;
			}
			//创建新连接，并且创建HTTPConnection类管理这个连接
			new_con->Start();
			//继续监听
			self->Start();
		}
		catch (std::exception& exp) {

		}
		});
}