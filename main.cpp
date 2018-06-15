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
    Discord_Initialize("457160064134938624", &handlers, 1, NULL);
}

int main(int args_n, char** args) {
  std::string email;

  if (args_n < 2) {
    std::cout << "Please provide your email as an argument" << std::endl;
    return -1;
  } else {
    email = args[1];
  }


  discordInit();

  const std::time_t start_time = std::time(nullptr);

  while(true){

    cpr::Response response;

    do {
      response = cpr::Get(cpr::Url{"https://rpc.jellywx.co.uk/"}, cpr::Header{{"email", email}});
    } while(response.text[0] != '{');

    auto json = nlohmann::json::parse(response.text);
    std::cout << json.dump(4) << std::endl;

    std::string state = json["state"];

    DiscordRichPresence drpc;
    memset(&drpc, 0, sizeof(drpc)); // sets the memory block to all zeros

    drpc.details = "Browsing the web";
    drpc.state = state.c_str();
    drpc.largeImageKey = "main";

    drpc.startTimestamp = start_time;

    Discord_UpdatePresence(&drpc);

    std::this_thread::sleep_for(std::chrono::milliseconds(15000));

  }
}
