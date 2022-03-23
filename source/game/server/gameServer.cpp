#include "gameServer.hpp"
#include "iniConfig.hpp"
#include "clock.hpp"
#include "game/client/gameClient.hpp"
#include "game/networkingUtils.hpp";
#include <spdlog/spdlog.h>
#include <steam/steamnetworkingsockets.h>
#include <array>
#include <assert.h>

gameServer *globals::g_server = nullptr;

static void connectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t *pInfo)
    {
        switch (pInfo->m_info.m_eState)
            {
                case k_ESteamNetworkingConnectionState_ClosedByPeer:
                case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
                    if (pInfo->m_eOldState != k_ESteamNetworkingConnectionState_Connected)
                        {
                            spdlog::warn("Connection old state was not connected {}", pInfo->m_hConn);
                        }
                    
                    spdlog::info("Client disconnected {}. Reason: {}", pInfo->m_hConn, networkUtility::reasonForDisconnect(pInfo->m_info.m_eEndReason));
                    if (globals::g_server)
                        {
                            globals::g_server->disconnectClient(pInfo->m_hConn);
                        }
                    else
                        {
                            spdlog::error("Somehow the server doesn't exist while disconnecting client");
                        }
                    break;
                case k_ESteamNetworkingConnectionState_Connecting:
                    if (globals::g_server)
                        {
                            if (globals::g_server->connectClient(pInfo->m_hConn))
                                {
                                    spdlog::info("{} has connected", pInfo->m_info.m_szConnectionDescription);
                                }
                            else
                                {
                                    spdlog::info("{} could not connect!", pInfo->m_hConn);
                                }
                        }
                    else
                        {
                            spdlog::info("Somehow the server doesn't exist while connecting client");
                        }
                    break;
                default:
                    break;
            }
    }

bool gameServer::connectClient(HSteamNetConnection connection)
    {
        assert(m_clients.find(connection) == m_clients.end());

        if (m_socketInterface->AcceptConnection(connection) != k_EResultOK)
            {
                spdlog::info("Failed to accept connection from {}", connection);
                return false;
            }

        if (!m_socketInterface->SetConnectionPollGroup(connection, m_pollGroup))
            {
                disconnectClient(connection);
                spdlog::error("Failed to set poll group for client: {}", connection);
                return false;
            }

        spdlog::info("Connecting client {}", connection);
        m_clients[connection] = nullptr;

        return true;
    }

void gameServer::disconnectClient(HSteamNetConnection connection)
    {
        if (m_clients.find(connection) == m_clients.end())
            {
                spdlog::warn("Cannot disconnect client that isn't in map!");
                return;
            }

        spdlog::info("Disconnecting client {}", connection);

        m_socketInterface->SetConnectionPollGroup(connection, 0);
        m_socketInterface->CloseConnection(connection, 0, "Disconnected", false);

        m_clients.erase(connection);
    }

void gameServer::handleMessage(const ISteamNetworkingMessage *message)
    {
        spdlog::info("Message from {}", message->GetConnection());
    }

void gameServer::run(SteamNetworkingIPAddr address)
    {
        constexpr int c_maxNumberOfIncomingMessages = 16;

        fe::clock frameClock;
        fe::time lastTime = frameClock.getTime();
        fe::time accumulator;

        while (m_running)
            {
                fe::time currentTime = frameClock.getTime();
                fe::time deltaTime = currentTime - lastTime;
                lastTime = currentTime;

                accumulator += deltaTime;

                int numberOfMessages = -1;
                std::array<ISteamNetworkingMessage*, c_maxNumberOfIncomingMessages> incomingMessages = { nullptr };
                while (numberOfMessages = m_socketInterface->ReceiveMessagesOnPollGroup(m_pollGroup, incomingMessages.data(), incomingMessages.size()), numberOfMessages != 0)
                    {
                        if (numberOfMessages < 0)
                            {
                                spdlog::error("# of messages < 0");
                                break;
                            }

                        for (int i = 0; i < numberOfMessages; i++) 
                            {
                                handleMessage(incomingMessages[i]);
                                incomingMessages[i]->Release();
                            }
                    }

                m_socketInterface->RunCallbacks();

                m_game.update();
                while (accumulator >= m_simulationRate)
                    {
                        m_game.fixedUpdate(m_simulationRate.asSeconds());
                        accumulator -= m_simulationRate;
                    }

                fe::time frameTime = frameClock.getTime() - currentTime;
                fe::time sleepTime = m_serverPollRate - frameTime;

                if (sleepTime < 0)
                    {
                        spdlog::warn("Server is running slow! dt: {}s desired poll rate: {}s", frameTime.asSeconds(), m_serverPollRate.asSeconds());
                    }
                std::this_thread::sleep_for(std::chrono::microseconds(sleepTime.asMicroseconds()));
            }
    }

