#include <curl.h>
#include "FromXMLToGoogleMapHTTPRequest.h"

// Constructeur
FromXMLToGoogleMapHTTPRequest::FromXMLToGoogleMapHTTPRequest(std::string id) : LwSaxParser()
{
    
}

FromXMLToGoogleMapHTTPRequest::~FromXMLToGoogleMapHTTPRequest() {
    
}

std::string FromXMLToGoogleMapHTTPRequest::getGoogleMapHttpRequest() {
    
   return "http://maps.googleapis.com/maps/api/distancematrix/xml?sensor=false&mode=driving&units=metric&origins=Grenoble+38031+46+avenue+Félix+Viallet|La Tronche+38700+Rond-Point+de+la+Croix+de+Vie|Grenoble+38000+rue+Casimir+Brenier|Grenoble+38042+25+rue+des+Martyrs&destinations=Grenoble+38031+46+avenue+Félix+Viallet|La Tronche+38700+Rond-Point+de+la+Croix+de+Vie|Grenoble+38000+rue+Casimir+Brenier|Grenoble+38042+25+rue+des+Martyrs";
}

std::string FromXMLToGoogleMapHTTPRequest::getGoogleMapHttpRequest_V2() {
    
  // Repris par Laurence :
//    return "origins=Grenoble+38031+46+avenue+Félix+Viallet|La Tronche+38700+Rond-Point+de+la+Croix+de+Vie|Grenoble+38000+rue+Casimir+Brenier|Grenoble+38042+25+rue+des+Martyrs&destinations=Grenoble+38031+46+avenue+Félix+Viallet|La Tronche+38700+Rond-Point+de+la+Croix+de+Vie|Grenoble+38000+rue+Casimir+Brenier|Grenoble+38042+25+rue+des+Martyrs";
//    
    // V3
    return "origins=" + std::string(curl_easy_escape(NULL,"Grenoble+38031+46+avenue+Félix+Viallet|La Tronche+38700+Rond-Point+de+la+Croix+de+Vie|Grenoble+38000+rue+Casimir+Brenier|Grenoble+38042+25+rue+des+Martyrs",0)) + "&destinations=" + std::string(curl_easy_escape(NULL,"Grenoble+38031+46+avenue+Félix+Viallet|La Tronche+38700+Rond-Point+de+la+Croix+de+Vie|Grenoble+38000+rue+Casimir+Brenier|Grenoble+38042+25+rue+des+Martyrs",0));
}
