def bench(fn, iters=20):
    """Time fn() on the GPU with CUDA events (not with a Python timer)."""
    for _ in range(3):                      # warm-up
        fn()
    torch.cuda.synchronize()
    start = torch.cuda.Event(enable_timing=True)
    end = torch.cuda.Event(enable_timing=True)
    start.record()
    for _ in range(iters):
        fn()
    end.record()
    torch.cuda.synchronize()
    return start.elapsed_time(end) / iters  # milliseconds per call