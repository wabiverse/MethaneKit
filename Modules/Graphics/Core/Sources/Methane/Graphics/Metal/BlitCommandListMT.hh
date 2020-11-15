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

FILE: Methane/Graphics/Metal/BlitCommandListMT.hh
Metal implementation of the blit command list interface.

******************************************************************************/

#pragma once

#include "CommandListMT.hpp"

#include <Methane/Graphics/BlitCommandList.h>
#include <Methane/Graphics/CommandListBase.h>

#import <Metal/Metal.h>

namespace Methane::Graphics
{

class CommandQueueMT;

class BlitCommandListMT final
    : public CommandListMT<id<MTLBlitCommandEncoder>, CommandListBase>
    , public BlitCommandList
{
public:
    BlitCommandListMT(CommandQueueBase& command_queue);

    // CommandList interface
    void Reset(CommandList::DebugGroup* p_debug_group = nullptr) final;
};

} // namespace Methane::Graphics
