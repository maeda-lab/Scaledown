//=============================================================================
// Copyright ｩ 2020 Jul 27 Tomoki Hirayama modified by using "scale-down telxistance"
// Copyright ｩ 2018 Jan 23 Masahiro Furukawa modified.
// Copyright ｩ 2014 NaturalPoint, Inc. All Rights Reserved.
// 
// This code is heritted from SampleClient.cpp in  NatNetSDK2.0.
// 
//=============================================================================

//=============================================================================
// Copyright ｩ 2014 NaturalPoint, Inc. All Rights Reserved.
// 
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall NaturalPoint, Inc. or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//=============================================================================

//
//SampleClient.cpp
//
//This program connects to a NatNet server, receives a data stream, and writes that data stream
//to an ascii file.  The purpose is to illustrate using the NatNetClient class.
//
//Usage [optional]:
//
//    SampleClient [ServerIP] [LocalIP] [OutputFilename]
//
//    [ServerIP]	IP address of the server (e.g. 192.168.0.107) ( defaults to local machine)
//    [OutputFilename]	Name of points file (pts) to write out.  defaults to Client-output.pts
//


//headerファイルのinclude

#include <math.h>
#include <string>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Jan 23, 2018 
// Masahiro Furukawa
//#include "fanuc_sender.h"
// global instance
//FANUC_SENDER fanuc_sender;

// jul 28 2020
//Tomoki Hirayama
#include"tel_kin.h"
#include"mbed_Serial.h"



double T[4][4];


#ifdef _WIN32
#   include <conio.h>
#else
#   include <unistd.h>
#   include <termios.h>
#endif

//vectorクラスのinclude
#include <vector>

#include <NatNetTypes.h>
#include <NatNetCAPI.h>
#include <NatNetClient.h>

#ifndef _WIN32
char getch();
#endif
//=====================各種関数の定義=========================

//header情報を書き込む
void _WriteHeader(FILE* fp, sDataDescriptions* pBodyDefs);
//Frame情報を書き込む
void _WriteFrame(FILE* fp, sFrameOfMocapData* data);
//Footer情報を書き込む
void _WriteFooter(FILE* fp); 

///わからん
void NATNET_CALLCONV ServerDiscoveredCallback(const sNatNetDiscoveredServer* pDiscoveredServer, void* pUserContext);
//サーバーからデータを受け取る関数
void NATNET_CALLCONV DataHandler(sFrameOfMocapData* data, void* pUserData);    // receives data from the server
//NatNetのエラーメッセージを受け取る関数
void NATNET_CALLCONV MessageHandler(Verbosity msgType, const char* msg);      // receives NatNet error messages
    
//Clientとの接続，切断
void resetClient();
int ConnectClient();

//double a_mini = -120.0;
//double b_mini = -73.0;
//double c_mini = -3.0;
//double d_mini = -90.0;
//double e_mini = -100.0;
//double f_mini = -100.0;


#define a_mini  -120.0
#define a_maxi  90.0
#define b_mini  -73.0
#define b_maxi  90.0
#define c_mini  -3.0
#define c_maxi  130.0
#define d_mini  -90.0
#define d_maxi  115.0
#define e_mini  -100.0
#define e_maxi  85.0
#define f_mini  -100.0
#define f_maxi  90.0

DWORD maindwSendSize;
DWORD maindwGetSize;
unsigned char start[1];
char buffer[1];
errno_t error;
#define scale  1/4;
int timer = 0;
void send(double arg);

struct PointandPosture {
    double x;
    double y;
    double z;
    double qx;
    double qy;
    double qz;
    double qw;
};

struct PointandPosture HumanFirstPlace = { 0.0,0.0,0.0,0.0,0.0,0.0,0.0 };
struct PointandPosture ArmFirstPlace = { 0.0,0.0,0.0,0.0,0.0,0.0,0.0 };

//NatNetのデフォルトの接続タイプを定義
static const ConnectionType kDefaultConnectionType = ConnectionType_Multicast;

//Client情報を示す部分を確保
NatNetClient* g_pClient = NULL;
//OutPutファイルの名前
FILE* g_outputFile;
//ローカルアドレス，サーバーアドレスを設定する変数
std::vector< sNatNetDiscoveredServer > g_discoveredServers;
//NatNetに接続するためのパラメータ
sNatNetClientConnectParams g_connectParams;
//NatNetのマルチキャストアドレスを設定する変数
char g_discoveredMulticastGroupAddr[kNatNetIpv4AddrStrLenMax] = NATNET_DEFAULT_MULTICAST_ADDRESS;
int g_analogSamplesPerMocapFrame = 0;
sServerDescription g_serverDescription;

