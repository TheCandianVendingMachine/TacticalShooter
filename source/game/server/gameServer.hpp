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
class gameClient;
class gameServer
    {
        private:
            static constexpr std::uint16_t c_defaultPort = 27020;
            static constexpr fe::time c_defaultSimulationRate = fe::seconds(1.0 / 20.0);
            static constexpr fe::time c_defaultServerPollRate = fe::seconds(1.0 / 50.0);

            HSteamListenSocket m_listenSocket = k_HSteamListenSocket_Invalid;
            HSteamNetPollGroup m_pollGroup = k_HSteamNetPollGroup_Invalid;
            ISteamNetworkingSockets *m_socketInterface = nullptr;

            HSteamNetConnection m_localServerConnection = k_HSteamNetConnection_Invalid;
            HSteamNetConnection m_localClientConnection = k_HSteamNetConnection_Invalid;

            bool m_isLAN = false;

            robin_hood::unordered_map<HSteamNetConnection, void*> m_clients;

            std::atomic<bool> m_running = true;
            std::thread m_serverThread;
            fe::time m_simulationRate = c_defaultSimulationRate;
            fe::time m_serverPollRate = c_defaultServerPollRate;

            bool connectClient(HSteamNetConnection connection);
            void disconnectClient(HSteamNetConnection connection);

            void handleMessage(const ISteamNetworkingMessage *message);

            void run(SteamNetworkingIPAddr address);
            void setSettings(iniConfig &serverConfig) const;

            bool createLAN(SteamNetworkingIPAddr localIP, gameClient *lanClient);
            bool createLocal(SteamNetworkingIPAddr localIP, gameClient *lanClient);
            bool createDedicated(SteamNetworkingIPAddr localIP);

            friend void connectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t *pInfo);

        public:
            std::uint16_t port = c_defaultPort;
            const bool &isLAN = m_isLAN;

            enum class mode
                {
                    LAN,
                    LOCAL,
                    DEDICATED
                };

            gameServer(mode serverType, gameClient *lanClient);
            ~gameServer();

    };

namespace globals
    {
        extern gameServer *g_server;
    }

