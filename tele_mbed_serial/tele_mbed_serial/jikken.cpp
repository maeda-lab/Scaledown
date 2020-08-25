#include <math.h>
#include <string>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
void NATNET_CALLCONV MessageHandler(Verbosity msgType, const char* msg);   // receives NatNet error messages

//Clientとの接続，切断
void resetClient();
int ConnectClient();

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


DWORD maindwSendSize;
DWORD maindwGetSize;
unsigned char start[1];
char buffer[1];
errno_t error;



//==============ここでスケール比を決定します==============================//
//#define scale  1/4;
double scale = 0.25;
//==============ここでスケール比を決定します==============================//



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

// Masahiro Furukawa
// Aug 24, 2020
//
// Interval Timer 
//
// reference ; https://misakichi-k.hatenablog.com/entry/2018/10/19/010134#WaitableTimer%E3%81%AB%E3%82%88%E3%82%8B%E5%9B%BA%E5%AE%9AFPS
//
////通常は有効でいい、テスト用に存在する
#define ENABLE_ERROR_CORRECTION

unsigned int	fps_;
bool			preframeIsDelay_;
HANDLE			timer_;
LONGLONG		freq_;
LONGLONG		preframeTime_;
LONGLONG		waitTime_;

LONGLONG getTime();
LONGLONG getWaitTime();
LONGLONG msTo100Ns(LONGLONG ms);
LONGLONG usTo100Ns(LONGLONG us);

LONGLONG getWaitTime();
LONGLONG getTime();
LONGLONG msTo100Ns(LONGLONG ms);
LONGLONG usTo100Ns(LONGLONG us);
void wait();



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
double markerPos[2458][3];
int t = 0;
int pos_save_counter = 0;
HANDLE mbed;
double Point[2458][3];

double master_J1 = 0.0;
double master_J2 = -120.0;
double master_J3 = 30.0;
double master_theta1 = master_J1;
double master_theta2 = -1 * master_J2 - 90.0;
double master_theta3 = -1 * master_J3 - master_J2 - 90.0;
double master_x = cal_fpx(master_J1, master_J2, master_J3);
double master_y = cal_fpy(master_J1, master_J2, master_J3);
double master_z = cal_fpz(master_J1, master_J2, master_J3);
double a, b, c;