HANDLE mbed;


int main(int argc, char* argv[])
{
    // print version info　バージョン情報の表示
    unsigned char ver[4];
    NatNet_GetVersion(ver);
    printf("NatNet Sample Client (NatNet ver. %d.%d.%d.%d)\n", ver[0], ver[1], ver[2], ver[3]);

    // Install logging callback　
    //callbackを使う．
    //NatNet_SetLogCallbackという関数の引数にMessageHandlerという関数が使われている状態．
    NatNet_SetLogCallback(MessageHandler);

    // create NatNet client　NatNetのクライアントを作る
    g_pClient = new NatNetClient();

    // set the frame callback handler
    //NatNetのクライアント内SetFrameReceivedCallbackを用いて
    //DataHandlerをCallback
    //この関数を使って，サーバーからデータを受け取る
    //
    g_pClient->SetFrameReceivedCallback(DataHandler, g_pClient);	// this function will receive data from the server



    //===========================サーバーの検出====================================
    // If no arguments were specified on the command line,
    // attempt to discover servers on the local network.
    if (argc == 1)
    {
        // An example of synchronous server discovery.
#if 0
        const unsigned int kDiscoveryWaitTimeMillisec = 5 * 1000; // Wait 5 seconds for responses.
        const int kMaxDescriptions = 10; // Get info for, at most, the first 10 servers to respond.
        sNatNetDiscoveredServer servers[kMaxDescriptions];
        int actualNumDescriptions = kMaxDescriptions;
        NatNet_BroadcastServerDiscovery(servers, &actualNumDescriptions);

        if (actualNumDescriptions < kMaxDescriptions)
        {
            // If this happens, more servers responded than the array was able to store.
        }
#endif

        // Do asynchronous server discovery.
        //サーバー検出するよ
        printf("Looking for servers on the local network.\n");
        //検出されたサーバーに対応する数字Keyを押して接続してね
        printf("Press the number key that corresponds to any discovered server to connect to that server.\n");
        //Q押せばいつでも終了できるよ
        printf("Press Q at any time to quit.\n\n");

        NatNetDiscoveryHandle discovery;
        NatNet_CreateAsyncServerDiscovery(&discovery, ServerDiscoveredCallback);

        //while (const int c = getch())
        //接続のためのパラメータ設定
        while (const int c = _getch())
        {
            if (c >= '1' && c <= '9')
            {
                const size_t serverIndex = c - '1';

                if (serverIndex < g_discoveredServers.size())
                {
                    const sNatNetDiscoveredServer& discoveredServer = g_discoveredServers[serverIndex];

                    if (discoveredServer.serverDescription.bConnectionInfoValid)
                    {
                        // Build the connection parameters.
#ifdef _WIN32
                        _snprintf_s(
#else
                        snprintf(
#endif
                            g_discoveredMulticastGroupAddr, sizeof g_discoveredMulticastGroupAddr,
                            "%" PRIu8 ".%" PRIu8".%" PRIu8".%" PRIu8"",
                            //"%hhu.%hhu.%hhu.%hhu",
                            discoveredServer.serverDescription.ConnectionMulticastAddress[0],
                            discoveredServer.serverDescription.ConnectionMulticastAddress[1],
                            discoveredServer.serverDescription.ConnectionMulticastAddress[2],
                            discoveredServer.serverDescription.ConnectionMulticastAddress[3]
                        );

                        g_connectParams.connectionType = discoveredServer.serverDescription.ConnectionMulticast ? ConnectionType_Multicast : ConnectionType_Unicast;
                        g_connectParams.serverCommandPort = discoveredServer.serverCommandPort;
                        g_connectParams.serverDataPort = discoveredServer.serverDescription.ConnectionDataPort;
                        g_connectParams.serverAddress = discoveredServer.serverAddress;
                        g_connectParams.localAddress = discoveredServer.localAddress;
                        g_connectParams.multicastAddress = g_discoveredMulticastGroupAddr;
                    }
                    else
                    {
                        // We're missing some info because it's a legacy server.
                        // Guess the defaults and make a best effort attempt to connect.
                        g_connectParams.connectionType = kDefaultConnectionType;
                        g_connectParams.serverCommandPort = discoveredServer.serverCommandPort;
                        g_connectParams.serverDataPort = 0;
                        g_connectParams.serverAddress = discoveredServer.serverAddress;
                        g_connectParams.localAddress = discoveredServer.localAddress;
                        g_connectParams.multicastAddress = NULL;
                    }

                    break;
                }
            }
            else if (c == 'q')
            {
                return 0;
            }
        }

        NatNet_FreeAsyncServerDiscovery(discovery);
    }
    else
    {
        g_connectParams.connectionType = kDefaultConnectionType;

        if (argc >= 2)
        {
            g_connectParams.serverAddress = argv[1];
        }

        if (argc >= 3)
        {
            g_connectParams.localAddress = argv[2];
        }
    }

    //===========================接続開始=========================


    //Serial通信の準備
    mbed = serial_open();
    mbed = serial_initialaize(mbed);
    mbed = serial_Config(mbed);

    int iResult;

    // Create NatNet Client
    //===================================クライアントと接続する=============================
    iResult = ConnectClient();
    if (iResult != ErrorCode_OK)
    {
        printf("Error initializing client.  See log for details.  Exiting");
        return 1;
    }
    else
    {
        printf("Client initialized and ready.\n");
    }

    void* response;
    int nBytes;

    // send/receive test request
    //==================================ちゃんと送受信できるかのチェック================================
    printf("[SampleClient] Sending Test Request\n");
    iResult = g_pClient->SendMessageAndWait("TestRequest", &response, &nBytes);
    if (iResult == ErrorCode_OK)
    {
        printf("[SampleClient] Received: %s", (char*)response);
    }

    // Retrieve Data Descriptions from server
    //===========================サーバーからデータを受け取る============================
    printf("\n\n[SampleClient] Requesting Data Descriptions...");
    sDataDescriptions* pDataDefs = NULL;
    iResult = g_pClient->GetDataDescriptionList(&pDataDefs);
    if (iResult != ErrorCode_OK || pDataDefs == NULL)
    {
        printf("[SampleClient] Unable to retrieve Data Descriptions.");
    }
    else
    {
        printf("[SampleClient] Received %d Data Descriptions:\n", pDataDefs->nDataDescriptions);
        for (int i = 0; i < pDataDefs->nDataDescriptions; i++)
        {
            printf("Data Description # %d (type=%d)\n", i, pDataDefs->arrDataDescriptions[i].type);
            //マーカー
            if (pDataDefs->arrDataDescriptions[i].type == Descriptor_MarkerSet)
            {
                // MarkerSet
                sMarkerSetDescription* pMS = pDataDefs->arrDataDescriptions[i].Data.MarkerSetDescription;
                printf("MarkerSet Name : %s\n", pMS->szName);
                for (int i = 0; i < pMS->nMarkers; i++)
                    printf("%s\n", pMS->szMarkerNames[i]);

            }
            //剛体
            else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_RigidBody)
            {
                // RigidBody
                sRigidBodyDescription* pRB = pDataDefs->arrDataDescriptions[i].Data.RigidBodyDescription;
                printf("RigidBody Name : %s\n", pRB->szName);
                printf("RigidBody ID : %d\n", pRB->ID);
                printf("RigidBody Parent ID : %d\n", pRB->parentID);
                printf("Parent Offset : %3.2f,%3.2f,%3.2f\n", pRB->offsetx, pRB->offsety, pRB->offsetz);

                if (pRB->MarkerPositions != NULL && pRB->MarkerRequiredLabels != NULL)
                {
                    for (int markerIdx = 0; markerIdx < pRB->nMarkers; ++markerIdx)
                    {
                        const MarkerData& markerPosition = pRB->MarkerPositions[markerIdx];
                        const int markerRequiredLabel = pRB->MarkerRequiredLabels[markerIdx];

                        printf("\tMarker #%d:\n", markerIdx);
                        printf("\t\tPosition: %.2f, %.2f, %.2f\n", markerPosition[0], markerPosition[1], markerPosition[2]);

                        if (markerRequiredLabel != 0)
                        {
                            printf("\t\tRequired active label: %d\n", markerRequiredLabel);
                        }
                    }
                }
            }
            /*//骨格
            else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_Skeleton)
            {
                // Skeleton
                sSkeletonDescription* pSK = pDataDefs->arrDataDescriptions[i].Data.SkeletonDescription;
                printf("Skeleton Name : %s\n", pSK->szName);
                printf("Skeleton ID : %d\n", pSK->skeletonID);
                printf("RigidBody (Bone) Count : %d\n", pSK->nRigidBodies);
                for (int j = 0; j < pSK->nRigidBodies; j++)
                {
                    sRigidBodyDescription* pRB = &pSK->RigidBodies[j];
                    printf("  RigidBody Name : %s\n", pRB->szName);
                    printf("  RigidBody ID : %d\n", pRB->ID);
                    printf("  RigidBody Parent ID : %d\n", pRB->parentID);
                    printf("  Parent Offset : %3.2f,%3.2f,%3.2f\n", pRB->offsetx, pRB->offsety, pRB->offsetz);
                }
            }
            //フォースプレート
            else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_ForcePlate)
            {
                // Force Plate
                sForcePlateDescription* pFP = pDataDefs->arrDataDescriptions[i].Data.ForcePlateDescription;
                printf("Force Plate ID : %d\n", pFP->ID);
                printf("Force Plate Serial : %s\n", pFP->strSerialNo);
                printf("Force Plate Width : %3.2f\n", pFP->fWidth);
                printf("Force Plate Length : %3.2f\n", pFP->fLength);
                printf("Force Plate Electrical Center Offset (%3.3f, %3.3f, %3.3f)\n", pFP->fOriginX, pFP->fOriginY, pFP->fOriginZ);
                for (int iCorner = 0; iCorner < 4; iCorner++)
                    printf("Force Plate Corner %d : (%3.4f, %3.4f, %3.4f)\n", iCorner, pFP->fCorners[iCorner][0], pFP->fCorners[iCorner][1], pFP->fCorners[iCorner][2]);
                printf("Force Plate Type : %d\n", pFP->iPlateType);
                printf("Force Plate Data Type : %d\n", pFP->iChannelDataType);
                printf("Force Plate Channel Count : %d\n", pFP->nChannels);
                for (int iChannel = 0; iChannel < pFP->nChannels; iChannel++)
                    printf("\tChannel %d : %s\n", iChannel, pFP->szChannelNames[iChannel]);
            }
            //周辺機器
            else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_Device)
            {
                // Peripheral Device
                sDeviceDescription* pDevice = pDataDefs->arrDataDescriptions[i].Data.DeviceDescription;
                printf("Device Name : %s\n", pDevice->strName);
                printf("Device Serial : %s\n", pDevice->strSerialNo);
                printf("Device ID : %d\n", pDevice->ID);
                printf("Device Channel Count : %d\n", pDevice->nChannels);
                for (int iChannel = 0; iChannel < pDevice->nChannels; iChannel++)
                    printf("\tChannel %d : %s\n", iChannel, pDevice->szChannelNames[iChannel]);
            }*/
            else
            {
                printf("Unknown data type.");
                // Unknown
            }
        }
    }


    // Create data file for writing received stream into
    //===================================データの保存====================================
    const char* szFile = "Client-output.pts";
    if (argc > 3)
        szFile = argv[3];

    errno_t error_goutputFile;
    error_goutputFile = fopen_s(&g_outputFile,szFile, "w");
    if (error_goutputFile !=0)
    {
        printf("error opening output file %s.  Exiting.", szFile);
        exit(1);
    }

    if (pDataDefs)
    {
        _WriteHeader(g_outputFile, pDataDefs);
        NatNet_FreeDescriptions(pDataDefs);
        pDataDefs = NULL;
    }
    //============================================サーバーとの接続完了！！！！！===================================
    
    // Ready to receive marker stream!
    printf("\nClient is connected to server and listening for data...\n");
    int c;
    bool bExit = false;
    printf("Select Natnet mode...\n\tq:quit \tr:reset \tp:get server info\n");
    printf("\ts:start\tm:change multicast \tu:change unicast \n");
    printf("\tc:connect \td:disconnect");

    while (c = _getch())
    {
        //q:終了
        //r:クライアントのリセット
        //p:情報ゲット
        //==================================s:スタート========================================
        //m:マルチキャストに変更
        //u:ユニキャストに変更
        //c:接続
        //d:切断

        switch (c)
        {
            case 'q':
            {   bExit = true;
                break;
            }
            case 'r':
            {    resetClient();
                break;
            }
            case 'p':
            {    sServerDescription ServerDescription;
                 memset(&ServerDescription, 0, sizeof(ServerDescription));
                 g_pClient->GetServerDescription(&ServerDescription);
                 if (!ServerDescription.HostPresent)
                    {
                        printf("Unable to connect to server. Host not present. Exiting.");
                        return 1;
                    }
                break;
            }
            case 's':
            {
                printf("\n\n[SampleClient] Requesting Data Descriptions...");
                sDataDescriptions* pDataDefs = NULL;
                iResult = g_pClient->GetDataDescriptionList(&pDataDefs);
                if (iResult != ErrorCode_OK || pDataDefs == NULL)
                    {
                        printf("[SampleClient] Unable to retrieve Data Descriptions.");
                    }
                else
                {
                    printf("[SampleClient] Received %d Data Descriptions:\n", pDataDefs->nDataDescriptions);
                }
            }
            break;
            case 'm':	                        // change to multicast
            {    
                g_connectParams.connectionType = ConnectionType_Multicast;
                iResult = ConnectClient();
                if (iResult == ErrorCode_OK)
                    printf("Client connection type changed to Multicast.\n\n");
                else
                    printf("Error changing client connection type to Multicast.\n\n");
                break; 
            }
            case 'u':	                        // change to unicast
            {   
                g_connectParams.connectionType = ConnectionType_Unicast;
                iResult = ConnectClient();
                if (iResult == ErrorCode_OK)
                    printf("Client connection type changed to Unicast.\n\n");
                else
                    printf("Error changing client connection type to Unicast.\n\n");
                break;
            }
            case 'c':                          // connect
            {
                iResult = ConnectClient();
                break;
            }
            case 'd':                          // disconnect
            {    // note: applies to unicast connections only - indicates to Motive to stop sending packets to that client endpoint
                iResult = g_pClient->SendMessageAndWait("Disconnect", &response, &nBytes);
                if (iResult == ErrorCode_OK)
                    printf("[SampleClient] Disconnected");
                break;
            }
            default:
                break;
        }
        if (bExit)
            break;
    }

    // Done - clean up.
    if (g_pClient)
    {
        g_pClient->Disconnect();
        delete g_pClient;
        g_pClient = NULL;
    }

    if (g_outputFile)
    {
        _WriteFooter(g_outputFile);
        fclose(g_outputFile);
        g_outputFile = NULL;
    }

    return ErrorCode_OK;
}


