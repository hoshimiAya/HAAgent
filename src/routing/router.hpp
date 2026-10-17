#pragma once
#include <string>
#include <functional>
#include <optional>
#include <vector>
#include <map>

#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

using Handler = std::function<http::response<http::string_body>(const http::request<http::string_body>&)>;

struct RouteMatch {
	Handler handler;
	std::map<std::string, std::string> path_params;
};

class Router {
public:
	// 注册路由处理函数
	void add_route(const std::string& method, const std::string& path, Handler handler);

	// 路由匹配函数，返回匹配的处理函数和路径参数
	std::optional<RouteMatch> match(const std::string& method, const std::string& path) const;

private:
	struct Route {
		std::string method;
		std::string path;
		Handler handler;
		std::vector<std::string> param_names; // 存储路径参数的名称
	};

	std::vector<Route> routes_;
};
