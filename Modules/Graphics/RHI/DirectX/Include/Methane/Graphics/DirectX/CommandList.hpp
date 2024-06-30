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

FILE: Methane/Graphics/DirectX/CommandList.hpp
DirectX 12 base template implementation of the command list interface.

******************************************************************************/

#pragma once

#include "ICommandList.h"
#include "CommandListDebugGroup.h"
#include "CommandQueue.h"
#include "Device.h"
#include "IContext.h"
#include "IResource.h"
#include "ProgramBindings.h"
#include "ErrorHandling.h"

#include <Methane/Graphics/Base/CommandList.h>
#include <Methane/Instrumentation.h>
#include <Methane/Checks.hpp>
#include <Methane/Memory.hpp>

#include <wrl.h>
#include <directx/d3d12.h>
#include <pix.h>
#include <boost/nowide/convert.hpp>
#include <fmt/format.h>

#ifdef METHANE_LOGGING_ENABLED
#include <magic_enum.hpp>
#endif

namespace Methane::Graphics::DirectX
{

namespace wrl = Microsoft::WRL;

template<class CommandListBaseT, typename = std::enable_if_t<std::is_base_of_v<Base::CommandList, CommandListBaseT>>>
class CommandList
    : public CommandListBaseT
    , public ICommandList
{
public:
    template<typename... ConstructArgs>
    explicit CommandList(D3D12_COMMAND_LIST_TYPE command_list_type, ConstructArgs&&... construct_args)
        : CommandListBaseT(std::forward<ConstructArgs>(construct_args)...)
    {
        META_FUNCTION_TASK();
        const wrl::ComPtr<ID3D12Device>& cp_device = GetDirectCommandQueue().GetDirectContext().GetDirectDevice().GetNativeDevice();
        META_CHECK_ARG_NOT_NULL(cp_device);

        ThrowIfFailed(cp_device->CreateCommandAllocator(command_list_type, IID_PPV_ARGS(&m_cp_command_allocator)), cp_device.Get());
        ThrowIfFailed(cp_device->CreateCommandList(0, command_list_type, m_cp_command_allocator.Get(), nullptr, IID_PPV_ARGS(&m_cp_command_list)), cp_device.Get());
        m_cp_command_list.As(&m_cp_command_list_4);

        InitializeTimestampQueries();
        BeginGpuZoneDx();

        SetCommandListState(Rhi::CommandListState::Encoding);
    }

    // Rhi::ICommandList interface

    void PushDebugGroup(Rhi::ICommandListDebugGroup& debug_group) final
    {
        META_FUNCTION_TASK();
        CommandListBaseT::PushDebugGroup(debug_group);
        const std::wstring& group_name = static_cast<CommandListDebugGroup&>(debug_group).GetWideName();
        PIXBeginEvent(m_cp_command_list.Get(), 0, group_name.c_str());
    }

    void PopDebugGroup() final
    {
        META_FUNCTION_TASK();
        CommandListBaseT::PopDebugGroup();
        PIXEndEvent(m_cp_command_list.Get());
    }

    void Commit() override
    {
        META_FUNCTION_TASK();
        const auto state_lock = Base::CommandList::LockStateMutex();
        CommandListBaseT::Commit();

        EndGpuZoneDx();

        m_cp_command_list->Close();
        m_is_native_committed = true;
    }

    void SetResourceBarriers(const Rhi::IResourceBarriers& resource_barriers) final
    {
        META_FUNCTION_TASK();
        VerifyEncodingState();
        
        const auto lock_guard = static_cast<const Base::ResourceBarriers&>(resource_barriers).Lock();
        if (resource_barriers.IsEmpty())
            return;

        META_LOG("{} Command list '{}' SET RESOURCE BARRIERS:\n{}",
                 magic_enum::enum_name(GetType()), GetName(), static_cast<std::string>(resource_barriers));
        META_CHECK_ARG_NOT_NULL(m_cp_command_list);

        const auto& dx_resource_barriers = static_cast<const IResource::Barriers&>(resource_barriers);
        const std::vector<D3D12_RESOURCE_BARRIER>& d3d12_resource_barriers = dx_resource_barriers.GetNativeResourceBarriers();
        m_cp_command_list->ResourceBarrier(static_cast<UINT>(d3d12_resource_barriers.size()), d3d12_resource_barriers.data());
    }

    // Rhi::ICommandList interface

    void Reset(Rhi::ICommandListDebugGroup* debug_group_ptr) override
    {
        META_FUNCTION_TASK();
        const auto state_lock = Base::CommandList::LockStateMutex();
        if (!m_is_native_committed)
            return;

        m_is_native_committed = false;

        const wrl::ComPtr<ID3D12Device>& cp_device = GetDirectCommandQueue().GetDirectContext().GetDirectDevice().GetNativeDevice();
        ThrowIfFailed(m_cp_command_allocator->Reset(), cp_device.Get());
        ThrowIfFailed(m_cp_command_list->Reset(m_cp_command_allocator.Get(), nullptr), cp_device.Get());

        BeginGpuZoneDx();

        Base::CommandList::Reset(debug_group_ptr);
    }

    // IObject interface

    bool SetName(std::string_view name) final
    {
        META_FUNCTION_TASK();
        if (!CommandListBaseT::SetName(name))
            return false;

        META_CHECK_ARG_NOT_NULL(m_cp_command_list);
        m_cp_command_list->SetName(nowide::widen(name).c_str());

        META_CHECK_ARG_NOT_NULL(m_cp_command_allocator);
        m_cp_command_allocator->SetName(nowide::widen(fmt::format("{} allocator", name)).c_str());

        return true;
    }

    // DirectX::ICommandList interface

    CommandQueue&              GetDirectCommandQueue() final      { return static_cast<CommandQueue&>(GetBaseCommandQueue()); }
    Rhi::CommandListType       GetCommandListType() const final   { return Base::CommandList::GetType(); }
    ID3D12GraphicsCommandList& GetNativeCommandList() const final
    {
        META_CHECK_ARG_NOT_NULL(m_cp_command_list);
        return *m_cp_command_list.Get();
    }
    ID3D12GraphicsCommandList4* GetNativeCommandList4() const final { return m_cp_command_list_4.Get(); }

protected:
    void ApplyProgramBindings(Base::ProgramBindings& program_bindings, Rhi::ProgramBindingsApplyBehaviorMask apply_behavior) final
    {
        // Optimization to skip dynamic_cast required to call Apply method of the Base::ProgramBinding implementation
        static_cast<ProgramBindings&>(program_bindings).Apply(*this, Base::CommandList::GetProgramBindingsPtr(), apply_behavior);
    }

    bool IsNativeCommitted() const             { return m_is_native_committed; }
    void SetNativeCommitted(bool is_committed) { m_is_native_committed = is_committed; }

    ID3D12CommandAllocator& GetNativeCommandAllocatorRef()
    {
        META_CHECK_ARG_NOT_NULL(m_cp_command_allocator);
        return *m_cp_command_allocator.Get();
    }

    ID3D12GraphicsCommandList& GetNativeCommandListRef()
    {
        META_CHECK_ARG_NOT_NULL(m_cp_command_list);
        return *m_cp_command_list.Get();
    }

    void BeginGpuZoneDx()
    {
        CommandListBaseT::BeginGpuZone();
#if defined(METHANE_GPU_INSTRUMENTATION_ENABLED) && METHANE_GPU_INSTRUMENTATION_ENABLED == 2
        static const std::string cl_unnamed = "Unnamed Command List";
        const std::string& cl_name = GetName();
        const std::string_view zone_name = cl_name.empty() ? cl_unnamed : cl_name;
        m_tracy_gpu_scope_opt.emplace(GetDirectCommandQueue().GetTracyD3D12Ctx(),
                                      static_cast<uint32_t>(__LINE__), __FILE__, strlen(__FILE__),
                                      __FUNCTION__, strlen(__FUNCTION__),
                                      zone_name.data(), zone_name.length(),
                                      m_cp_command_list.Get(), true);
#endif
    }

    void EndGpuZoneDx()
    {
        CommandListBaseT::EndGpuZone();
#if defined(METHANE_GPU_INSTRUMENTATION_ENABLED) && METHANE_GPU_INSTRUMENTATION_ENABLED == 2
        m_tracy_gpu_scope_opt.reset();
#endif
    }

private:
    wrl::ComPtr<ID3D12CommandAllocator>       m_cp_command_allocator;
    wrl::ComPtr<ID3D12GraphicsCommandList>    m_cp_command_list;
    wrl::ComPtr<ID3D12GraphicsCommandList4>   m_cp_command_list_4;    // extended interface for the same command list (may be unavailable on older Windows)
    bool                                      m_is_native_committed = false;

#if defined(METHANE_GPU_INSTRUMENTATION_ENABLED) && METHANE_GPU_INSTRUMENTATION_ENABLED == 2
    Opt<tracy::D3D12ZoneScope> m_tracy_gpu_scope_opt;
#endif
};

} // namespace Methane::Graphics::DirectX
