#include "Client.h"

int main(int argc, char** argv)
{
    client::Client client;
    client.Init();
    client.Run();
    client.Shutdown();

    return 0;
}