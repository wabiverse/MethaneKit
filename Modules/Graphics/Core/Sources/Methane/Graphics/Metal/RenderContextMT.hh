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

FILE: Methane/Graphics/Metal/RenderContextMT.hh
Metal implementation of the render context interface.

******************************************************************************/

#pragma once

#include "ContextMT.hpp"

#include <Methane/Graphics/RenderContextBase.h>
#include <Methane/TracyGpu.hpp>

#import <Methane/Platform/MacOS/AppViewMT.hh>
#import <Metal/Metal.h>

namespace Methane::Graphics
{

class RenderContextMT final : public ContextMT<RenderContextBase>
{
public:
    RenderContextMT(const Platform::AppEnvironment& env, DeviceBase& device, tf::Executor& parallel_executor, const Settings& settings);
    ~RenderContextMT() final;

    // Context interface
    void  WaitForGpu(WaitFor wait_for) final;

    // RenderContext interface
    bool     ReadyToRender() const final;
    void     Resize(const FrameSize& frame_size) final;
    void     Present() final;
    bool     SetVSyncEnabled(bool vsync_enabled) final;
    bool     SetFrameBuffersCount(uint32_t frame_buffers_count) final;
    float    GetContentScalingFactor() const final;
    uint32_t GetFontResolutionDpi() const final;
    Platform::AppView GetAppView() const final { return { m_app_view }; }

    // ContextBase overrides
    void Initialize(DeviceBase& device, bool deferred_heap_allocation, bool is_callback_emitted = true)  final;
    void Release() final;

    id<CAMetalDrawable> GetNativeDrawable()       { return m_app_view.currentDrawable; }
    CommandQueueMT&     GetRenderCommandQueueMT();

    void OnGpuExecutionCompleted();

private:
    AppViewMT*           m_app_view;
    id<MTLCaptureScope>  m_frame_capture_scope;
    dispatch_semaphore_t m_dispatch_semaphore;
};

} // namespace Methane::Graphics
