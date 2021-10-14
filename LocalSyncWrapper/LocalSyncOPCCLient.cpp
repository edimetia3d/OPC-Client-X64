#include <OPCHost.h>

#include "LocalSyncOPCCLient.h"

#include <Winsvc.h>

LocalSyncOPCCLient::LocalSyncOPCCLient()
{
    p_group_ = nullptr;
    refresh_rate_ = 0;
    p_host_ = nullptr;
    p_opc_server_ = nullptr;
    is_env_ready = false;

} // LocalSyncOPCCLient::LocalSyncOPCCLient

LocalSyncOPCCLient::~LocalSyncOPCCLient()
{
    DisConnect();
    Stop();

} // LocalSyncOPCCLient::~LocalSyncOPCCLient

bool LocalSyncOPCCLient::Init()
{
    COPCClient::init();
    return true;

} // LocalSyncOPCCLient::Init

bool LocalSyncOPCCLient::Stop()
{
    COPCClient::stop();
    return true;

} // LocalSyncOPCCLient::Stop

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

    char hostName[100] = "localhost";
    gethostname(hostName, 100);
    p_host_ = COPCClient::makeHost(COPCHost::S2WS(hostName));

    //  connect to server and get item names
    std::vector<CLSID> localClassIdList;
    std::vector<std::wstring> localServerList;
    p_host_->getListOfDAServers(IID_CATID_OPCDAServer20, localServerList, localClassIdList);
    if (!localServerList.size())
    {
        return false;
    }

    p_opc_server_ = p_host_->connectDAServer(COPCHost::S2WS(serverName));
    std::vector<std::wstring> item_name_vector;
    p_opc_server_->getItemNames(item_name_vector);

    // make group
    p_group_ = p_opc_server_->makeGroup(L"Group", true, 1000, refresh_rate_, 0.0);

    // add items
    int item_index = 0;
    for (unsigned int i = 0; i < item_name_vector.size(); ++i)
    {
        if (ItemNameFilter(COPCHost::WS2S(item_name_vector[i])))
        {
            name_item_map_[item_name_vector[i]] = p_group_->addItem(item_name_vector[i], true);
            ++item_index;
        }
    } // if

    if (!IsConnected())
    {
        CleanOPCMember();
        return false;
    } // if

    VariantInit(&read_tmp_variant_);
    return true;

} // LocalSyncOPCCLient::Connect

bool LocalSyncOPCCLient::IsConnected()
{
    if (!is_env_ready)
    {
        return false;
    }

    // check is opc server runing
    if (IsOPCRuning() && IsOPCConnectedPLC())
    {
        return true;
    }

    return false;

} // LocalSyncOPCCLient::IsConnected

bool LocalSyncOPCCLient::DisConnect()
{
    if (!is_env_ready)
    {
        return true;
    }

    CleanOPCMember();
    return true;

} // LocalSyncOPCCLient::DisConnect

bool LocalSyncOPCCLient::IsOPCRuning()
{
    if (!p_opc_server_)
    {
        return false;
    }

    ServerStatus status = {0};
    p_opc_server_->getStatus(status);
    if (status.dwServerState != OPC_STATUS_RUNNING)
    {
        return false;
    }

    return true;

} // LocalSyncOPCCLient::IsOPCRuning

bool LocalSyncOPCCLient::IsOPCConnectedPLC()
{
    return true;

} // LocalSyncOPCCLient::IsOPCConnectedPLC

