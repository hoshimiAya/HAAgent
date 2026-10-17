#include "router.hpp"

#include <sstream>
#include <regex>

void Router::add_route(const std::string& method, const std::string& path, Handler handler)
{
	Route route;
	route.method = method;
	route.path = path;
	route.handler = handler;

	std::istringstream stream(path);
	std::string segment;
	std::string pattern;

	if (path == "/") {
		pattern = "/";
	}

	while (std::getline(stream, segment, '/')) {
		if (segment.empty()) {
			continue;
		}

		if (segment[0] == ':') {
			route.param_names.push_back(segment.substr(1));
			pattern += "/([^/]+)";
		}
		else {
			pattern += "/" + segment;
		}
	}

	route.path = pattern;
	routes_.push_back(route);
}

std::optional<RouteMatch> Router::match(const std::string& method, const std::string& path) const
{
	for (const auto& route : routes_) {
		if (route.method != "*" && route.method != method) {
			continue;
		}

		std::regex route_regex(route.path);
		std::smatch match;

		if (std::regex_match(path, match, route_regex)) {
			RouteMatch result;
			result.handler = route.handler;

			for (size_t i = 0; i < route.param_names.size(); ++i) {
				result.path_params[route.param_names[i]] = match[i + 1];
			}
			return result;
		}
	}
	return std::nullopt;
}
