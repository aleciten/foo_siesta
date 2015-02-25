#include <memory>
#include <functional>
#include <Poco\URI.h>
#include <Poco\Net\HTTPRequestHandlerFactory.h>
#include <Poco\Net\HTTPServerRequest.h>

class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
	struct KeyInUseException : public std::exception {};

	RequestHandlerFactory(std::string baseUrl);
	RequestHandlerFactory();

	Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);

private:
	template<typename T>
	void _registerHandler(std::string key) {
		if (_handlerRegistry.find(key) != _handlerRegistry.end()) throw KeyInUseException();
		_handlerRegistry[key] = _ctorFunc([](std::vector<std::string> relativeSegments) -> Poco::Net::HTTPRequestHandler* { return new T(relativeSegments); });
	}
	Poco::Net::HTTPRequestHandler* _create(std::string key, std::vector<std::string> relativeSegments);
	bool _matchesBase(Poco::URI &uriString);
	
	std::vector<std::string> _basePathSegments;
	typedef std::function<Poco::Net::HTTPRequestHandler*(std::vector<std::string>)> _ctorFunc;
	std::map<std::string, _ctorFunc> _handlerRegistry;
};