#include "Topology Telephony Controller.h"
#include <iostream>

HRESULT TelephonyController::Initialize()
{
    CComPtr<IMMDeviceEnumerator> spEnumerator;
    CComPtr<IMMDeviceCollection> spMMDeviceCollection;
    UINT deviceCount = 0;
    BOOL fTelephonyBidiEndpointFound = FALSE;

    HRESULT hr = S_OK;

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (FAILED(hr))
	{
		//std::cout << "CoInitializeEx failed: " << std::error_code(hr, std::system_category()).message() << std::endl;
		return hr;
	}

    hr = spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER);

    if (SUCCEEDED(hr))
    {
        hr = spEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED, &spMMDeviceCollection);
    }

    if (SUCCEEDED(hr))
    {
        hr = spMMDeviceCollection->GetCount(&deviceCount);
    }

    // This loop will iterate through all the endpoints and look the KSNODETYPE_Telephony_RX endpoint
    // For this endpoint it will then find a topology filter and a wave filter and activate IKSControl on these filters.
    for (UINT i = 0; i < deviceCount; i++)
    {
        CComPtr<IMMDevice>       spEndpoint;
        CComPtr<IMMDevice>       spDevice;
        CComPtr<IPart>           spTopologyFilterEndpointConnectorPart;
        CComPtr<IDeviceTopology> spEndpointTopology;
        CComPtr<IConnector>      spEndpointConnector;
        CComPtr<IConnector>      spTopologyFilterEndpointConnector;
        CComPtr<IDeviceTopology> spAdapterTopology;
        CComPtr<IKsControl>      spWaveKsControl;
        CComHeapPtr<wchar_t>     wszAdapterTopologyDeviceId;
        CComPtr<IMMDevice>       spAdapterDevice;
        GUID                     guidEndpointClass = GUID_NULL;
        CComPtr<IConnector>      spConnector;
        CComPtr<IConnector>      spConConnectedTo;
        CComPtr<IPart>           spConnectedPart;
        CComPtr<IDeviceTopology> spMiniportTopology;
        CComHeapPtr<wchar_t>     wszPhoneTopologyDeviceId;
        CComPtr<IMMDevice>       spWaveAdapterDevice;
        CComPtr<IPartsList>      spPartsList;
        CComPtr<IPart>           spPart;

        if (SUCCEEDED(hr))
        {
            hr = spMMDeviceCollection->Item(i, &spEndpoint);
        }

        // Get the IDeviceTopology interface from the endpoint device.
        if (SUCCEEDED(hr))
        {
            hr = spEndpoint->Activate(
                __uuidof(IDeviceTopology),
                CLSCTX_ALL,
                NULL,
                (VOID**)&spEndpointTopology);
        }

        // Get the default connector for the endpoint device which is connected to
        // the connector on the topology filter.
        if (SUCCEEDED(hr))
        {
            hr = spEndpointTopology->GetConnector(0, &spEndpointConnector);
        }

        // Get the endpoint connector on the topology filter.
        if (SUCCEEDED(hr))
        {
            hr = spEndpointConnector->GetConnectedTo(&spTopologyFilterEndpointConnector);
        }

        // Get the endpoint connector part, endpoint class, and local ID.
        if (SUCCEEDED(hr))
        {
            hr = spTopologyFilterEndpointConnector.QueryInterface(&spTopologyFilterEndpointConnectorPart);
        }

        if (SUCCEEDED(hr))
        {
            hr = spTopologyFilterEndpointConnectorPart->GetSubType(&guidEndpointClass);
        }

        // If this is the TELEPHONY_BIDI node we've found the Telephony topology
        if (SUCCEEDED(hr))
        {
            if (guidEndpointClass != KSNODETYPE_TELEPHONY_BIDI)
            {
                continue;
            }
        }

        // Get the IDeviceTopology interface for the adapter device from the endpoint connector part.
        if (SUCCEEDED(hr))
        {
            fTelephonyBidiEndpointFound = TRUE;

            m_MMDevice = spEndpoint;

            hr = spTopologyFilterEndpointConnectorPart->GetTopologyObject(&spAdapterTopology);
        }

        // Activate the KSControl on topo filter
        if (NULL == m_spTopologyKsControl)
        {
            if (SUCCEEDED(hr))
            {
                hr = spAdapterTopology->GetDeviceId(&wszAdapterTopologyDeviceId);
            }

            if (SUCCEEDED(hr))
            {
                hr = spEnumerator->GetDevice(wszAdapterTopologyDeviceId, &spAdapterDevice);
            }

            if (SUCCEEDED(hr))
            {
                hr = spAdapterDevice->Activate(
                    __uuidof(IKsControl),
                    CLSCTX_ALL,
                    NULL,
                    (VOID**)&m_spTopologyKsControl);

                if (FAILED(hr))
                {
                    //LogFailure(L"Activating KsControl for Topo filter failed. hr=0x%x\n", hr);
                }
            }
        }

        // Now proceed with enumerating the rest of the topology
        if (SUCCEEDED(hr))
        {
            hr = spTopologyFilterEndpointConnectorPart->EnumPartsOutgoing(&spPartsList);
        }

        if (SUCCEEDED(hr))
        {
            hr = spPartsList->GetPart(0, &spPart);
        }

        if (SUCCEEDED(hr))
        {
            hr = spPart.QueryInterface(&spConnector);
        }

        if (SUCCEEDED(hr))
        {
            hr = spConnector->GetConnectedTo(&spConConnectedTo);
        }

        // retrieve the part that this connector is on, which should be a part
        // on the miniport.
        if (SUCCEEDED(hr))
        {
            hr = spConConnectedTo.QueryInterface(&spConnectedPart);
        }

        if (SUCCEEDED(hr))
        {
            hr = spConnectedPart->GetTopologyObject(&spMiniportTopology);
        }

        if (SUCCEEDED(hr))
        {
            hr = spMiniportTopology->GetDeviceId(&wszPhoneTopologyDeviceId);
        }

        if (SUCCEEDED(hr))
        {
            hr = spEnumerator->GetDevice(wszPhoneTopologyDeviceId, &spWaveAdapterDevice);
        }

        if (SUCCEEDED(hr))
        {
            hr = spWaveAdapterDevice->Activate(
                __uuidof(IKsControl),
                CLSCTX_ALL,
                NULL,
                (VOID**)&spWaveKsControl);

            if (FAILED(hr))
            {
                //LogFailure(L"Activating KsControl for Wave filter failed. hr =0x%x\n", hr);
            }
        }

        if (SUCCEEDED(hr))
        {
            m_spWaveKsControl = spWaveKsControl;
        }
    }

    if (!fTelephonyBidiEndpointFound)
    {
        hr = E_NOTFOUND;
    }

    if (FAILED(hr))
    {
        //LogFailure(L"Initialization failed with hr=0x%x\n", hr);
    }

    return hr;
}

