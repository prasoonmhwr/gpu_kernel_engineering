"""Session 1 demo: is the GPU visible, and how fast is a 4096 x 4096 matmul?

Run:  python my_script.py
"""
import torch


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


def main():
    print("torch:", torch.__version__, "| CUDA available:", torch.cuda.is_available())
    if not torch.cuda.is_available():
        raise SystemExit("No GPU visible. Check the driver, or pick a GPU runtime.")
    print("GPU:", torch.cuda.get_device_name(0))

    n = 4096
    x = torch.randn(n, n, device="cuda")
    y = torch.randn(n, n, device="cuda")
    z = x @ y
    print("z:", z.shape, z.dtype, z.device)

    # Correctness: compare against the same product computed on the CPU.
    ref = x.cpu() @ y.cpu()
    print("max abs error vs CPU:", (z.cpu() - ref).abs().max().item())

    # Speed: FLOPs of a matmul ~ 2 * M * N * K
    ms = bench(lambda: x @ y)
    tflops = 2 * n**3 / (ms * 1e-3) / 1e12
    print(f"{ms:.2f} ms per matmul, {tflops:.1f} TFLOPs (fp32)")


if __name__ == "__main__":
    main()