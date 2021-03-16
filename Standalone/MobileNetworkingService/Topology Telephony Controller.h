#ifndef TELEPHONYCONTROLLER_H_
#define TELEPHONYCONTROLLER_H_

#include <atlcomcli.h>
#include <atlcoll.h>
#include <Windows.h>
#include <atlbase.h>
#include <atlstr.h>
#include <string.h>
#include "mmdeviceapi.h"
#include "devicetopology.h"

#define LogFailure wprintf
#define LogComment wprintf
// From sysvad.h
#define VALUE_NORMALIZE_P(v, step) \
    ((((v)+(step) / 2) / (step)) * (step))

#define VALUE_NORMALIZE(v, step) \
    ((v) > 0 ? VALUE_NORMALIZE_P((v), (step)) : -(VALUE_NORMALIZE_P(-(v), (step))))

#define VALUE_NORMALIZE_IN_RANGE_EX(v, min, max, step) \
    ((v) > (max) ? (max) : \
    (v) < (min) ? (min) : \
    VALUE_NORMALIZE((v), (step)))

#define MAX_PROVIDERIDS 4

// From sysvad.h
#define VALUE_NORMALIZE_P(v, step) \
    ((((v)+(step) / 2) / (step)) * (step))

#define VALUE_NORMALIZE(v, step) \
    ((v) > 0 ? VALUE_NORMALIZE_P((v), (step)) : -(VALUE_NORMALIZE_P(-(v), (step))))

#define VALUE_NORMALIZE_IN_RANGE_EX(v, min, max, step) \
    ((v) > (max) ? (max) : \
    (v) < (min) ? (min) : \
    VALUE_NORMALIZE((v), (step)))

class TelephonyController
{

public:
    HRESULT Initialize();
    HRESULT SetTelephonyVolume(_In_ float volume);
    CComPtr<IMMDevice> GetMMDevice();

private:
    CComPtr<IKsControl>     m_spWaveKsControl;      // KsControl for wave filter
    CComPtr<IKsControl>     m_spTopologyKsControl;  // KsControl for topology filter
    CComPtr<IMMDevice>      m_MMDevice;
};

#endif