CComPtr<IMMDevice> TelephonyController::GetMMDevice()
{
    return m_MMDevice;
}

// Sets tel volume. This is a property on topology filter.
HRESULT TelephonyController::SetTelephonyVolume(_In_ float volume)
{
	LONG properVolume;
    HRESULT hr = S_OK;
    KSPROPERTY volumeProp;
    ULONG ulBytesReturned;
    KSPROPERTY_DESCRIPTION desc = { 0 };
    KSPROPERTY_DESCRIPTION * pdesc = NULL;
    PKSPROPERTY_MEMBERSHEADER pKsPropMembHead = NULL;
    PKSPROPERTY_STEPPING_LONG pKsPropStepLong = NULL;

    if (m_spTopologyKsControl == NULL)
    {
        hr = E_POINTER;
    }

    // Determine valid volume ranges
    if (SUCCEEDED(hr))
    {
        KSPROPERTY volumeSupportProp;
        ULONG cbSupportProp = 0;

        volumeSupportProp.Set = KSPROPSETID_TelephonyTopology;
        volumeSupportProp.Id = KSPROPERTY_TELEPHONY_VOLUME;
        volumeSupportProp.Flags = KSPROPERTY_TYPE_BASICSUPPORT;

        hr = m_spTopologyKsControl->KsProperty(
            (PKSPROPERTY)&volumeSupportProp,
            sizeof(volumeSupportProp),
            &desc,
            sizeof(desc),
            &cbSupportProp
            );

        if (SUCCEEDED(hr) && desc.DescriptionSize > sizeof(desc))
        {
            pdesc = (KSPROPERTY_DESCRIPTION*)CoTaskMemAlloc(desc.DescriptionSize);
            if (NULL == pdesc)
            {
                hr = E_OUTOFMEMORY;
            }

            if (SUCCEEDED(hr))
            {
                hr = m_spTopologyKsControl->KsProperty(
                    (PKSPROPERTY)&volumeSupportProp,
                    sizeof(volumeSupportProp),
                    pdesc,
                    desc.DescriptionSize,
                    &cbSupportProp
                    );
            }

            if (SUCCEEDED(hr))
            {
                if (pdesc->PropTypeSet.Set != KSPROPTYPESETID_General ||
                    pdesc->PropTypeSet.Id != VT_I4 ||
                    pdesc->PropTypeSet.Flags != 0 ||
                    pdesc->MembersListCount < 1 ||
                    pdesc->Reserved != 0 ||
                    pdesc->DescriptionSize < (sizeof(KSPROPERTY_DESCRIPTION)+sizeof(KSPROPERTY_MEMBERSHEADER)+sizeof(KSPROPERTY_STEPPING_LONG)))
                {
                    //LogFailure(L"Full volume basicsupport Property Set is invalid or isn't large enough to include stepping information\n");
                    hr = E_INVALIDARG;
                }
            }

            if (SUCCEEDED(hr))
            {
                pKsPropMembHead = reinterpret_cast<PKSPROPERTY_MEMBERSHEADER>(pdesc + 1);
                ULONG flags = pKsPropMembHead->Flags & (KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_MULTICHANNEL | KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_UNIFORM);
                if (pKsPropMembHead->MembersFlags != KSPROPERTY_MEMBER_STEPPEDRANGES ||
                    pKsPropMembHead->MembersSize < sizeof(KSPROPERTY_STEPPING_LONG) ||
                    pKsPropMembHead->MembersCount < 1 ||
                    (flags != (KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_MULTICHANNEL | KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_UNIFORM)
                    && flags != 0))
                {
                    //LogFailure(L"Volume basicsupport Property members invalid\n");
                    hr = E_INVALIDARG;
                }
            }

            if (SUCCEEDED(hr))
            {
                pKsPropStepLong = reinterpret_cast<PKSPROPERTY_STEPPING_LONG>(pKsPropMembHead + 1);

				volume = pKsPropStepLong->Bounds.SignedMinimum + volume * (pKsPropStepLong->Bounds.SignedMaximum - pKsPropStepLong->Bounds.SignedMinimum);
				properVolume = long(volume);

                // Round volume to nearest supported value
				properVolume = VALUE_NORMALIZE_IN_RANGE_EX(
					properVolume,
                    pKsPropStepLong->Bounds.SignedMinimum,
                    pKsPropStepLong->Bounds.SignedMaximum,
                    (LONG)(pKsPropStepLong->SteppingDelta)
                    );

                /*LogComment(L"Applying driver stepping: %d %d %d (step, min, max); new volume = %d\n",
                    pKsPropStepLong->SteppingDelta,
                    pKsPropStepLong->Bounds.SignedMinimum,
                    pKsPropStepLong->Bounds.SignedMaximum,
					properVolume
                    );*/
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        volumeProp.Set = KSPROPSETID_TelephonyTopology;
        volumeProp.Id = KSPROPERTY_TELEPHONY_VOLUME;
        volumeProp.Flags = KSPROPERTY_TYPE_SET;

        hr = m_spTopologyKsControl->KsProperty(
            (PKSPROPERTY)&volumeProp,
            sizeof(volumeProp),
            &properVolume,
            sizeof(LONG),
            &ulBytesReturned
            );
    }

    if (pdesc)
    {
        CoTaskMemFree(pdesc);
    }
    return hr;
}