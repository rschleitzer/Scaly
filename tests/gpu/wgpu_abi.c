/* The host declares wgpu-native's structures itself (gpu_darwin.c, "WebGPU,
 * through wgpu-native"), so that it builds where no header of it is. This
 * file holds those declarations against the header of the wgpu-native that
 * IS here: it compiles, or the two have drifted apart and a kernel handed
 * to this library would be read wrong.
 *
 *   cc -fsyntax-only -I<wgpu-native>/include -DSCALY_GPU_HOST='"<path>/gpu_darwin.c"' wgpu_abi.c
 */
#include <stddef.h>
#include <webgpu.h>
#include SCALY_GPU_HOST

#define SAME_SIZE(ours, theirs) _Static_assert(sizeof(ours) == sizeof(theirs), #ours " is not as large as " #theirs)
#define SAME_PLACE(ours, field, theirs, their_field) \
    _Static_assert(offsetof(ours, field) == offsetof(theirs, their_field), #ours "." #field " is not where " #theirs "." #their_field " is")

SAME_SIZE(wg_text, WGPUStringView);
SAME_SIZE(wg_chain, WGPUChainedStruct);
SAME_PLACE(wg_chain, kind, WGPUChainedStruct, sType);
SAME_SIZE(wg_callback, WGPURequestAdapterCallbackInfo);
SAME_SIZE(wg_callback, WGPURequestDeviceCallbackInfo);
SAME_SIZE(wg_callback, WGPUBufferMapCallbackInfo);
SAME_PLACE(wg_callback, mode, WGPURequestAdapterCallbackInfo, mode);
SAME_PLACE(wg_callback, callback, WGPURequestAdapterCallbackInfo, callback);
SAME_PLACE(wg_callback, user1, WGPURequestAdapterCallbackInfo, userdata1);
SAME_PLACE(wg_callback, callback, WGPUBufferMapCallbackInfo, callback);
SAME_PLACE(wg_callback, user1, WGPUBufferMapCallbackInfo, userdata1);
SAME_SIZE(wg_error_callback, WGPUUncapturedErrorCallbackInfo);
SAME_PLACE(wg_error_callback, callback, WGPUUncapturedErrorCallbackInfo, callback);
SAME_SIZE(wg_named, WGPUShaderModuleDescriptor);
SAME_SIZE(wg_device_descriptor, WGPUDeviceDescriptor);
SAME_PLACE(wg_device_descriptor, limits, WGPUDeviceDescriptor, requiredLimits);
SAME_PLACE(wg_device_descriptor, queue, WGPUDeviceDescriptor, defaultQueue);
SAME_PLACE(wg_device_descriptor, lost, WGPUDeviceDescriptor, deviceLostCallbackInfo);
SAME_PLACE(wg_device_descriptor, error, WGPUDeviceDescriptor, uncapturedErrorCallbackInfo);
_Static_assert(sizeof(WGPULimits) <= 64 * sizeof(unsigned long long), "the block for a card's limits is smaller than WGPULimits");
SAME_SIZE(wg_wgsl, WGPUShaderSourceWGSL);
SAME_PLACE(wg_wgsl, code, WGPUShaderSourceWGSL, code);
SAME_SIZE(wg_stage, WGPUComputeState);
SAME_PLACE(wg_stage, module, WGPUComputeState, module);
SAME_PLACE(wg_stage, entry, WGPUComputeState, entryPoint);
SAME_SIZE(wg_pipeline_descriptor, WGPUComputePipelineDescriptor);
SAME_PLACE(wg_pipeline_descriptor, compute, WGPUComputePipelineDescriptor, compute);
SAME_SIZE(wg_buffer_descriptor, WGPUBufferDescriptor);
SAME_PLACE(wg_buffer_descriptor, usage, WGPUBufferDescriptor, usage);
SAME_PLACE(wg_buffer_descriptor, size, WGPUBufferDescriptor, size);
SAME_PLACE(wg_buffer_descriptor, mapped, WGPUBufferDescriptor, mappedAtCreation);
SAME_SIZE(wg_entry, WGPUBindGroupEntry);
SAME_PLACE(wg_entry, binding, WGPUBindGroupEntry, binding);
SAME_PLACE(wg_entry, buffer, WGPUBindGroupEntry, buffer);
SAME_PLACE(wg_entry, offset, WGPUBindGroupEntry, offset);
SAME_PLACE(wg_entry, size, WGPUBindGroupEntry, size);
SAME_SIZE(wg_group_descriptor, WGPUBindGroupDescriptor);
SAME_PLACE(wg_group_descriptor, layout, WGPUBindGroupDescriptor, layout);
SAME_PLACE(wg_group_descriptor, count, WGPUBindGroupDescriptor, entryCount);
SAME_PLACE(wg_group_descriptor, entries, WGPUBindGroupDescriptor, entries);
SAME_SIZE(unsigned long long, WGPUFuture);

_Static_assert(WG_SPONTANEOUS == WGPUCallbackMode_AllowSpontaneous, "the callback mode");
_Static_assert(WG_WGSL == WGPUSType_ShaderSourceWGSL, "the kind of a WGSL source");
_Static_assert(WG_MAP_READ == WGPUBufferUsage_MapRead && WG_MAP_READ == WGPUMapMode_Read, "reading a mapped buffer");
_Static_assert(WG_COPY_SRC == WGPUBufferUsage_CopySrc && WG_COPY_DST == WGPUBufferUsage_CopyDst, "copying a buffer");
_Static_assert(WG_STORAGE == WGPUBufferUsage_Storage, "a storage buffer");
_Static_assert(WG_OK == WGPURequestAdapterStatus_Success && WG_OK == WGPURequestDeviceStatus_Success
               && WG_OK == WGPUMapAsyncStatus_Success && WG_OK == WGPUStatus_Success, "what success is");

/* the functions, as the host calls them */
static void wgpu_abi_calls(void)
{
    wg.create_instance = (void *(*)(const void *))wgpuCreateInstance;
    wg.adapter_limits = (int (*)(void *, void *))wgpuAdapterGetLimits;
    wg.device_queue = (void *(*)(void *))wgpuDeviceGetQueue;
    wg.write_buffer = (void (*)(void *, void *, unsigned long long, const void *, size_t))wgpuQueueWriteBuffer;
    wg.pass_dispatch = (void (*)(void *, unsigned int, unsigned int, unsigned int))wgpuComputePassEncoderDispatchWorkgroups;
    wg.copy_buffer = (void (*)(void *, void *, unsigned long long, void *, unsigned long long, unsigned long long))wgpuCommandEncoderCopyBufferToBuffer;
    wg.mapped_range = (const void *(*)(void *, size_t, size_t))wgpuBufferGetConstMappedRange;
}