void NATNET_CALLCONV ServerDiscoveredCallback(const sNatNetDiscoveredServer* pDiscoveredServer, void* pUserContext)
{
    char serverHotkey = '.';
    if (g_discoveredServers.size() < 9)
    {
        serverHotkey = static_cast<char>('1' + g_discoveredServers.size());
    }

    const char* warning = "";

    if (pDiscoveredServer->serverDescription.bConnectionInfoValid == false)
    {
        warning = " (WARNING: Legacy server, could not autodetect settings. Auto-connect may not work reliably.)";
    }

    printf("[%c] %s %d.%d at %s%s\n",
        serverHotkey,
        pDiscoveredServer->serverDescription.szHostApp,
        pDiscoveredServer->serverDescription.HostAppVersion[0],
        pDiscoveredServer->serverDescription.HostAppVersion[1],
        pDiscoveredServer->serverAddress,
        warning);

    g_discoveredServers.push_back(*pDiscoveredServer);
}

// Establish a NatNet Client connection
int ConnectClient()
{
    // release previous server
    g_pClient->Disconnect();

    // Init Client and connect to NatNet server
    // to use NatNet default port assignments
    int retCode = g_pClient->Connect(g_connectParams);
    // to use a different port for commands and/or data:
    if (retCode != ErrorCode_OK)
    {
        printf("Unable to connect to server.  Error code: %d. Exiting", retCode);
        return ErrorCode_Internal;
    }
    else
    {
        void* pResult;
        int nBytes = 0;
        ErrorCode ret = ErrorCode_OK;

        // print server info
        memset(&g_serverDescription, 0, sizeof(g_serverDescription));
        ret = g_pClient->GetServerDescription(&g_serverDescription);
        if (ret != ErrorCode_OK || !g_serverDescription.HostPresent)
        {
            printf("Unable to connect to server. Host not present. Exiting.");
            return 1;
        }
        printf("\n[SampleClient] Server application info:\n");
        printf("Application: %s (ver. %d.%d.%d.%d)\n", g_serverDescription.szHostApp, g_serverDescription.HostAppVersion[0],
            g_serverDescription.HostAppVersion[1], g_serverDescription.HostAppVersion[2], g_serverDescription.HostAppVersion[3]);
        printf("NatNet Version: %d.%d.%d.%d\n", g_serverDescription.NatNetVersion[0], g_serverDescription.NatNetVersion[1],
            g_serverDescription.NatNetVersion[2], g_serverDescription.NatNetVersion[3]);
        printf("Client IP:%s\n", g_connectParams.localAddress);
        printf("Server IP:%s\n", g_connectParams.serverAddress);
        printf("Server Name:%s\n", g_serverDescription.szHostComputerName);

        // get mocap frame rate
        ret = g_pClient->SendMessageAndWait("FrameRate", &pResult, &nBytes);
        if (ret == ErrorCode_OK)
        {
            float fRate = *((float*)pResult);
            printf("Mocap Framerate : %3.2f\n", fRate);
        }
        else
            printf("Error getting frame rate.\n");

        // get # of analog samples per mocap frame of data
        ret = g_pClient->SendMessageAndWait("AnalogSamplesPerMocapFrame", &pResult, &nBytes);
        if (ret == ErrorCode_OK)
        {
            g_analogSamplesPerMocapFrame = *((int*)pResult);
            printf("Analog Samples Per Mocap Frame : %d\n", g_analogSamplesPerMocapFrame);
        }
        else
            printf("Error getting Analog frame rate.\n");
    }

    return ErrorCode_OK;
}

