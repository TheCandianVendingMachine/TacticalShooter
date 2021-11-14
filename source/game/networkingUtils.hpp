// networkingUtils.hpp
// Utilities that both client and server can use for whatever need
#pragma once
#include <steam/isteamnetworkingsockets.h>

namespace networkUtility
    {
        inline constexpr const char *reasonForDisconnect(int reason);
    }

inline constexpr const char *networkUtility::reasonForDisconnect(int reason)
    {
        switch (reason)
            {
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_AppException_Generic:             return "[AppException] Generic Exception";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_App_Generic:                      return "[App] Generic Reason";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Invalid:                          return "[Invalid] Invalid sentinel value";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Local_HostedServerPrimaryRelay:   return "[Local] Hosted server having trouble talking to relay";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Local_ManyRelayConnectivity:      return "[Local] No connection to any relays";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Local_NetworkConfig:              return "[Local] Cannot get SDR network config";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Local_OfflineMode:                return "[Local] In offline mode";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Local_P2P_ICE_NoPublicAddresses:  return "[Local] Cannot determine public address";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Local_Rights:                     return "[Local] No rights";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Remote_BadCert:                   return "[Remote] Bad cert";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Remote_BadCrypt:                  return "[Remote] Bad crypt";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Remote_BadProtocolVersion:        return "[Remote] Bad protocol version";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Remote_P2P_ICE_NoPublicAddresses: return "[Remote] NAT punch failure. Bad NAT/Firewall config [4007]";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Remote_Timeout:                   return "[Remote] Connection to relay timed out";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Misc_Generic:                     return "[Misc] Generic non-software bug";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Misc_InternalError:               return "[Misc] Generic software bug";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Misc_NoRelaySessionsToClient:     return "[Misc] Dedicated server has no relay sessions active";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Misc_P2P_NAT_Firewall:            return "[Misc] NAT punch failure. Bad NAT/Firewall config [5009]";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Misc_P2P_Rendezvous:              return "[Misc] P2P Rendezvous failure";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Misc_PeerSentNoConnection:        return "[Misc] Peer sent no connection";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Misc_SteamConnectivity:           return "[Misc] Steam connectivity failed";
                case ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_Misc_Timeout:                     return "[Misc] Connection to remote host timed out";
                default:
                    break;
            }
        return "Unknown cause";
    }
