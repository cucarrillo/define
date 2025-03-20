// main.c
// source code for define
// created by Cesar Carrillo
// returns a definition for a word

#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

// api url & max length of strings
#define API_URL "https://api.dictionaryapi.dev/api/v2/entries/en/\0"
#define MAX_STR 64

// color output
#define ANSI_RED	"\033[0;31m"
#define ANSI_WHITE	"\033[0;0m"
#define ANSI_BLUE	"\033[0;34m"

// std error printing
void printError(const char* string)
{
	printf(ANSI_RED);
	printf("[!] ");

	printf(ANSI_WHITE);
	printf(string);
}

// exit program when malloc fails
void memPanic()
{
	printError("no memory allocated.\n");
	
	exit(1);
}

// variables for cjson
char* 	gbuffer = NULL;
size_t 	gsize	= 0;
int 	greset 	= 0;

// cjson function to read request
size_t writeData(char* buffer, size_t size, size_t nmemb, void* userp)
{
	size_t bytes = size * nmemb;
	
	if(gsize > 0)
	{
		gbuffer = realloc(gbuffer, gsize + bytes);

		if(gbuffer == NULL) { memPanic(); };

		for(int i = 0; i < bytes; i++)
		{
			gbuffer[i + gsize] = buffer[i];
		}

		gsize += bytes;
	}
	else
	{
		gbuffer = malloc(bytes);

		if(gbuffer == NULL) { memPanic(); };

		for(int i = 0; i < bytes; i++)
		{
			gbuffer[i] = buffer[i];
		}

		gsize = bytes;
	}

	return bytes;
}

// parses the requested data by cjson
void parsebuffer()
{
	cJSON* json = cJSON_ParseWithLength(gbuffer, gsize);

	// if we have a title, then an error was thrown
	// probably that a definition was not found
	cJSON* title = cJSON_GetObjectItemCaseSensitive(json, "title");

	if(cJSON_IsString(title))
	{
		printError("word not found.\n");
		return;
	}

	cJSON* list = cJSON_GetArrayItem(json, 0);

	
	cJSON* meanings = cJSON_GetObjectItemCaseSensitive(list, "meanings");
	cJSON* sourceURLS = cJSON_GetObjectItemCaseSensitive(list, "sourceUrls");

	cJSON* url = cJSON_GetArrayItem(sourceURLS, 0);

	cJSON* meaning1 = cJSON_GetArrayItem(meanings, 0);

	cJSON* def = cJSON_GetObjectItemCaseSensitive(meaning1, "definitions");

	cJSON* mean = NULL;

	cJSON_ArrayForEach(mean, meanings)
	{
		cJSON* type = cJSON_GetObjectItemCaseSensitive(mean, "partOfSpeech");
		cJSON* defi = cJSON_GetObjectItemCaseSensitive(mean, "definitions");
		cJSON* txt = cJSON_GetArrayItem(defi, 0);
		cJSON* str = cJSON_GetObjectItemCaseSensitive(txt, "definition");

		printf(ANSI_RED);
		printf("[%s]\n", type->valuestring);
		
		printf(ANSI_WHITE);
		printf("%s\n\n", str->valuestring);
	}

	printf(ANSI_RED);
	printf("[source]\n");

	printf(ANSI_BLUE);
	printf("%s\n", url->valuestring);

	cJSON_Delete(json);
}

// entry point
int main(int argc, char** argv)
{
	if(argc < 2) 
	{ 
		printf(ANSI_RED);
		printf("[Usage]\n");

		printf(ANSI_WHITE);
		printf("define ");
		printf("<word>\n");

		return 0;
	}

	// combine API_URL and word passed
	char* request = API_URL;
	char* full = NULL;

	int rsize = 0; // request size
	int wsize = 0; // word passed size

	for(int i = 0; i < MAX_STR; i++)
	{
		if(i >= MAX_STR)
		{
			printError("exceeded word length");
			return 0;
		}

		// check for special characters
		if(wsize == 0)
		{
			int c = argv[1][i];
			int isUpperCase = ((c >= 65) && (c <= 90));
			int isLowerCase = ((c >= 97) && (c <= 122));

			if(!isUpperCase && !isLowerCase && c != '\0')
			{
				printError("special character found.\n");

				return 0;
			}
		}

		if(wsize == 0 && argv[1][i] == '\0') { wsize = i; }
		if(rsize == 0 && request[i] == '\0') { rsize = i; }
		if(rsize != 0 && wsize != 0) 		 { break; }
	}

	int newsize = (rsize * sizeof(char)) + (wsize * sizeof(char));

	full = malloc(newsize);

	if(full == NULL) { memPanic(); }

	for(int i = 0; i < rsize; i++) { full[i] = request[i]; }
	for(int i = 0; i < wsize; i++) { full[rsize + i] = argv[1][i]; }
	
	CURL* 		curl;
	CURLcode 	res;

	curl = curl_easy_init();

	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, full);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
		
		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
		{
			printError("failed to search for word.\n");

			return 0;
		}
		
		parsebuffer();

		free(gbuffer);
		gsize = 0;
		
		curl_easy_cleanup(curl);
	}

	printf(ANSI_WHITE);

	return 0;
}
