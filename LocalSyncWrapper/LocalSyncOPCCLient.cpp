

#include "LocalSyncOPCCLient.h"
#include <Winsvc.h>

#pragma comment(lib, "Ws2_32.lib")

#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib, "../OPCClientToolKit/x64/Debug/OPCClientToolKit64D.lib")
#else
#pragma comment(lib, "../OPCClientToolKit/Win32/Debug/OPCClientToolKit32D.lib")
#endif
#endif

#ifdef NDEBUG
#ifdef _WIN64
#pragma comment(lib, "../OPCClientToolKit/x64/Release/OPCClientToolKit64.lib")
#else
#pragma comment(lib, "../OPCClientToolKit/x64/Release/OPCClientToolKit32.lib")
#endif
#endif

LocalSyncOPCCLient::LocalSyncOPCCLient()
{
    p_group_ = nullptr;
    refresh_rate_ = 0;
    p_host_ = nullptr;
    p_opc_server_ = nullptr;
    is_env_ready = false;
}

LocalSyncOPCCLient::~LocalSyncOPCCLient()
{
    DisConnect();
    Stop();
}

bool LocalSyncOPCCLient::Init()
{
    COPCClient::init();
    return true;
}

bool LocalSyncOPCCLient::Stop()
{
    COPCClient::stop();
    return true;
}

bool LocalSyncOPCCLient::Connect(std::string serverName)
{
    // check if opc service runing
    if (!DetectService("OpcEnum"))
    {
        return false;
    }
    is_env_ready = true;

    // create local host

    WSADATA wsData;
    ::WSAStartup(MAKEWORD(2, 2), &wsData);
    char tmp[100];
    gethostname(tmp, 100);
    std::string hostname = tmp;
    p_host_ = COPCClient::makeHost(hostname);

    //  connect to server and get item names
    std::vector<std::string> localServerList;
    p_host_->getListOfDAServers(IID_CATID_OPCDAServer20, localServerList);
    if (localServerList.size() == 0)
    {
        return false;
    }

    p_opc_server_ = p_host_->connectDAServer(serverName);
    std::vector<std::string> item_name_vector;
    p_opc_server_->getItemNames(item_name_vector);

    // make group
    p_group_ = p_opc_server_->makeGroup("Group", true, 1000, refresh_rate_, 0.0);

    // add items
    int item_index = 0;
    for (unsigned int i = 0; i < item_name_vector.size(); i++)
    {
        if (ItemNameFilter(item_name_vector[i]))
        {
            name_item_map_[item_name_vector[i]] = p_group_->addItem(item_name_vector[i], true);
            item_index += 1;
        }
    }
    if (!IsConnected())
    {
        CleanOPCMember();
        return false;
    }
    VariantInit(&read_tmp_variant_);
    return true;
}

bool LocalSyncOPCCLient::IsConnected()
{
    if (!is_env_ready)
    {
        return false;
    }
    // check is opc server runing
    if (IsOPCRuning())
    {
        if (IsOPCConnectedPLC())
        {
            return true;
        }
    }
    return false;
}

bool LocalSyncOPCCLient::DisConnect()
{
    if (!is_env_ready)
    {
        return true;
    }

    CleanOPCMember();
    return true;
}

bool LocalSyncOPCCLient::IsOPCRuning()
{
    if (p_opc_server_ == nullptr)
    {
        return false;
    }
    ServerStatus status;
    p_opc_server_->getStatus(status);
    if (status.dwServerState != OPC_STATUS_RUNNING)
    {
        return false;
    }
    return true;
}

bool LocalSyncOPCCLient::IsOPCConnectedPLC()
{
    return true;
}

