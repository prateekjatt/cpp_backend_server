#include "router.h"
#include "routes.h"

Router::Router(){
    logger = Logger::getInstance();
    for(const auto &route:routes){
        const auto parsedRoute = boost::urls::parse_origin_form(boost::urls::encode(route.path,boost::urls::unreserved_chars+"/"));  
        if(parsedRoute.has_error()){
            logger->log(LogType::ERROR,"Invalid Route Found: " + route.path);
            exit(EXIT_FAILURE);
        }
        const auto parsedSeg = parsedRoute.value().segments();
        std::vector<std::string> segs;
        std::copy(parsedSeg.begin(),parsedSeg.end(),std::back_inserter(segs));
        segementedRoutes.push_back({segs,route.methodHandlers});
    }
}

boost::asio::awaitable<boost::beast::http::message_generator> Router::handleRequest(HttpRequest &&request){
    HttpResponse response;
    response.version(request.version());
    response.keep_alive(request.keep_alive());

    try{
        auto parsed_url = boost::urls::parse_origin_form(request.target());
        if(parsed_url.has_error()){
            response.result(boost::beast::http::status::bad_request);
            response.set(boost::beast::http::field::content_type,"application/json");
            
            boost::json::value res = boost::json::object();
            res.as_object()["status"] = "error";
            res.as_object()["message"] = "Bad Request!";
            response.body() = boost::json::serialize(res);
            response.prepare_payload();
            co_return response;
        }
        
        std::vector<std::pair<std::string,std::string>> pathParams;
        RouteHandler handler = nullptr;
        bool anyRouteMatched = findRouteByPath(parsed_url.value().segments(),request.method(),pathParams,handler);
    
        if(handler != nullptr) {
            RequestParams requestParams;
            requestParams["pathParams"] = {};
            requestParams["queryParams"] = {};
            std::for_each(pathParams.begin(),pathParams.end(),[&requestParams](const std::pair<std::string,std::string> &a){
                requestParams["pathParams"][a.first] = a.second;
            });
            
            const auto queryParams = parsed_url.value().params(); 
            std::for_each(queryParams.begin(),queryParams.end(),[&requestParams](const boost::urls::param_view &a){
                requestParams["queryParams"][a.key] = a.value;
            });
    
            co_await handler(std::move(request),response,requestParams);
        } else if(anyRouteMatched){
            response.result(boost::beast::http::status::method_not_allowed);
            response.set(boost::beast::http::field::content_type,"application/json");
            
            boost::json::value res = boost::json::object();
            res.as_object()["status"] = "error";
            res.as_object()["message"] = "Method Not Allowed!";
            response.body() = boost::json::serialize(res);
        } else {
            response.result(boost::beast::http::status::not_found);
            response.set(boost::beast::http::field::content_type,"application/json");
            
            boost::json::value res = boost::json::object();
            res.as_object()["status"] = "error";
            res.as_object()["message"] = "Not Found!";
            response.body() = boost::json::serialize(res);
        } 
        response.prepare_payload();
    } catch(std::exception &e){
        logger->log(LogType::ERROR,std::string("Internal Server Error: ") + e.what());
        
        response.result(boost::beast::http::status::internal_server_error);
        response.set(boost::beast::http::field::content_type,"application/json");
        
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Internal Server Error!";
        response.body() = boost::json::serialize(res);
        response.prepare_payload();    
    } catch(...){
        logger->log(LogType::ERROR,std::string("Internal Server Error: "));
        
        response.result(boost::beast::http::status::internal_server_error);
        response.set(boost::beast::http::field::content_type,"application/json");
        
        boost::json::value res = boost::json::object();
        res.as_object()["status"] = "error";
        res.as_object()["message"] = "Internal Server Error!";
        response.body() = boost::json::serialize(res);
        response.prepare_payload();    
    }

    logger->log(LogType::INFO, std::string(request.method_string()) + " " + std::string(request.target()) + 
    " HTTP/" + std::to_string(request.version()/10) + "." + std::to_string(request.version()%10) + " " + 
    std::to_string(response.result_int()) + " " + std::string(response.reason()));
    co_return response;
}

bool Router::findRouteByPath(const boost::urls::segments_view &path,const boost::beast::http::verb &method,std::vector<std::pair<std::string,std::string>> &pathParams,RouteHandler &handler) {
    bool anyRouteMatched = false;
    for(const auto &route: segementedRoutes){
        if(path.size() != route.path.size()) continue;
        pathParams.clear();

        bool matched = true;
        auto pathIt = path.begin();
        std::for_each(route.path.begin(),route.path.end(),[&](const std::string &seg){
            if(seg[0] != '{' && seg != (*pathIt)){
                matched = false; 
                return;
            }
            if(seg[0] == '{'){
                pathParams.push_back(std::make_pair(seg.substr(1,seg.size()-2),(*pathIt)));
            }
        });
        
        if(matched){
            anyRouteMatched = true;
            
            if(route.methodHandlers.contains(method)){
                handler = route.methodHandlers.at(method);
                break;
            }
        }
    }

    return anyRouteMatched;
}