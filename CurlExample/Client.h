#pragma once
#include <string>
#include <exception>
#include <map>
#include <vector>
#include <curl/curl.h>

namespace client
 {
    class ClientException : public std::exception
    {
    public:
        ClientException(const char* cause) : cause(cause)
        {
        }

        ~ClientException() override = default;

        const char* what() const noexcept override;
    private:
        std::string cause;
    };

    class Client
    {
    public:
        Client() = default;
        ~Client();

        void Init();
        void Run();
        void Shutdown();
    private:
        CURL* curl_handle;
    };
 } // namespace client
