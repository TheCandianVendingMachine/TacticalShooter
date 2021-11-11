// gameServer.hpp
// Handles game state, sending messages to clients
#pragma once
#include "time.hpp"

#include <cstdint>
#include <steam/isteamnetworkingsockets.h>
#include <robin_hood.h>
#include <atomic>
#include <thread>

class iniConfig;
class gameServer
    {
        private:
            static constexpr std::uint16_t c_defaultPort = 27020;
            static constexpr fe::time c_defaultSimulationRate = fe::seconds(1.0 / 20.0);
            static constexpr fe::time c_defaultServerPollRate = fe::seconds(1.0 / 50.0);

            HSteamListenSocket m_listenSocket = k_HSteamListenSocket_Invalid;
            HSteamNetPollGroup m_pollGroup = k_HSteamNetPollGroup_Invalid;
            ISteamNetworkingSockets *m_socketInterface = nullptr;

            robin_hood::unordered_map<HSteamNetConnection, void*> m_clients;

            std::atomic<bool> m_running = true;
            std::thread m_serverThread;
            fe::time m_simulationRate = c_defaultSimulationRate;
            fe::time m_serverPollRate = c_defaultServerPollRate;

            void handleMessage(const ISteamNetworkingMessage *message);

            void run(SteamNetworkingIPAddr address);
            void setSettings(iniConfig &serverConfig) const;

        public:
            gameServer();
            ~gameServer();

    };