int main(int argc, char* argv[])
{
    printf("masterx,master_y,master_z=%lf,%lf,%lf\n",master_x,master_y,master_z);
    fps_ = 65;// 60;
    timer_ = CreateWaitableTimer(NULL, FALSE, NULL);
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq_);
    preframeTime_ = getTime();
    waitTime_ = -msTo100Ns(1000) / fps_;
    SetWaitableTimer(timer_, (LARGE_INTEGER*)&waitTime_, 0, NULL, NULL, FALSE);

    LARGE_INTEGER li;
    QueryPerformanceFrequency((LARGE_INTEGER*)&li);
    auto freq = li.QuadPart;

    //get perf timer
    auto getTm = []()->LONGLONG {
        LARGE_INTEGER cnt;
        QueryPerformanceCounter(&cnt);
        return cnt.QuadPart;
    };


    FILE* fp1;
    errno_t error_fp;
    const char* input_data = "sweep.csv";
    //読み取りデータを開く
    
    
    error_fp = fopen_s(&fp1, input_data, "r");
    
    for (int i = 0; i < 2458; i++)
    {
        //配列にデータを入れる，データの単位はすべてmです．
        fscanf_s(fp1, "%lf,%lf\n", &Point[i][0], &Point[i][1]);
        Point[i][2] = 0.0;
    }
    fclose(fp1);


    //バージョン情報の表示
    unsigned char ver[4];
    NatNet_GetVersion(ver);
    printf("NatNet Sample Client (NatNet ver. %d.%d.%d.%d)\n", ver[0], ver[1], ver[2], ver[3]);
    //callbackを使う．
    NatNet_SetLogCallback(MessageHandler);
    //NatNetのクライアントを作る
    g_pClient = new NatNetClient();
    //この関数を使って，サーバーからデータを受け取る
    g_pClient->SetFrameReceivedCallback(DataHandler, g_pClient);	// this function will receive data from the server
    
                                                                    //===========================サーバーの検出====================================
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
    //===================================クライアントと接続する=============================
    int iResult;
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

    //==================================ちゃんと送受信できるかのチェック================================
    printf("[SampleClient] Sending Test Request\n");
    iResult = g_pClient->SendMessageAndWait("TestRequest", &response, &nBytes);
    if (iResult == ErrorCode_OK)
    {
        printf("[SampleClient] Received: %s", (char*)response);
    }
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
            else
            {
                printf("Unknown data type.");
                // Unknown
            }
        }
    }

    //===================================データの保存====================================
    const char* szFile = "Client-output.pts";
    if (argc > 3)
        szFile = argv[3];

    errno_t error_goutputFile;
    error_goutputFile = fopen_s(&g_outputFile, szFile, "w");
    if (error_goutputFile != 0)
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

        FILE* fp_marker;
        errno_t error_marker;
        const char* savename = "scaledownlog_.csv";
        error_marker = fopen_s(&fp_marker, savename, "w");
        for (int ii = 0; ii < 2458; ii++)
        {
            fprintf(fp_marker, "%lf,%lf,%lf\n", markerPos[ii][0], markerPos[ii][1], markerPos[ii][2]);
        }
        fclose(fp_marker);
        //fclose(fp1);
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
    const uint64_t softwareLatencyHostTicks = data->TransmitTimestamp - data->CameraDataReceivedTimestamp;
    const double softwareLatencyMillisec = (softwareLatencyHostTicks * 1000) / static_cast<double>(g_serverDescription.HighResClockFrequency);
    const double transitLatencyMillisec = pClient->SecondsSinceHostTimestamp(data->TransmitTimestamp) * 1000.0;
    if (g_outputFile)
    {
        _WriteFrame(g_outputFile, data);
    }
    int i = 0;
    const bool bSystemLatencyAvailable = data->CameraMidExposureTimestamp != 0;

    if (bSystemLatencyAvailable)
    {
        const uint64_t systemLatencyHostTicks = data->TransmitTimestamp - data->CameraMidExposureTimestamp;
        const double systemLatencyMillisec = (systemLatencyHostTicks * 1000) / static_cast<double>(g_serverDescription.HighResClockFrequency);
        const double clientLatencyMillisec = pClient->SecondsSinceHostTimestamp(data->CameraMidExposureTimestamp) * 1000.0;
        printf("System latency : %.2lf milliseconds\n", systemLatencyMillisec);
        printf("Total client latency : %.2lf milliseconds (transit time +%.2lf ms)\n", clientLatencyMillisec, transitLatencyMillisec);
    }
    else
    {
        // printf("Transit latency : %.2lf milliseconds\n", transitLatencyMillisec);
    }
    // FrameOfMocapData params
    bool bIsRecording = ((data->params & 0x01) != 0);
    bool bTrackedModelsChanged = ((data->params & 0x02) != 0);
    //アームの単位はmmで計算していることに注意　
    //アームには変位を入力している.

    //=======================================================================1000倍すると誤差も1000倍になることに注意====================================
    double x = Point[i][0] * scale + master_x ;//x座標の変位
    double y = Point[i][1] * scale + master_y;//x座標の変位
    double z = Point[i][2] * scale + master_z;//x座標の変位

    double j1 = cal_J1(x, y, z);
    double j3 = cal_J3(x, y, z, j1);    
    double j2 = cal_J2(x, y, z, j1, j3);
    //======================手先座標系→基準座標系========================
    //変位+初期位置=posi
    double theta1 = j1;
    double theta2 = -1 * j2 - 90.0;
    double theta3 = -j3 - j2 - 90.0;

    a = theta1 - master_theta1;
    b = theta2 - master_theta2;
    c = theta3 - master_theta3;
    printf("a,b,c=%lf,%lf,%lf\n", a, b, c);
    send(a + 60.0);
    send(b + 60.0);
    send(c + 60.0);
    send(0.0 + 60.0);
    send(0.0 + 60.0);
    send(0.0 + 60.0);
    //wait();
    //t++;

    // labeled markers - this includes all markers (Active, Passive, and 'unlabeled' (markers with no asset but a PointCloud ID)
    bool bOccluded;     // marker was not visible (occluded) in this frame
    bool bPCSolved;     // reported position provided by point cloud solve
    bool bModelSolved;  // reported position provided by model solve
    bool bHasModel;     // marker has an associated asset in the data stream
    bool bUnlabeled;    // marker is 'unlabeled', but has a point cloud ID that matches Motive PointCloud ID (In Motive 3D View)
    bool bActiveMarker; // marker is an actively labeled LED marker

    //printf("Markers [Count=%d]\n", data->nLabeledMarkers);
    for (i = 0; i < data->nLabeledMarkers; i++)
    {

        bOccluded = ((data->LabeledMarkers[i].params & 0x01) != 0);
        bPCSolved = ((data->LabeledMarkers[i].params & 0x02) != 0);
        bModelSolved = ((data->LabeledMarkers[i].params & 0x04) != 0);
        bHasModel = ((data->LabeledMarkers[i].params & 0x08) != 0);
        bUnlabeled = ((data->LabeledMarkers[i].params & 0x10) != 0);
        bActiveMarker = ((data->LabeledMarkers[i].params & 0x20) != 0);

        sMarker marker = data->LabeledMarkers[i];

        // Marker ID Scheme:
        // Active Markers:
        //   ID = ActiveID, correlates to RB ActiveLabels list
        // Passive Markers: 
        //   If Asset with Legacy Labels
        //      AssetID 	(Hi Word)
        //      MemberID	(Lo Word)
        //   Else
        //      PointCloud ID
        int modelID, markerID;
        NatNet_DecodeID(marker.ID, &modelID, &markerID);

        char szMarkerType[512];
        if (bActiveMarker)
            strcpy_s(szMarkerType, "Active");
        else if (bUnlabeled)
            strcpy_s(szMarkerType, "Unlabeled");
        else
            strcpy_s(szMarkerType, "Labeled");

        //printf("%s Marker [ModelID=%d, MarkerID=%d, Occluded=%d, PCSolved=%d, ModelSolved=%d] [size=%3.2f] [pos=%3.2f,%3.2f,%3.2f]\n",
           // szMarkerType, modelID, markerID, bOccluded, bPCSolved, bModelSolved, marker.size, marker.x, marker.y, marker.z);

        //ここに手先位置の座標が入る
        markerPos[pos_save_counter][0] = marker.x;
        markerPos[pos_save_counter][1] = marker.y;
        markerPos[pos_save_counter][2] = marker.z;
        pos_save_counter++;
    }
    if (pos_save_counter == 2457)
    {
        FILE* fp_marker;
        errno_t error_marker;
        const char* savename = "scaledownlog.csv";
        error_marker = fopen_s(&fp_marker, savename, "w");
        //ヘッダーの記録
        fprintf(fp_marker, "scale比,%lf\n",scale);
        fprintf(fp_marker, "カウント,目標x,目標y,目標z,計測x,計測y,計測z\n");
        for (int ii = 0; ii < 2458; ii++)
        {
            fprintf(fp_marker, "%d,%lf,%lf,%lf,%lf,%lf,%lf\n",ii,Point[ii][0], Point[ii][1], Point[ii][2], markerPos[ii][0], markerPos[ii][1], markerPos[ii][2]);
        }
        fclose(fp_marker);
        exit(0);
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
    for (int cnt = 0; cnt < 5; cnt++)
    {

        serial_Write(mbed, arg, cnt);

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



LONGLONG getWaitTime() {
    return waitTime_;
}
LONGLONG getTime() {
    LONGLONG tm;
    QueryPerformanceCounter((LARGE_INTEGER*)&tm);
    return tm;
}
LONGLONG msTo100Ns(LONGLONG ms) {
    return usTo100Ns(ms * 1000);
}
LONGLONG usTo100Ns(LONGLONG us) {
    return us * 10;
}
void wait() 
{
    //timer object wait. one frame time each if to timeout.

#ifdef ENABLE_ERROR_CORRECTION
    auto waitRet = WaitForSingleObject(timer_, (1000 + fps_ - 1) / fps_);
#else
    auto waitRet = WaitForSingleObject(timer_, INFINITE);
#endif
    auto current = getTime();
    //タイマーがタイム・アウトしている場合はwait-wait間ですでに時間が過ぎているものとして誤差調整処理の対象外にする
#ifdef ENABLE_ERROR_CORRECTION
    auto sub = (current - preframeTime_) - freq_ / fps_;
    auto delay = waitRet == WAIT_TIMEOUT;
    if (delay == false && preframeIsDelay_ == false) {
        waitTime_ += sub;
    }
    preframeIsDelay_ = delay;
#endif
    SetWaitableTimer(timer_, (LARGE_INTEGER*)&waitTime_, 0, NULL, NULL, FALSE);
    preframeTime_ = getTime();

}