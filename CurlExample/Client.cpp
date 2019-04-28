#include "Client.h"
#include <sstream>
#include <iostream>
#include <fstream>

namespace client
{
namespace
{
    static bool CURL_INITED = false;

    size_t WriteFunction(void* ptr, size_t size, size_t nmemb, std::string* data)
    {
        data->append(reinterpret_cast<char*>(ptr), size * nmemb);
        return size * nmemb;
    }

    std::string CurlGet(CURL* curl_handle, const std::string& server)
    {
        curl_easy_setopt(curl_handle, CURLOPT_URL, server.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L); // Ignore SSL certificates verification
        //        curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, true); // Enable if you want to debug what CURL is doing

        std::string response_string;
        std::string header_string;
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteFunction);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &header_string);

        auto res = curl_easy_perform(curl_handle);
        if (res != CURLE_OK)
        {
            std::stringstream ss;
            ss << "Could not perform GET. Error: " << res << " (" << curl_easy_strerror(res) << ")" << std::endl;
            std::string error = ss.str();
            throw ClientException("error");
        }
        
        return response_string;
    }
} // anonymous namespace

    
Client::~Client()=default;

void Client::Init()
{
    if (!CURL_INITED)
    {
        if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK)
            throw ClientException{ "Could not init CURL" };
        CURL_INITED = true;
    }

    curl_handle = curl_easy_init();
    if (!curl_handle)
        throw ClientException{ "Could not init CURL" };
}

void Client::Run()
{
    for(;;)
    {
        std::cout << "> ";
        std::string input;
        std::cin >> input;
        if(input == "g" || input == "get")
        {
            std::string url;
            std::cin >> url;
            try
            {
                std::string response = CurlGet(curl_handle, url);
                std::cout << "Successfully performed GET on " << url << std::endl;
                std::cout << "Response was:" << std::endl;
                std::cout << response << std::endl;
            }
            catch(const ClientException& exc)
            {
                std::cout << "An exception occured: " << exc.what() << std::endl;
            }
        }
        else if(input == "gf" || input == "getfile")
        {
            std::string url;
            std::string filename;
            std::cin >> url;
            std::cin >> filename;
            try
            {
                std::string response = CurlGet(curl_handle, url);
                std::cout << "Successfully performed GET on " << url << std::endl;
                std::ofstream file{ filename };
                if(file.is_open())
                {
                    file << response;
                    file.close();
                }
            }
            catch(const ClientException& exc)
            {
                std::cout << "An exception occured: " << exc.what() << std::endl;
            }
        }
        else if(input == "h" || input == "help")
        {
            std::cout << "Welcome to CURL example client!" << std::endl;
            std::cout << "Commands:" << std::endl;
            std::cout << "'g <url>' or 'get <url>' - perform GET on url and show response" << std::endl;
            std::cout << "'gf <url> <file>' or 'getfile <url> <file>' - perform GET on url and store response in file" << std::endl;
            std::cout << "'q' or 'quit' - quit client" << std::endl;
            std::cout << "'h' or 'help' - show this message" << std::endl;
        }
        else if(input == "q" || input == "quit")
        {
            std::cout << "Quitting..." << std::endl;
            break;
        }
        else
        {
            std::cout << "Unrecognized command. Type 'help' for list of the commands." << std::endl;
        }
    }
}

void Client::Shutdown()
{
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
}

const char* ClientException::what() const noexcept
{
    return cause.c_str();
}
} // namespace client