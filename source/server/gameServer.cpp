#include "gameServer.hpp"
#include "iniConfig.hpp"
#include "clock.hpp"
#include <spdlog/spdlog.h>
#include <steam/steamnetworkingsockets.h>
#include <array>

void gameServer::handleMessage(const ISteamNetworkingMessage *message)
    {
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

                while (accumulator >= m_simulationRate)
                    {
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

gameServer::gameServer()
    {
        iniConfig serverConfig("server_settings.ini");
        setSettings(serverConfig);

        SteamDatagramErrMsg errorMessage;
        if (!GameNetworkingSockets_Init(nullptr, errorMessage))
            {
                spdlog::error("Cannot initialise GameNetworkingSockets: {}", errorMessage);
                return;
            }

        m_socketInterface = SteamNetworkingSockets();
        SteamNetworkingIPAddr localIP;
        localIP.Clear();
        localIP.m_port = serverConfig["server"]["port"].asShort();

        m_listenSocket = m_socketInterface->CreateListenSocketIP(localIP, 0, nullptr);
        if (m_listenSocket == k_HSteamListenSocket_Invalid)
            {
                spdlog::error("Failed to listen on port {} [listen socket]", localIP.m_port);
                return;
            }

        m_pollGroup = m_socketInterface->CreatePollGroup();
        if (m_pollGroup == k_HSteamNetPollGroup_Invalid)
            {
                spdlog::error("Failed to listen on port {} [poll group]", localIP.m_port);
                return;
            }

        spdlog::info("Server listening on port {}", localIP.m_port);

        m_simulationRate = fe::seconds(serverConfig["simulation"]["simRate"].asDouble());
        m_serverPollRate = fe::seconds(serverConfig["server"]["pollRate"].asDouble());

        m_serverThread = std::thread(&gameServer::run, this, localIP);
    }

gameServer::~gameServer()
    {
        spdlog::info("Shutting down server");
        m_running = false;
        m_serverThread.join();

        m_socketInterface->CloseListenSocket(m_listenSocket);
        m_listenSocket = k_HSteamListenSocket_Invalid;

        m_socketInterface->DestroyPollGroup(m_pollGroup);
        m_pollGroup = k_HSteamNetPollGroup_Invalid;
    }
