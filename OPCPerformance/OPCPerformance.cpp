
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

#include "OPCClient.h"
#include "OPCGroup.h"
#include "OPCHost.h"
#include "OPCItem.h"
#include "OPCServer.h"
#include "opcda.h"
#include "stdafx.h"
#include <fstream>
#include <sys\timeb.h>

using namespace std;

class CTransComplete : public ITransactionComplete
{
    unsigned completeCount;

  public:
    CTransComplete() : completeCount(0){};

    void complete(CTransaction &transaction)
    {
        printf("-");
        ++completeCount;
        unsigned noErrors = 0;
        POSITION pos = transaction.opcData.GetStartPosition();
        while (pos != NULL)
        {
            OPCItemData *data = transaction.opcData.GetNextValue(pos);
            if (!data || FAILED(data->wQuality))
            {
                ++noErrors;
            }
        }
        printf("Refresh %d completed with %d errors\n", completeCount, noErrors);
    }

    const unsigned getNumberOfCompletes() const
    {
        return completeCount;
    }
};

class CMyCallback : public IAsynchDataCallback
{
  public:
    void OnDataChange(COPCGroup &group, CAtlMap<COPCItem *, OPCItemData *> &changes)
    {
        printf("Group %s, item changes\n", group.getName().c_str());
        POSITION pos = changes.GetStartPosition();
        while (pos != NULL)
        {
            COPCItem *item = changes.GetNextKey(pos);
            printf("-----> %s\n", item->getName().c_str());
        }
    }
};

void recordTheNameSpace(COPCServer &opcServer, const char *fileName)
{
    std::vector<std::string> opcItemNames;
    opcServer.getItemNames(opcItemNames);
    printf("Namespace holds %d items\n", opcItemNames.size());
    Sleep(1000);

    fstream itemListFile(fileName, ios::out);
    for (unsigned i = 0; i < opcItemNames.size(); i++)
    {
        opcItemNames[i].c_str();
        itemListFile << opcItemNames[i].c_str() << "\n";
    }
    itemListFile.close();

    printf("Wrote item names to %s\n", fileName);
    Sleep(1000);
}

void runRefreshTest(COPCServer &opcServer, const char *fileName, unsigned noRefreshs)
{
    char str[256];
    fstream itemListFile(fileName, ios::in);
    std::vector<std::string> itemNames;
    while (!itemListFile.eof())
    {
        itemListFile.getline(str, 2000);
        itemNames.push_back(str);
    }
    itemListFile.close();

    unsigned long revisedRefreshRate;
    COPCGroup *group = opcServer.makeGroup("testGroup", true, 100000, revisedRefreshRate, 0.0);

    printf("Made OPC group\n");
    Sleep(1000);

    std::vector<COPCItem *> itemsCreated;
    std::vector<HRESULT> errors;
    group->addItems(itemNames, itemsCreated, errors, true);

    printf("Added %d items to the OPC group\n", itemsCreated.size());
    Sleep(1000);

    CMyCallback usrCallBack;
    group->enableAsynch(usrCallBack);

    printf("About to start refresh test\n");
    Sleep(1000);

    CTransComplete complete;
    timeb startTime;
    ftime(&startTime);
    CTransaction *t = group->refresh(OPC_DS_DEVICE, &complete);
    ATL::CAutoPtrArray<CTransaction> transactions;
    CAutoPtr<CTransaction> trans;
    for (unsigned i = 0; i < noRefreshs; i++)
    {
        trans.Attach(group->refresh(OPC_DS_DEVICE, &complete));
        transactions.Add(trans);
    }

    while (complete.getNumberOfCompletes() != noRefreshs)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(1);
    }
    timeb endTime;
    ftime(&endTime);

    unsigned long processTime_mS = ((endTime.time - startTime.time) * 1000) + (endTime.millitm - startTime.millitm);
    printf("It took %d mSec to make %d refreshes (%f refreshs per second)\n", processTime_mS, noRefreshs,
           ((float)processTime_mS) / ((float)noRefreshs * 1000.0f));
    printf("Each refresh represents a hardware read of %d items\n", itemsCreated.size());
    delete t;

    Sleep(1000);
}

int _tmain(int argc, _TCHAR *argv[])
{
    if ((argc != 5) && (argc != 4))
    {
        cout << "usage: \n To measure the performance of refreshs for a given OPC "
                "server, use \nOPCPerformance.exe <OPCItemList> <Host> <OPCServer> "
                "<noRefreshs> or "
             << endl;
        cout << "To store the namespace in the supplied file "
                "use\nOPCPerformance.exe <OPCItemList> <Host> <OPCServer>"
             << endl;
        return 1;
    }

    bool saveNameSpace = (argc == 4);

    COPCClient::init();
    COPCHost *host = COPCClient::makeHost(argv[2]);
    COPCServer *opcServer = host->connectDAServer(argv[3]);

    printf("Connected to %s\n", argv[3]);
    Sleep(1000);

    if (saveNameSpace)
    {
        recordTheNameSpace(*opcServer, argv[1]);
    }
    else
    {
        runRefreshTest(*opcServer, argv[1], atoi(argv[4]));
    }

    return 0;
}