// check service status
bool LocalSyncOPCCLient::DetectService(char *ServiceName)
{
    // open manager
    SC_HANDLE hSC = ::OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
    if (hSC == NULL)
    {
        return false;
    }
    // open service
    SC_HANDLE hSvc = ::OpenService(hSC, ServiceName, SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
    if (hSvc == NULL)
    {
        return false;
    }
    // read status
    SERVICE_STATUS status;
    if (::QueryServiceStatus(hSvc, &status) != FALSE)
    {
        return true;
    }

    return false;
}

bool LocalSyncOPCCLient::ItemNameFilter(std::string item_name)
{
    return true;
}

bool LocalSyncOPCCLient::SyncWriteItem(std::string item_name, VARIANT *var)
{
    name_item_map_[item_name]->writeSync(*var);
    return true;
}

bool LocalSyncOPCCLient::SyncReadItem(std::string item_name, VARIANT *var)
{
    static OPCItemData data;
    name_item_map_[item_name]->readSync(data, OPC_DS_DEVICE);
    VariantCopy(var, &data.vDataValue);
    return true;
}

bool LocalSyncOPCCLient::ReadBool(std::string item_name)
{
    SyncReadItem(item_name, &read_tmp_variant_);
    return read_tmp_variant_.boolVal;
}

bool LocalSyncOPCCLient::WriteBool(std::string item_name, bool item_value)
{
    static VARIANT var;
    static VARTYPE a = (var.vt = VT_BOOL);
    var.boolVal = item_value;
    SyncWriteItem(item_name, &var);
    return true;
}

float LocalSyncOPCCLient::ReadFloat(std::string item_name)
{
    SyncReadItem(item_name, &read_tmp_variant_);
    return read_tmp_variant_.fltVal;
}

bool LocalSyncOPCCLient::WirteFloat(std::string item_name, float item_value)
{
    static VARIANT var;
    static VARTYPE a = (var.vt = VT_R4);
    var.fltVal = item_value;
    SyncWriteItem(item_name, &var);
    return true;
}

uint16_t LocalSyncOPCCLient::ReadUint16(std::string item_name)
{
    SyncReadItem(item_name, &read_tmp_variant_);
    return read_tmp_variant_.uiVal;
}

bool LocalSyncOPCCLient::WriteUint16(std::string item_name, uint16_t item_value)
{
    static VARIANT var;
    static VARTYPE a = (var.vt = VT_UI2);
    var.uiVal = item_value;
    SyncWriteItem(item_name, &var);
    return true;
}

bool LocalSyncOPCCLient::ReadUint16Array(std::string item_name, uint16_t *item_value_array, int array_size)
{
    VARIANT array_variant;
    VariantInit(&array_variant);
    SyncReadItem(item_name, &array_variant);

    uint16_t *buf;
    SafeArrayAccessData(array_variant.parray, (void **)&buf);
    for (int i = 0; i < array_size; ++i)
    {
        item_value_array[i] = buf[i];
    }
    SafeArrayUnaccessData(array_variant.parray);
    VariantClear(&array_variant);
    return true;
}

bool LocalSyncOPCCLient::WriteUint16Array(std::string item_name, uint16_t *item_value_array, int array_size)
{
    // create variant and safe array
    VARIANT array_variant;
    VariantInit(&array_variant);
    VARTYPE a = (array_variant.vt = VT_ARRAY | VT_UI2);
    SAFEARRAYBOUND rgsabound[1];
    rgsabound[0].cElements = array_size;
    rgsabound[0].lLbound = 0;
    array_variant.parray = SafeArrayCreate(VT_UI2, 1, rgsabound);
    // copy data
    uint16_t *buf;
    SafeArrayAccessData(array_variant.parray, (void **)&buf);
    for (int i = 0; i < array_size; ++i)
    {
        buf[i] = item_value_array[i];
    }
    SafeArrayUnaccessData(array_variant.parray);
    // write variant
    SyncWriteItem(item_name, &array_variant);
    VariantClear(&array_variant);
    return true;
}

bool LocalSyncOPCCLient::CleanOPCMember()
{
    if (IsOPCRuning()) // delete heap if connected
    {
        for (auto iter = name_item_map_.begin(); iter != name_item_map_.end(); ++iter)
        {
            delete iter->second;
        }
        name_item_map_.clear();

        if (p_group_)
        {
            delete p_group_;
            p_group_ = nullptr;
        }

        if (p_host_)
        {
            delete p_host_;
            p_host_ = nullptr;
        }

        if (p_opc_server_)
        {
            delete p_opc_server_;
            p_opc_server_ = nullptr;
        }
    }

    refresh_rate_ = 0;
    is_env_ready = false;
    return true;
}