void gameServer::setSettings(iniConfig &serverConfig) const
    {
        if (!serverConfig["server"].has("port")) 
            {
                serverConfig["server"]["port"] = c_defaultPort;
            }

        if (!serverConfig["server"].has("pollRate")) 
            {
                serverConfig["server"]["pollRate"] = c_defaultServerPollRate.asSeconds();
            }

        if (!serverConfig["simulation"].has("simRate"))
            {
                serverConfig["simulation"]["simRate"] = c_defaultSimulationRate.asSeconds();
            }

        serverConfig.save("server_settings.ini");
    }

bool gameServer::createLAN(SteamNetworkingIPAddr localIP, gameClient *lanClient)
    {
        SteamNetworkingIdentity serverIdentity{};
        serverIdentity.SetIPAddr(localIP);

        SteamNetworkingIdentity clientIdentity{};
        clientIdentity.SetLocalHost();

        if (!m_socketInterface->CreateSocketPair(&m_localServerConnection, &m_localClientConnection, m_isLAN, &serverIdentity, &clientIdentity))
            {
                spdlog::error("Could not create socket pair for local server");
                return false;
            }

        lanClient->m_connection = m_localClientConnection;
        lanClient->m_connected = true;

        if (!connectClient(m_localClientConnection))
            {
                return false;
            }

        lanClient->onConnect();

        return true;
    }

bool gameServer::createLocal(SteamNetworkingIPAddr localIP, gameClient *lanClient)
    {
        SteamNetworkingConfigValue_t opt{};
        opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void *)connectionStatusChangedCallback);

        m_listenSocket = m_socketInterface->CreateListenSocketIP(localIP, 1, &opt);
        if (m_listenSocket == k_HSteamListenSocket_Invalid)
            {
                spdlog::error("Failed to listen on port {} [listen socket]", localIP.m_port);
                return false;
            }
        m_socketInterface->SetConnectionName(m_listenSocket, "Local Server");

        return createLAN(localIP, lanClient);
    }

bool gameServer::createDedicated(SteamNetworkingIPAddr localIP)
    {
        SteamNetworkingConfigValue_t opt{};
        opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void *)connectionStatusChangedCallback);

        m_listenSocket = m_socketInterface->CreateListenSocketIP(localIP, 1, &opt);
        if (m_listenSocket == k_HSteamListenSocket_Invalid)
            {
                spdlog::error("Failed to listen on port {} [listen socket]", localIP.m_port);
                return false;
            }
        m_socketInterface->SetConnectionName(m_listenSocket, "Dedicated Server");

        return true;
    }

gameServer::gameServer(mode serverType, gameClient *lanClient)
    {
        iniConfig serverConfig("server_settings.ini");
        setSettings(serverConfig);      

        port = serverConfig["server"]["port"].asShort();
        m_simulationRate = fe::seconds(serverConfig["simulation"]["simRate"].asDouble());
        m_serverPollRate = fe::seconds(serverConfig["server"]["pollRate"].asDouble());

        m_socketInterface = SteamNetworkingSockets();
        SteamNetworkingIPAddr localIP{};
        localIP.Clear();
        localIP.m_port = port;

        m_pollGroup = m_socketInterface->CreatePollGroup();
        if (m_pollGroup == k_HSteamNetPollGroup_Invalid)
            {
                spdlog::error("Failed to listen on port {} [poll group]", localIP.m_port);
                return;
            }

        bool created = false;
        switch (serverType)
            {
                case gameServer::mode::LAN:
                    m_isLAN = true; // set this here because createLAN has side effects when creating local, non LAN server
                    created = createLAN(localIP, lanClient);
                    break;
                case gameServer::mode::LOCAL:
                    created = createLocal(localIP, lanClient);
                    break;
                case gameServer::mode::DEDICATED:
                    created = createDedicated(localIP);
                    break;
                default:
                    break;
            }

        if (!created)
            {
                spdlog::error("Error creating server");
                return;
            }

        spdlog::info("Server listening on port {}", localIP.m_port);

        m_serverThread = std::thread(&gameServer::run, this, localIP);
    }

gameServer::~gameServer()
    {
        spdlog::info("Shutting down server");
        m_running = false;
        m_serverThread.join();

        for (auto &client : m_clients)
            {
                m_socketInterface->CloseConnection(client.first, 0, "Server Shutdown", true);
            }
        m_clients.clear();

        m_socketInterface->CloseListenSocket(m_listenSocket);
        m_listenSocket = k_HSteamListenSocket_Invalid;

        m_socketInterface->DestroyPollGroup(m_pollGroup);
        m_pollGroup = k_HSteamNetPollGroup_Invalid;

        GameNetworkingSockets_Kill();
    }
