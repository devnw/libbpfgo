//+build ignore

#include <vmlinux.h>

#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 1 << 24);
} events1 SEC(".maps"), events2 SEC(".maps");

long ringbuffer_flags = 0;

SEC("kprobe/sys_mmap")
int kprobe__sys_mmap(struct pt_regs *ctx)
{
    int *process;

    // Reserve space on the ringbuffer for the sample
    process = bpf_ringbuf_reserve(&events1, sizeof(int), ringbuffer_flags);
    if (!process) {
        return 0;
    }

    *process = 2021;

    bpf_ringbuf_submit(process, ringbuffer_flags);

    process = bpf_ringbuf_reserve(&events2, sizeof(int), ringbuffer_flags);
    if (!process) {
        return 0;
    }

    *process = 2024;

    bpf_ringbuf_submit(process, ringbuffer_flags);
    return 0;
}