// check service status
bool LocalSyncOPCCLient::DetectService(std::string ServiceName)
{
    // open manager
    SC_HANDLE hSC = ::OpenSCManager(nullptr, nullptr, GENERIC_EXECUTE);
    if (!hSC)
    {
        HRESULT result = ::GetLastError();
        printf("LocalSyncOPCCLient::DetectService: ::OpenSCManager() FAILED with error 0x%08x\n", result);
        return false;
    } // if

    // open service
    SC_HANDLE hSvc =
        ::OpenService(hSC, COPCHost::S2LPCTSTR(ServiceName), SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
    if (!hSvc)
    {
        HRESULT result = ::GetLastError();
        printf("LocalSyncOPCCLient::DetectService: ::OpenService() FAILED with error 0x%08x\n", result);
        return false;
    } // if

    // read status
    SERVICE_STATUS status;
    if (!::QueryServiceStatus(hSvc, &status))
    {
        HRESULT result = ::GetLastError();
        printf("LocalSyncOPCCLient::DetectService: ::QueryServiceStatus() FAILED with error 0x%08x\n", result);
        return false;
    } // if

    return true;

} // LocalSyncOPCCLient::DetectService

bool LocalSyncOPCCLient::ItemNameFilter(std::string item_name)
{
    (void)item_name;
    return true;

} // LocalSyncOPCCLient::ItemNameFilter

bool LocalSyncOPCCLient::SyncReadItem(std::string item_name, VARIANT *var)
{
    static OPCItemData data; // pretty dangerous design..
    name_item_map_[COPCHost::S2WS(item_name)]->readSync(data, OPC_DS_DEVICE);
    VariantCopy(var, &data.vDataValue);
    return true;

} // LocalSyncOPCCLient::SyncReadItem

bool LocalSyncOPCCLient::SyncWriteItem(std::string item_name, VARIANT *var)
{
    return name_item_map_[COPCHost::S2WS(item_name)]->writeSync(*var);

} // LocalSyncOPCCLient::SyncWriteItem

bool LocalSyncOPCCLient::ReadBool(std::string item_name)
{
    SyncReadItem(item_name, &read_tmp_variant_);
    return read_tmp_variant_.boolVal;

} // LocalSyncOPCCLient::ReadBool

bool LocalSyncOPCCLient::WriteBool(std::string item_name, bool item_value)
{
    static VARIANT var;
    var.vt = VT_BOOL;
    var.boolVal = item_value;
    return SyncWriteItem(item_name, &var);

} // LocalSyncOPCCLient::WriteBool

float LocalSyncOPCCLient::ReadFloat(std::string item_name)
{
    SyncReadItem(item_name, &read_tmp_variant_);
    return read_tmp_variant_.fltVal;

} // LocalSyncOPCCLient::ReadFloat

bool LocalSyncOPCCLient::WriteFloat(std::string item_name, float item_value)
{
    static VARIANT var;
    var.vt = VT_R4;
    var.fltVal = item_value;
    return SyncWriteItem(item_name, &var);

} // LocalSyncOPCCLient::WriteFloat

uint16_t LocalSyncOPCCLient::ReadUint16(std::string item_name)
{
    SyncReadItem(item_name, &read_tmp_variant_);
    return read_tmp_variant_.uiVal;

} // LocalSyncOPCCLient::ReadUint16

bool LocalSyncOPCCLient::WriteUint16(std::string item_name, uint16_t item_value)
{
    static VARIANT var;
    var.vt = VT_UI2;
    var.uiVal = item_value;
    return SyncWriteItem(item_name, &var);

} // LocalSyncOPCCLient::WriteUint16

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

} // LocalSyncOPCCLient::ReadUint16Array

bool LocalSyncOPCCLient::WriteUint16Array(std::string item_name, uint16_t *item_value_array, int array_size)
{
    // create variant and safe array
    VARIANT array_variant;
    VariantInit(&array_variant);
    array_variant.vt = VT_ARRAY | VT_UI2;
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

} // LocalSyncOPCCLient::WriteUint16Array

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
        } // if

        if (p_host_)
        {
            delete p_host_;
            p_host_ = nullptr;
        } // if

        if (p_opc_server_)
        {
            delete p_opc_server_;
            p_opc_server_ = nullptr;
        } // if
    }     // if

    refresh_rate_ = 0;
    is_env_ready = false;
    return true;

} // LocalSyncOPCCLient::CleanOPCMember
