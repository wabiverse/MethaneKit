/******************************************************************************

Copyright 2020 Evgeny Gorodetskiy

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

FILE: Methane/Graphics/Metal/Fence.hh
Metal fence implementation.

******************************************************************************/

#pragma once

#include <Methane/Graphics/Base/Fence.h>
#include <Methane/Instrumentation.h>

#include <mutex>
#include <condition_variable>

#import <Metal/Metal.h>

namespace Methane::Graphics::Metal
{

class CommandQueue;

class Fence final : public Base::Fence
{
public:
    explicit Fence(Base::CommandQueue& command_queue);

    // IFence overrides
    void Signal() override;
    void WaitOnCpu() override;
    void WaitOnGpu(Rhi::ICommandQueue& wait_on_command_queue) override;

    // IObject override
    bool SetName(std::string_view name) override;

private:
    CommandQueue& GetMetalCommandQueue();
    
    static const dispatch_queue_t& GetDispatchQueue();

    id<MTLSharedEvent>          m_mtl_event;
    MTLSharedEventListener*     m_mtl_event_listener;
    TracyLockable(std::mutex,   m_wait_mutex);
    std::condition_variable_any m_wait_condition_var;
    bool                        m_is_signalled = false;
};

} // namespace Methane::Graphics::Metal
