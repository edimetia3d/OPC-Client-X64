/*
OPCClientToolKit
Copyright (C) 2006 Mark C. Beharrell

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.
*/

#include <fstream>
#include <iostream>
#include <string>
#include <sys/timeb.h>
#include <sys/types.h>

#include "OPCClient.h"
#include "OPCGroup.h"
#include "OPCHost.h"
#include "OPCItem.h"
#include "OPCServer.h"
#include "opcda.h"

using namespace std;

class CTransComplete : public ITransactionComplete
{
    unsigned completeCount;

  public:
    CTransComplete() : completeCount(0)
    {
    }

    void complete(CTransaction &transaction)
    {
        printf("-");
        ++completeCount;
        unsigned nbrErrors = 0;
        POSITION pos = transaction.getItemDataMap().GetStartPosition();
        while (pos)
        {
            OPCItemData *data = transaction.getItemDataMap().GetNextValue(pos);
            if (!data || FAILED(data->wQuality))
            {
                ++nbrErrors;
            }
        } // while

        printf("refresh %d completed with %d errors\n", completeCount, nbrErrors);
    } // complete

    const unsigned getNumberOfCompletes() const
    {
        return completeCount;
    }

}; // CTransComplete

class CMyCallback : public IAsyncDataCallback
{
  public:
    void OnDataChange(COPCGroup &group, COPCItemDataMap &changes)
    {
        printf("group '%ws', item changes:\n", group.getName().c_str());
        POSITION pos = changes.GetStartPosition();
        while (pos)
        {
            OPCItemData *data = changes.GetNextValue(pos);
            printf("-----> %ws\n", data->item()->getName().c_str());
        } // while
    }     // OnDataChange

}; // CMyCallback

void recordNameSpace(COPCServer &opcServer, const char *fileName)
{
    std::vector<std::wstring> opcItemNames;
    opcServer.getItemNames(opcItemNames);
    printf("namespace holds %d items\n", static_cast<int>(opcItemNames.size()));
    Sleep(1000);

    fstream itemListFile(fileName, ios::out);

    for (unsigned i = 0; i < opcItemNames.size(); ++i)
    {
        itemListFile << COPCHost::WS2S(opcItemNames[i]) << "\n";
    }

    itemListFile.close();

    printf("wrote item names to %s\n", fileName);
    Sleep(1000);

} // recordNameSpace

void runRefreshTest(COPCServer &opcServer, const char *fileName, unsigned nbrRefreshs)
{
    std::vector<std::wstring> itemNames;
    fstream itemListFile(fileName, ios::in);

    while (!itemListFile.eof())
    {
        char str[256] = {0};
        itemListFile.getline(str, 255);
        if (strlen(str))
        {
            itemNames.push_back(COPCHost::S2WS(str));
        }
    } // while

    itemListFile.close();

    unsigned long revisedRefreshRate;
    COPCGroup *group = opcServer.makeGroup(L"testGroup", true, 100000, revisedRefreshRate, 0.0);
    printf("made OPC group\n");
    Sleep(1000);

    std::vector<COPCItem *> itemsCreated;
    std::vector<HRESULT> errors;
    group->addItems(itemNames, itemsCreated, errors, true);

    printf("added %d items to the OPC group\n", static_cast<int>(itemsCreated.size()));
    Sleep(1000);

    CMyCallback usrCallBack;
    group->enableAsync(&usrCallBack);

    printf("about to start refresh test\n");
    Sleep(1000);

    CTransComplete complete;
    timeb startTime;
    ftime(&startTime);
    CTransaction *t = group->refresh(OPC_DS_DEVICE, &complete);
    ATL::CAutoPtrArray<CTransaction> transactions;
    CAutoPtr<CTransaction> trans;
    for (unsigned i = 0; i < nbrRefreshs; ++i)
    {
        trans.Attach(group->refresh(OPC_DS_DEVICE, &complete));
        transactions.Add(trans);
    } // for

    while (complete.getNumberOfCompletes() != nbrRefreshs)
    {
        MSG msg;
        while (PeekMessage(&msg, nullptr, NULL, NULL, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } // while
        Sleep(1);
    } // while

    timeb endTime;
    ftime(&endTime);

    int processTime_mS =
        static_cast<int>(((endTime.time - startTime.time) * 1000) + (endTime.millitm - startTime.millitm));
    printf("It took %d mSec to make %d refreshes (%f refreshs per second)\n", processTime_mS, nbrRefreshs,
           ((float)processTime_mS) / ((float)nbrRefreshs * 1000.0f));
    printf("Each refresh represents a hardware read of %d items\n", static_cast<int>(itemsCreated.size()));
    delete t;

    Sleep(1000);

} // runRefreshTest

int main(int argc, char *argv[])
{
    if ((argc != 5) && (argc != 4))
    {
        cout << "usage: \n To measure the performance of refreshs for a given OPC server, use \nOPCPerformance.exe "
                "<OPCItemList> <Host> <OPCServer> <nbrRefreshs> or "
             << endl;
        cout << "To store the namespace in the supplied file use\nOPCPerformance.exe <OPCItemList> <Host> <OPCServer>"
             << endl;
        return 1;
    } // if

    bool saveNameSpace = (argc == 4);

    COPCClient::init();
    COPCHost *host = COPCClient::makeHost(COPCHost::LPCSTR2WS(argv[2]));
    COPCServer *opcServer = host->connectDAServer(COPCHost::LPCSTR2WS(argv[3]));

    printf("connected to %s\n", argv[3]);
    Sleep(1000);

    if (saveNameSpace)
    {
        recordNameSpace(*opcServer, argv[1]);
    }
    else
    {
        runRefreshTest(*opcServer, argv[1], atoi(argv[4]));
    }

    return 0;
}
