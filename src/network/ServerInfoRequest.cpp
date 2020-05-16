#include "ServerInfoRequest.hpp"
#include "CryptoManager.hpp"
#include "Log.h"

extern "C" {
    #include "errors.h"
}

int ServerInfoRequest::request(std::string address, PSERVER_DATA server) {
    LiInitializeServerInformation(&server->serverInfo);
    char* addr = (char *)malloc(sizeof(char) * address.length() + 1);
    addr[address.length()] = '\0';
    memcpy(addr, address.c_str(), sizeof(char) * address.length());
    server->serverInfo.address = addr;
    
    int result = NetworkClientError;
    char url[1024];
    
    bool certs_exists = CryptoManager::certs_exists();
    
    if (certs_exists) {
        snprintf(url, sizeof(url), "%s:47984/serverinfo?uniqueid=%s", server->serverInfo.address, unique_id.c_str());
    } else {
        snprintf(url, sizeof(url), "%s:47989/serverinfo", server->serverInfo.address);
    }
    
    Data data;
    if (NetworkClient::perform(url, certs_exists, &data) != NetworkClientOK) {
        return NetworkClientError;
    }
    
    char *pairedText = NULL;
    char *currentGameText = NULL;
    char *stateText = NULL;
    char *serverCodecModeSupportText = NULL;
    
    if (xml_status(data.bytes(), data.size()) == GS_ERROR)
        goto cleanup;
    
    if (xml_search(data.bytes(), data.size(), "currentgame", &currentGameText) != GS_OK) {
        goto cleanup;
    }
    
    if (xml_search(data.bytes(), data.size(), "PairStatus", &pairedText) != GS_OK)
        goto cleanup;
    
    if (xml_search(data.bytes(), data.size(), "appversion", (char**) &server->serverInfo.serverInfoAppVersion) != GS_OK)
        goto cleanup;
    
    if (xml_search(data.bytes(), data.size(), "state", &stateText) != GS_OK)
        goto cleanup;
    
    if (xml_search(data.bytes(), data.size(), "ServerCodecModeSupport", &serverCodecModeSupportText) != GS_OK)
        goto cleanup;
    
    if (xml_search(data.bytes(), data.size(), "gputype", &server->gpuType) != GS_OK)
        goto cleanup;
    
    if (xml_search(data.bytes(), data.size(), "GsVersion", &server->gsVersion) != GS_OK)
        goto cleanup;
    
    if (xml_search(data.bytes(), data.size(), "hostname", &server->hostname) != GS_OK)
        goto cleanup;
    
    if (xml_search(data.bytes(), data.size(), "GfeVersion", (char**) &server->serverInfo.serverInfoGfeVersion) != GS_OK)
        goto cleanup;
    
    if (xml_modelist(data.bytes(), data.size(), &server->modes) != GS_OK)
        goto cleanup;
    
    // These fields are present on all version of GFE that this client supports
    if (!strlen(currentGameText) || !strlen(pairedText) || !strlen(server->serverInfo.serverInfoAppVersion) || !strlen(stateText))
        goto cleanup;
    
    server->paired = pairedText != NULL && strcmp(pairedText, "1") == 0;
    server->currentGame = currentGameText == NULL ? 0 : atoi(currentGameText);
    server->supports4K = serverCodecModeSupportText != NULL;
    server->serverMajorVersion = atoi(server->serverInfo.serverInfoAppVersion);
    
    if (strstr(stateText, "_SERVER_BUSY") == NULL) {
        // After GFE 2.8, current game remains set even after streaming
        // has ended. We emulate the old behavior by forcing it to zero
        // if streaming is not active.
        server->currentGame = 0;
    }
    result = NetworkClientOK;
    
cleanup:
    if (pairedText != NULL)
        free(pairedText);
    
    if (currentGameText != NULL)
        free(currentGameText);
    
    if (serverCodecModeSupportText != NULL)
        free(serverCodecModeSupportText);
    
    if (result == GS_OK && !server->unsupported) {
        if (server->serverMajorVersion > MAX_SUPPORTED_GFE_VERSION) {
            gs_error = "Ensure you're running the latest version of Moonlight Embedded or downgrade GeForce Experience and try again";
            result = NetworkClientError;
        } else if (server->serverMajorVersion < MIN_SUPPORTED_GFE_VERSION) {
            gs_error = "Moonlight Embedded requires a newer version of GeForce Experience. Please upgrade GFE on your PC and try again.";
            result = NetworkClientError;
        }
    }
    
    return result;
}
