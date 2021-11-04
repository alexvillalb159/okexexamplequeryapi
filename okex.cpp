//sudo apt install libcurl4 libcurl4-gnutls-dev libjsoncpp-dev
// g++ okex.cpp  -lcurl -lcurl-gnutls -ljsoncpp -o okex

// Documentación: 
// https://www.okex.com/docs/en/#README
// https://www.okex.com/docs-v5/en/#market-maker-program
// https://curl.se/libcurl/c/libcurl.html
// https://github.com/open-source-parsers/jsoncpp/blob/master/example/readFromString/readFromString.cpp


#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include <string>
#include <sstream>
#include <stdexcept>
#include <time.h>
#include <iomanip>
#include <jsoncpp/json/json.h>
#include <stdlib.h>

using namespace std;



struct memory {
	char *response;
	size_t size;
};
 
static size_t cb(void *data, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct memory *mem = (struct memory *)userp;

	char *ptr = (char *) realloc(mem->response, mem->size + realsize + 1);
	if(ptr == NULL)
	return 0;  /* out of memory! */

	mem->response = ptr;
	memcpy(&(mem->response[mem->size]), data, realsize);
	mem->size += realsize;
	mem->response[mem->size] = 0;

	return realsize;
}
 


int main()
{
	CURL *curl;
	CURLcode res;

 
	struct curl_slist *headerlist=NULL;
	headerlist = curl_slist_append( headerlist, "Accept: application/json");

	curl = curl_easy_init();
	if(curl) {
		struct memory chunk = {0};

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_URL, "https://www.okex.com/api/v5/market/ticker?instId=BTC-USDT");

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
		/* we pass our 'chunk' struct to the callback function */

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
		res = curl_easy_perform(curl);

		if(res != CURLE_OK) {
		    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		} else {
			long response_code;
    			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
			if(response_code == 200) {
				Json::CharReaderBuilder  builder;
				Json::Value value;
				JSONCPP_STRING err;
	  			Json::Value root;
				const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

				if (!reader->parse(chunk.response, chunk.response + chunk.size, &root,
					       &err)) {
					std::cout << "error" << std::endl;
					return EXIT_FAILURE;
				}

				cout << "Value: " << root["data"][0]["last"] << endl;
			} else {
				//cout << "ERROR: " << chunk.response << endl;
				cout << "\x1b[1;31mERROR\x1b[0m" <<  endl;

			}
		}
	}



	return 0;
}
