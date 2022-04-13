/* 
   Copyright (C) 2004-2006 Teksoft SRL Romania
   
   THIS SOURCE CODE IS PROPERTY OF Teksoft SRL Romania.
   http://www.teksoftco.com
   All rights reserved.

Description:
============
RadioDevices sample offers the possibility of controlling the Radio Devices (Phone,Bluetooth,WiFi),
on devices running Windows CE for Pocket PC 4.0 and Windows CE for Pocket PC 5.0

License:
========
Redistribution and use in source and binary forms, with or without modification, are permitted 
provided that the following conditions are met (1,2,3): 
1) Redistribution of source code is not allowed in any way, nor is allowed selling the source code 
and this library to other parties.
2) Redistribution in binary form must reproduce the above copyright notice, this list of conditions 
and the following disclaimer in the documentation and/or other materials provided with the 
distribution. 
3) Neither the name of Teksoft SRL Romania or the names of contributors may be used to endorse 
or promote products derived from this software without specific prior written permission. 

This software is provided "AS IS," without a warranty of any kind. ALL EXPRESS OR IMPLIED 
CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED. Teksoft SRL Romania 
("Teksoft") AND ITS LICENSORS SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT 
OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES. IN NO EVENT WILL Teksoft OR 
ITS LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR DIRECT, INDIRECT, SPECIAL, 
CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF 
LIABILITY, ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF Teksoft HAS BEEN 
ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 
*/

#include "stdafx.h"
#include "WirelessDevices.h"


_GetWirelessDevices		pGetWirelessDevices = NULL;
_ChangeRadioState		pChangeRadioState = NULL;
_FreeDeviceList			pFreeDeviceList = NULL;

HINSTANCE	g_DllWrlspwr;

BOOL InitDLL()
{
	g_DllWrlspwr = LoadLibrary(TEXT("ossvcs.dll"));
	if (g_DllWrlspwr == NULL)
		return FALSE;
	pGetWirelessDevices   = (_GetWirelessDevices)GetProcAddress(g_DllWrlspwr,MAKEINTRESOURCE(GetWirelessDevice_ORDINAL));
	if (pGetWirelessDevices == NULL)
		return FALSE;
	
	pChangeRadioState   = (_ChangeRadioState)GetProcAddress(g_DllWrlspwr,MAKEINTRESOURCE(ChangeRadioState_ORDINAL));
	if (pChangeRadioState == NULL)
		return FALSE;
	
	pFreeDeviceList	   = (_FreeDeviceList)GetProcAddress(g_DllWrlspwr,MAKEINTRESOURCE(FreeDeviceList_ORDINAL));
	if (pFreeDeviceList == NULL)
		return FALSE;
	return TRUE;
}

BOOL DeinitDLL()
{
	return FreeLibrary(g_DllWrlspwr);
}


//
//  FUNCTION: SetWDevState(DWORD dwDevice, DWORD dwState)
//
//  PURPOSE:  set the status of the desired wireless device
//
//  USAGE:	
//			
//			SetWDevState( RADIODEVICES_BLUETOOTH, 1); -start bluetooth
//			
//			SetWDevState( RADIODEVICES_PHONE, 1);	  -start phone
//			
//			SetWDevState( RADIODEVICES_MANAGED, 1);	  -start WIFI
//
//
DWORD SetWDevState(DWORD dwDevice, DWORD dwState)
{
	RDD * pDevice = NULL;
    RDD * pTD;
    HRESULT hr;
	DWORD retval = 0;

//	InitDLL();
    hr = pGetWirelessDevices(&pDevice, 0);
	if(hr == E_FAIL) return -1; //Unspecified failure.

	if(hr == E_OUTOFMEMORY) return -2;
	//if(hr != S_OK) return -3;
    
    if (pDevice)
    {
        pTD = pDevice;

        // loop through the linked list of devices
        while (pTD)
        {
          if  (pTD->DeviceType == dwDevice)
          {
              hr = pChangeRadioState(pTD, dwState, RADIODEVICES_PRE_SAVE);
			  retval = 0;
			  break;
          }
          
            pTD = pTD->pNext;
            
        }
        // Free the list of devices retrieved with    
        // GetWirelessDevices()
		pFreeDeviceList(pDevice);
    }

	if(hr == S_OK)return retval;
	
	return -3;
}

//
//  FUNCTION: GetWDevState(DWORD* bWifi, DWORD* bPhone, DWORD* bBT)
//
//  PURPOSE:  get status of all wireless devices at once
//
//  USAGE:	
//			
//			GetWDevState(&dwWifi, &dwPhone, &dwBT);
//
//
DWORD GetWDevState(DWORD* bWifi, DWORD* bPhone, DWORD* bBT)
{
	RDD * pDevice = NULL;
    RDD * pTD;

    HRESULT hr;
	DWORD retval = 0;
	
    hr = pGetWirelessDevices(&pDevice, 0);

	if(hr != S_OK) return -1;
	
    if (pDevice)
    {
	    pTD = pDevice;

        // loop through the linked list of devices
		while (pTD)
		{
			switch (pTD->DeviceType)
			{
				case RADIODEVICES_MANAGED:
				*bWifi = pTD->dwState;
				break;
				case RADIODEVICES_PHONE:
				*bPhone = pTD->dwState;
				break;
				case RADIODEVICES_BLUETOOTH:
				*bBT = pTD->dwState;
				break;
				default:
				break;
			}
			pTD = pTD->pNext; 
	    }
        // Free the list of devices retrieved with    
        // GetWirelessDevices()
        pFreeDeviceList(pDevice);
    }

	if(hr == S_OK)return retval;
	
	return -2;
}