// DataHandler receives data from the server
void NATNET_CALLCONV DataHandler(sFrameOfMocapData* data, void* pUserData)
{
    NatNetClient* pClient = (NatNetClient*)pUserData;

    // Software latency here is defined as the span of time between:
    //   a) The reception of a complete group of 2D frames from the camera system (CameraDataReceivedTimestamp)
    // and
    //   b) The time immediately prior to the NatNet frame being transmitted over the network (TransmitTimestamp)
    //
    // This figure may appear slightly higher than the "software latency" reported in the Motive user interface,
    // because it additionally includes the time spent preparing to stream the data via NatNet.
    const uint64_t softwareLatencyHostTicks = data->TransmitTimestamp - data->CameraDataReceivedTimestamp;
    const double softwareLatencyMillisec = (softwareLatencyHostTicks * 1000) / static_cast<double>(g_serverDescription.HighResClockFrequency);

    // Transit latency is defined as the span of time between Motive transmitting the frame of data, and its reception by the client (now).
    // The SecondsSinceHostTimestamp method relies on NatNetClient's internal clock synchronization with the server using Cristian's algorithm.
    const double transitLatencyMillisec = pClient->SecondsSinceHostTimestamp(data->TransmitTimestamp) * 1000.0;

    if (g_outputFile)
    {
        _WriteFrame(g_outputFile, data);
    }

    int i = 0;

    //printf("FrameID : %d\n", data->iFrame);
    //printf("Timestamp : %3.2lf\n", data->fTimestamp);
    //printf("Software latency : %.2lf milliseconds\n", softwareLatencyMillisec);

    // Only recent versions of the Motive software in combination with ethernet camera systems support system latency measurement.
    // If it's unavailable (for example, with USB camera systems, or during playback), this field will be zero.
    const bool bSystemLatencyAvailable = data->CameraMidExposureTimestamp != 0;

    if (bSystemLatencyAvailable)
    {
        // System latency here is defined as the span of time between:
        //   a) The midpoint of the camera exposure window, and therefore the average age of the photons (CameraMidExposureTimestamp)
        // and
        //   b) The time immediately prior to the NatNet frame being transmitted over the network (TransmitTimestamp)
        const uint64_t systemLatencyHostTicks = data->TransmitTimestamp - data->CameraMidExposureTimestamp;
        const double systemLatencyMillisec = (systemLatencyHostTicks * 1000) / static_cast<double>(g_serverDescription.HighResClockFrequency);

        // Client latency is defined as the sum of system latency and the transit time taken to relay the data to the NatNet client.
        // This is the all-inclusive measurement (photons to client processing).
        const double clientLatencyMillisec = pClient->SecondsSinceHostTimestamp(data->CameraMidExposureTimestamp) * 1000.0;

        // You could equivalently do the following (not accounting for time elapsed since we calculated transit latency above):
        //const double clientLatencyMillisec = systemLatencyMillisec + transitLatencyMillisec;

        printf("System latency : %.2lf milliseconds\n", systemLatencyMillisec);
        printf("Total client latency : %.2lf milliseconds (transit time +%.2lf ms)\n", clientLatencyMillisec, transitLatencyMillisec);
    }
    else
    {
        // printf("Transit latency : %.2lf milliseconds\n", transitLatencyMillisec);
    }



    //======追加部分=====
    FILE* fp;
    const char* fname = "scaledownlog.csv";
    errno_t error_fp;
    error_fp = fopen_s(&fp, fname, "w");
    fclose(fp);
    //======追加部分=====

    // FrameOfMocapData params
    bool bIsRecording = ((data->params & 0x01) != 0);
    bool bTrackedModelsChanged = ((data->params & 0x02) != 0);
    //if (bIsRecording)
    //    printf("RECORDING\n");
    //if (bTrackedModelsChanged)
        //printf("Models Changed.\n");


    // timecode - for systems with an eSync and SMPTE timecode generator - decode to values
    //int hour, minute, second, frame, subframe;
    //NatNet_DecodeTimecode(data->Timecode, data->TimecodeSubframe, &hour, &minute, &second, &frame, &subframe);
    // decode to friendly string
    //char szTimecode[128] = "";
    //NatNet_TimecodeStringify(data->Timecode, data->TimecodeSubframe, szTimecode, 128);
    //printf("Timecode : %s\n", szTimecode);

    // Rigid Bodies
    //printf("Rigid Bodies [Count=%d]\n", data->nRigidBodies);


    /*for (i = 0; i < data->nRigidBodies; i++) {
        // params
        // 0x01 : bool, rigid body was successfully tracked in this frame
    //int i = 2;
    //int i = 0;

        bool bTrackingValid = data->RigidBodies[i].params & 0x01;
        printf("Rigid Body [ID=%d  Error=%3.4f  Valid=%d]\n", data->RigidBodies[i].ID, data->RigidBodies[i].MeanError, bTrackingValid);
        printf("\tx\ty\tz\tqx\tqy\tqz\tqw\n");
        printf("\t%3.4f\t%3.4f\t%3.4f\t%3.4f\t%3.4f\t%3.4f\t%3.4f\n",
            data->RigidBodies[i].x,
            data->RigidBodies[i].y, //https://qiita.com/anqooqie/items/4b9767563baf3e65463b
            data->RigidBodies[i].z,
            data->RigidBodies[i].qx,
            data->RigidBodies[i].qy,
            data->RigidBodies[i].qz,
            data->RigidBodies[i].qw);

        //こいつら全部フロート

    }*/

    i = 0;

    timer++;
    if (timer < 100)
    {
        //初期位置の割り出し
        HumanFirstPlace.x  = HumanFirstPlace.x  + data->RigidBodies[i].x;
        HumanFirstPlace.y  = HumanFirstPlace.y  + data->RigidBodies[i].y;
        HumanFirstPlace.z  = HumanFirstPlace.z  + data->RigidBodies[i].z;
        //HumanFirstPlace.qx = HumanFirstPlace.qx + data->RigidBodies[i].qx;
        //HumanFirstPlace.qy = HumanFirstPlace.qy + data->RigidBodies[i].qy;
        //HumanFirstPlace.qz = HumanFirstPlace.qz + data->RigidBodies[i].qz;
        //HumanFirstPlace.qw = HumanFirstPlace.qw + data->RigidBodies[i].qw;
        
    }
    else
    {
        //初期位置の計算
        HumanFirstPlace.x  = HumanFirstPlace.x / timer;
        HumanFirstPlace.y  = HumanFirstPlace.y / timer;
        HumanFirstPlace.z  = HumanFirstPlace.z / timer;
        //HumanFirstPlace.qx = HumanFirstPlace.qx / timer;
        //HumanFirstPlace.qy = HumanFirstPlace.qy / timer;
        //HumanFirstPlace.qz = HumanFirstPlace.qz / timer;
        //  HumanFirstPlace.qw = HumanFirstPlace.qw / timer;







        if(timer==101) printf("人間の初期位置を以下のように定めます．\n%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", HumanFirstPlace.x, HumanFirstPlace.y, HumanFirstPlace.z, HumanFirstPlace.qx, HumanFirstPlace.qy, HumanFirstPlace.qz, HumanFirstPlace.qw);
        //=================人間の頭部座標→アームの手先座標=======================================
        struct PointandPosture arm;
        //アームの単位はmmで計算していることに注意　
        //アームには変位を入力している.

        //=======================================================================1000倍すると誤差も1000倍になることに注意====================================
        arm.x = ;// -1.0 * double(data->RigidBodies[i].z - HumanFirstPlace.z) * scale;
        arm.y = ;//       double(data->RigidBodies[i].y - HumanFirstPlace.y)* scale;
        arm.z = 10.0;//       double(data->RigidBodies[i].x - HumanFirstPlace.x)* scale;
        arm.qx = 0.0; -1.0 * double(data->RigidBodies[i].qz);
        arm.qy = 0.0;       double(data->RigidBodies[i].qy);
        arm.qz = 0.0;       double(data->RigidBodies[i].qx);
        arm.qw = 1.0;       double(data->RigidBodies[i].qw);
        

        /*arm.x = 0.1;
        arm.y = 0;
        arm.z = 0;
        arm.qx = -1.0 * double(data->RigidBodies[i].qz - HumanFirstPlace.qz);
        arm.qy = double(data->RigidBodies[i].qy - HumanFirstPlace.qy);
        arm.qz = double(data->RigidBodies[i].qx - HumanFirstPlace.qx);
        arm.qw = double(data->RigidBodies[i].qw - HumanFirstPlace.qw);
        */


        //======================手先座標系→基準座標系========================
        cal_T(arm.x, arm.y, arm.z, arm.qx, arm.qy, arm.qz, arm.qw);

        double a = cal_a();
        double b = cal_b(a);
        double c = cal_c(a);
        double d = cal_d(a, b, c);
        double e = cal_e(a, b, c, d);
        double f = cal_f(a, b, c);

        printf("a\tb\tc\td\te\tf\tの値は次のようになりました．\n");
        printf("%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf\n", a, b, c, d, e, f);
        //間に合わなかったらmbedに送る回数を減らしてみたり

        //解チェック，不当な値なら入寮しない


        //===================================a,b,c,d,e,fを合体！！！！→


        //シリアル通信を開始，角を送る
        //Sleep(5);
        buffer[0] = 's';//startコマンドを送る
        error = WriteFile(mbed, buffer, 1, &maindwSendSize, NULL);
        if (!error)
        {
            printf("Send error in s : Cannot send s \n");
        }
        else
        {
            WriteFile(mbed, start, 1, &maindwSendSize, NULL);
            printf("Start command Sending  Success!!\n");
            send(a - a_mini);
            send(b - b_mini);
            send(c - c_mini);
            send(d - d_mini);
            send(e - e_mini);
            send(f - f_mini);

        }
    }

}



