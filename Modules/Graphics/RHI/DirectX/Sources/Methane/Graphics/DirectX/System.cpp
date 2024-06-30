/******************************************************************************

Copyright 2019-2020 Evgeny Gorodetskiy

Licensed under the Apache License, Version 2.0 (the "License"),
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************

FILE: Methane/Graphics/Metal/System.cpp
DirectX 12 implementation of the device interface.

******************************************************************************/

#include <Methane/Graphics/DirectX/System.h>
#include <Methane/Graphics/DirectX/Device.h>
#include <Methane/Graphics/DirectX/ErrorHandling.h>

#include <Methane/Platform/Windows/Utils.h>
#include <Methane/Instrumentation.h>
#include <Methane/Checks.hpp>

#ifdef _DEBUG
#include <dxgidebug.h>

// Uncomment to enable debugger breakpoint on DirectX debug warning or error
// #define BREAK_ON_DIRECTX_DEBUG_LAYER_MESSAGE_ENABLED
#endif

#include <boost/nowide/convert.hpp>
#include <array>
#include <algorithm>
#include <cassert>

namespace Methane::Graphics::Rhi
{

Rhi::ISystem& Rhi::ISystem::Get()
{
    META_FUNCTION_TASK();
    static const auto s_system_ptr = std::make_shared<DirectX::System>();
    return *s_system_ptr;
}

} // namespace Methane::Graphics::Rhi

namespace Methane::Graphics::DirectX
{

#ifdef _DEBUG

static bool EnableDebugLayer()
{
    META_FUNCTION_TASK();
    wrl::ComPtr<ID3D12Debug> debug_controller_cptr;
    if (!SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller_cptr))))
    {
        META_LOG("WARNING: Unable to get D3D12 debug interface. " \
                 "Install 'Graphics Tools' in Windows optional features and try again.");
        return false;
    }

    debug_controller_cptr->EnableDebugLayer();

    wrl::ComPtr<IDXGIInfoQueue> dxgi_info_queue_cptr;
    if (!SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgi_info_queue_cptr))))
    {
        META_LOG("WARNING: Unable to get D3D12 info-queue interface.");
        return true;
    }

#ifdef BREAK_ON_DIRECTX_DEBUG_LAYER_MESSAGE_ENABLED
    dxgi_info_queue_cptr->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, true);
    dxgi_info_queue_cptr->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
    dxgi_info_queue_cptr->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
#endif

    std::array<DXGI_INFO_QUEUE_MESSAGE_ID, 0> skip_message_ids = {{ }};
    std::array<DXGI_INFO_QUEUE_MESSAGE_SEVERITY, 1> skip_message_severities = {{
        DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO,
    }};

    DXGI_INFO_QUEUE_FILTER filter {};
    filter.DenyList.NumSeverities = static_cast<UINT>(skip_message_severities.size());
    filter.DenyList.pSeverityList = skip_message_severities.data();
    filter.DenyList.NumIDs        = static_cast<UINT>(skip_message_ids.size());
    filter.DenyList.pIDList       = skip_message_ids.data();
    dxgi_info_queue_cptr->AddStorageFilterEntries(DXGI_DEBUG_ALL, &filter);

    return true;
}

#endif

System::System()
{
    META_FUNCTION_TASK();
    Initialize();
}

System::~System()
{
    META_FUNCTION_TASK();

#ifdef ADAPTERS_CHANGE_HANDLING
    UnregisterAdapterChangeEvent();
#endif

    m_cp_factory.Reset();

    ClearDevices();
    ReportLiveObjects();
}

void System::Initialize()
{
    META_FUNCTION_TASK();
    UINT dxgi_factory_flags = 0;

#ifdef _DEBUG
    if (EnableDebugLayer())
        dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

    ThrowIfFailed(CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&m_cp_factory)));
    META_CHECK_ARG_NOT_NULL(m_cp_factory);

#ifdef ADAPTERS_CHANGE_HANDLING
    RegisterAdapterChangeEvent();
#endif
}

#ifdef ADAPTERS_CHANGE_HANDLING

void System::RegisterAdapterChangeEvent()
{
    META_FUNCTION_TASK();
    wrl::ComPtr<IDXGIFactory7> cp_factory7;
    if (!SUCCEEDED(m_cp_factory->QueryInterface(IID_PPV_ARGS(&cp_factory7))))
        return;

    m_adapter_change_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_adapter_change_event == nullptr)
    {
        ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
    }

    META_CHECK_ARG_NOT_NULL(cp_factory7);
    ThrowIfFailed(cp_factory7->RegisterAdaptersChangedEvent(m_adapter_change_event, &m_adapter_change_registration_cookie));
}

