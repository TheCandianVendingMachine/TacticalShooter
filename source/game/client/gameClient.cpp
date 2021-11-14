#include "gameClient.hpp"
#include "game/networkingUtils.hpp"
#include <spdlog/spdlog.h>
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <array>

gameClient *globals::g_client = nullptr;

static void connectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t *pInfo)
    {
        if (globals::g_client == nullptr)
            {
                spdlog::error("Cannot change client connection state without client object");
                return;
            }

        if (pInfo->m_hConn != globals::g_client->m_connection)
            {
                spdlog::warn("Parsing connection state for different client: Me {} Them {}", globals::g_client->m_connection, pInfo->m_hConn);
                return;
            }

        switch (pInfo->m_info.m_eState)
            {
                case k_ESteamNetworkingConnectionState_ClosedByPeer:
                case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
                    spdlog::info("Disconnecting");
                    switch (pInfo->m_eOldState)
                        {
                            case k_ESteamNetworkingConnectionState_Connecting:
                                spdlog::info("Could not connect to server. Reason: {}", networkUtility::reasonForDisconnect(pInfo->m_info.m_eEndReason));
                                break;
                            case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
                                spdlog::info("Lost connection. Reason: {}", networkUtility::reasonForDisconnect(pInfo->m_info.m_eEndReason));
                                break;
                            default:
                                spdlog::info("Disconnected. Reason: {}", networkUtility::reasonForDisconnect(pInfo->m_info.m_eEndReason));
                                break;
                        }
                    globals::g_client->disconnect();
                    break;
                case k_ESteamNetworkingConnectionState_Connecting:
                    spdlog::info("Connecting");
                    break;
                case k_ESteamNetworkingConnectionState_Connected:
                    spdlog::info("Connected");
                    globals::g_client->onConnect();
                    break;
                default:
                    break;
            }
    }

void gameClient::onConnect()
    {
        m_connected = true;
    }

void gameClient::handleMessage(ISteamNetworkingMessage *message)
    {
    }

gameClient::gameClient()
    {
        m_socketInterface = SteamNetworkingSockets();
    }

gameClient::~gameClient()
    {
        disconnect();
    }

void gameClient::connect(const SteamNetworkingIPAddr &serverAddress)
    {
        char stringAddress[SteamNetworkingIPAddr::k_cchMaxString] = "";
        serverAddress.ToString(stringAddress, sizeof(stringAddress), true);

        spdlog::info("Connecting to {}", stringAddress);
        
        SteamNetworkingConfigValue_t opt{};
        opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)connectionStatusChangedCallback);

        m_connection = m_socketInterface->ConnectByIPAddress(serverAddress, 1, &opt);
        m_socketInterface->SetConnectionName(m_connection, "Client -> Server");
        if (m_connection == k_HSteamNetConnection_Invalid)
            {
                spdlog::error("Failed to create connection");
                return;
            }

        m_connected = true;
    }

void gameClient::disconnect()
    {
        if (!m_connected) { return; }
        spdlog::info("Leaving server");
        m_socketInterface->CloseConnection(m_connection, 0, "Leaving", true);
        m_connection = k_HSteamNetConnection_Invalid;
        m_connected = false;
    }

void gameClient::pollMessages()
    {
        constexpr int c_maxMessagePerLoop = 16;

        std::array<ISteamNetworkingMessage*, c_maxMessagePerLoop> messages = { nullptr };
        int numMessages = 0;
        while (numMessages = m_socketInterface->ReceiveMessagesOnConnection(m_connection, messages.data(), messages.size()), numMessages != 0)
            {
                if (numMessages < 0)
                    {
                        spdlog::error("Error checking messages");
                        return;
                    }

                for (auto &message : messages)
                    {
                        handleMessage(message);
                        message->Release();
                    }
            }

        m_socketInterface->RunCallbacks();
    }