// MessageHandler receives NatNet error/debug messages
void NATNET_CALLCONV MessageHandler(Verbosity msgType, const char* msg)
{
    // Optional: Filter out debug messages
    if (msgType < Verbosity_Info)
    {
        return;
    }

    printf("\n[NatNetLib]");

    switch (msgType)
    {
    case Verbosity_Debug:
        printf(" [DEBUG]");
        break;
    case Verbosity_Info:
        printf("  [INFO]");
        break;
    case Verbosity_Warning:
        printf("  [WARN]");
        break;
    case Verbosity_Error:
        printf(" [ERROR]");
        break;
    default:
        printf(" [?????]");
        break;
    }

    printf(": %s\n", msg);
}


/* File writing routines */
void _WriteHeader(FILE* fp, sDataDescriptions* pBodyDefs)
{
    int i = 0;

    if (!pBodyDefs->arrDataDescriptions[0].type == Descriptor_MarkerSet)
        return;

    sMarkerSetDescription* pMS = pBodyDefs->arrDataDescriptions[0].Data.MarkerSetDescription;

    fprintf(fp, "<MarkerSet>\n\n");
    fprintf(fp, "<Name>\n%s\n</Name>\n\n", pMS->szName);

    fprintf(fp, "<Markers>\n");
    for (i = 0; i < pMS->nMarkers; i++)
    {
        fprintf(fp, "%s\n", pMS->szMarkerNames[i]);
    }
    fprintf(fp, "</Markers>\n\n");

    fprintf(fp, "<Data>\n");
    fprintf(fp, "Frame#\t");
    for (i = 0; i < pMS->nMarkers; i++)
    {
        fprintf(fp, "M%dX\tM%dY\tM%dZ\t", i, i, i);
    }
    fprintf(fp, "\n");

}


