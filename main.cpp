#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <cstdlib>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include "discord_rpc.h"


static void handleDiscordDisconnected(int errcode, const char* message)
{
    printf("\nDiscord: disconnected (%d: %s)\n", errcode, message);
    Discord_Shutdown();
}

static void handleDiscordError(int errcode, const char* message)
{
    printf("\nDiscord: error (%d: %s)\n", errcode, message);
}

void discordInit()
{
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.disconnected = handleDiscordDisconnected;
    handlers.errored = handleDiscordError;
    Discord_Initialize("455298663598653446", &handlers, 1, NULL);
}

int main() {
  discordInit();

  const std::time_t start_time = std::time(nullptr);

  while(true){

    auto response = cpr::Get(cpr::Url{"https://jsonplaceholder.typicode.com/posts/1"});//, cpr::Header{{"email", "judewrs@gmail.com"}});
    std::cout << response.text << std::endl;
    //auto json = nlohmann::json::parse(response.text);
    //std::cout << json.dump(4) << std::endl;

    DiscordRichPresence drpc;
    memset(&drpc, 0, sizeof(drpc)); // sets the memory block to all zeros
    drpc.state = "Browsing the web";
    drpc.details = response.text.c_str();
    drpc.startTimestamp = start_time;

    Discord_UpdatePresence(&drpc);

    std::this_thread::sleep_for(std::chrono::milliseconds(15000));

  }
}
