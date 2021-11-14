// gameClient.hpp
// Connects to a server, gets data, interpolates, extrapolates, etc
#pragma once
#include <steam/isteamnetworkingsockets.h>

class gameClient
    {
        private:
            HSteamNetConnection m_connection = k_HSteamNetConnection_Invalid;
            ISteamNetworkingSockets *m_socketInterface = nullptr;

            bool m_connected = false;

            void onConnect();
            void handleMessage(ISteamNetworkingMessage *message);

            friend class gameServer;
            friend void connectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t *pInfo);

        public:
            gameClient();
            ~gameClient();

            void connect(const SteamNetworkingIPAddr &serverAddress);
            void disconnect();

            void pollMessages();

    };

namespace globals
    {
        extern gameClient *g_client;
    }
