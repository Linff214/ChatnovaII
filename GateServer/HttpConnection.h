#pragma once
#include "const.h"
class HttpConnection:public std::enable_shared_from_this<HttpConnection>
{
	
public:
	friend class LogicSystem;
	HttpConnection(boost::asio::io_context &ioc);
	void Start();
	tcp::socket& GetSocket() {
		return _socket;
	}
private:
	void CheckDeadline();//超时检测函数，超过一定时间（例如 60 秒）没有完成请求或响应，就关闭连接；
	void WriteResponse();//写回响应的函数
	void HandleReq();//处理请求
	void PreParseGetParam();
	tcp::socket _socket;
	beast::flat_buffer _buffer{ 8192 };//字节数组，接受对方发来的数据，8k的缓存
	http::request<http::dynamic_body> _request;
	http::response<http::dynamic_body> _response;
	net::steady_timer deadline_{
		_socket.get_executor(),std::chrono::seconds(60)
	};//定时器，检测超时
	std::string _get_url;
	std::unordered_map<std::string, std::string> _get_params;
};

