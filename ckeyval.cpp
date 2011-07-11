#include "headers.h"

struct curl_data {
	unsigned char* data;
	int ptr;
	int len;
};

size_t curl_write_data(void* buffer, size_t size, size_t nmemb, void* userp) {
	curl_data* data = (curl_data*)userp;
	unsigned char* input = (unsigned char*)buffer;
	for(int x=0; x<nmemb; x++) {
		data->data[data->ptr] = input[x];
		data->ptr++;
	}

	return size;
}

size_t curl_write_dummy(void* buffer, size_t size, size_t nmemb, void* userp) {
	return size;
}

size_t curl_read_data(void* buffer, size_t size, size_t nmemb, void* userp) {
	curl_data* data = (curl_data*)userp;
	unsigned char* output = (unsigned char*)buffer;
	int x;
	for(x=0; x<nmemb; x++) {
		output[x] = data->data[data->ptr];
		data->ptr++;
		if(data->ptr >= data->len) break;
	}

	return size;
}

CKeyVal::CKeyVal() {
	curl_global_init(CURL_GLOBAL_WIN32);
	curl_handle = NULL;
}

CKeyVal::~CKeyVal() {

}

unsigned char* CKeyVal::getValue(char* key) {
	curl_handle = curl_easy_init();
	char* url = new char[128];
	sprintf(url, "http://api.openkeyval.org/%s", key);
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	delete [] url;

	curl_data* data = new curl_data;
	data->data = new unsigned char[128000];
	data->ptr = 0;

	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_write_data); 
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, data); 
	curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 4);
	curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 4);
	int result = curl_easy_perform(curl_handle);
	
    curl_easy_cleanup(curl_handle);

	if(data->ptr == 0) return NULL;

	data->data[data->ptr] = 0;

	char* error = new char[16];
	int x;
	for(x=2; x<7; x++) {
		error[x-2] = data->data[x];
	}
	error[x-2] = 0;
	if(!strcmp(error, "error")) return NULL;
	delete [] error;

	if(result) return data->data;
	
	return NULL;
}

bool CKeyVal::setValue(char* key, unsigned char* value, int value_size) {
	curl_handle = curl_easy_init();
	char* url = new char[128];
	sprintf(url, "http://api.openkeyval.org/%s", key);
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	delete [] url;

	curl_data* data = new curl_data;
	data->data = value;
	data->ptr = value_size;
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_write_dummy); 

	struct curl_httppost *post=NULL;
	struct curl_httppost *last=NULL;

	curl_formadd(&post, &last, CURLFORM_COPYNAME, "data", CURLFORM_PTRCONTENTS, data->data, CURLFORM_CONTENTSLENGTH, data->ptr,CURLFORM_END);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, post);
	curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 4);
	curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 4);

	int result = curl_easy_perform(curl_handle);
	//curl_slist_free_all(headers);
	curl_easy_cleanup(curl_handle);

	return result;
}