void System::UnregisterAdapterChangeEvent()
{
    META_FUNCTION_TASK();
    wrl::ComPtr<IDXGIFactory7> cp_factory7;
    if (m_adapter_change_registration_cookie == 0 ||
        !SUCCEEDED(m_cp_factory->QueryInterface(IID_PPV_ARGS(&cp_factory7))))
        return;

    META_CHECK_ARG_NOT_NULL(cp_factory7);
    ThrowIfFailed(cp_factory7->UnregisterAdaptersChangedEvent(m_adapter_change_registration_cookie));
    m_adapter_change_registration_cookie = 0;

    CloseHandle(m_adapter_change_event);
    m_adapter_change_event = NULL;
}

#endif // def ADAPTERS_CHANGE_HANDLING

void System::CheckForChanges()
{
    META_FUNCTION_TASK();

#ifdef ADAPTERS_CHANGE_HANDLING
    const bool adapters_changed = m_adapter_change_event ? WaitForSingleObject(m_adapter_change_event, 0) == WAIT_OBJECT_0
                                                         : !m_cp_factory->IsCurrent();

    if (!adapters_changed)
        return;

#ifdef ADAPTERS_CHANGE_HANDLING
    UnregisterAdapterChangeEvent();
#endif

    Initialize();

    const Ptrs<IDevice>& devices = GetGpuDevices();
    Ptrs<IDevice>   prev_devices = devices;
    UpdateGpuDevices(GetDeviceCapabilities());

    for (const Ptr<IDevice>& prev_device_ptr : prev_devices)
    {
        META_CHECK_ARG_NOT_NULL(prev_device_ptr);
        Device& prev_device = static_cast<Device&>(*prev_device_ptr);
        auto device_it = std::find_if(devices.begin(), devices.end(),
                                      [prev_device](const Ptr<IDevice>& device_ptr)
                                      {
                                          Device& device = static_cast<Device&>(*device_ptr);
                                          return prev_device.GetNativeAdapter().GetAddressOf() == device.GetNativeAdapter().GetAddressOf();
                                      });

        if (device_it == devices.end())
        {
            RemoveDevice(prev_device);
        }
    }
#endif
}

const Ptrs<Rhi::IDevice>& System::UpdateGpuDevices(const Platform::AppEnvironment&, const Rhi::DeviceCaps& required_device_caps)
{
    META_FUNCTION_TASK();
    return UpdateGpuDevices(required_device_caps);
}

const Ptrs<Rhi::IDevice>& System::UpdateGpuDevices(const Rhi::DeviceCaps& required_device_caps)
{
    META_FUNCTION_TASK();
    META_CHECK_ARG_NOT_NULL(m_cp_factory);

    const D3D_FEATURE_LEVEL dx_feature_level = D3D_FEATURE_LEVEL_11_0;
    SetDeviceCapabilities(required_device_caps);
    ClearDevices();

    IDXGIAdapter1* p_adapter = nullptr;
    for (UINT adapter_index = 0; DXGI_ERROR_NOT_FOUND != m_cp_factory->EnumAdapters1(adapter_index, &p_adapter); ++adapter_index)
    {
        META_CHECK_ARG_NOT_NULL(p_adapter);
        if (IsSoftwareAdapterDxgi(*p_adapter))
            continue;

        AddDevice(p_adapter, dx_feature_level);
    }

    wrl::ComPtr<IDXGIAdapter> cp_warp_adapter;
    m_cp_factory->EnumWarpAdapter(IID_PPV_ARGS(&cp_warp_adapter));
    if (cp_warp_adapter)
    {
        AddDevice(cp_warp_adapter, dx_feature_level);
    }

    return GetGpuDevices();
}

void System::AddDevice(const wrl::ComPtr<IDXGIAdapter>& cp_adapter, D3D_FEATURE_LEVEL feature_level)
{
    META_FUNCTION_TASK();

    // Check to see if the adapter supports Direct3D 12, but don't create the actual device yet
    if (!SUCCEEDED(D3D12CreateDevice(cp_adapter.Get(), feature_level, _uuidof(ID3D12Device), nullptr)))
        return;

    if (const Rhi::DeviceFeatureMask device_supported_features = Device::GetSupportedFeatures(cp_adapter, feature_level);
        !device_supported_features.HasBits(GetDeviceCapabilities().features))
        return;

    Base::System::AddDevice(std::make_shared<Device>(cp_adapter, feature_level, GetDeviceCapabilities()));
}

void System::ReportLiveObjects() const noexcept
{
    META_FUNCTION_TASK();
#ifdef _DEBUG
    wrl::ComPtr<IDXGIDebug1> dxgi_debug;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgi_debug))) && dxgi_debug)
    {
        dxgi_debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
    }
#endif
}

} // namespace Methane::Graphics::DirectX