void _WriteFrame(FILE* fp, sFrameOfMocapData* data)
{
    fprintf(fp, "%d", data->iFrame);
    for (int i = 0; i < data->MocapData->nMarkers; i++)
    {
        fprintf(fp, "\t%.5f\t%.5f\t%.5f", data->MocapData->Markers[i][0], data->MocapData->Markers[i][1], data->MocapData->Markers[i][2]);
    }
    fprintf(fp, "\n");
}


void _WriteFooter(FILE* fp)
{
    fprintf(fp, "</Data>\n\n");
    fprintf(fp, "</MarkerSet>\n");
}


void resetClient()
{
    int iSuccess;

    printf("\n\nre-setting Client\n\n.");

    iSuccess = g_pClient->Disconnect();
    if (iSuccess != 0)
        printf("error un-initting Client\n");

    iSuccess = g_pClient->Connect(g_connectParams);
    if (iSuccess != 0)
        printf("error re-initting Client\n");
}

void send(double arg)
{
    for (int cnt = 0; cnt < 4; cnt++)
    {
        //Sleep(5);
        //printf("arg%d=%f\n", cnt, arg);
        serial_Write(mbed, arg, cnt);
       // char getter[1];
        //errno_t error = ReadFile(mbed, getter, 1, &maindwGetSize, NULL);
        //if (!error)
        //{
        //    printf("Get error in send : Cannot get data \n");
        //}
        //else
        //{
        //    printf("%c\n", getter[0]);
        //}
    }

}

#ifndef _WIN32
char getch()
{
    char buf = 0;
    termios old = { 0 };

    fflush(stdout);

    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");

    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;

    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");

    if (read(0, &buf, 1) < 0)
        perror("read()");

    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;

    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");

    //printf( "%c\n", buf );

    return buf;
}
#endif
