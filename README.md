Recent work has argued that sequential consistency (SC) in GPUs can perform on par with weak memory models, provided ordering stalls are made less frequent by relaxing ordering for private and read-only data. In this paper, we address the complementary problem of reducing stall latencies for both read-only and read-write data.

We find that SC stalls are particularly problematic for workloads with inter-workgroup sharing, and occur primarily due to earlier stores in the same thread; a substantial part of the overhead comes from the need to stall until write permissions are obtained (to ensure write atomicity). To address this, we propose RCC, a GPU coherence protocol which grants write permissions without stalling but can still be used to implement SC. RCC uses logical timestamps to determine a global memory order and L1 read permissions; even though each core may see a different logical “time,” SC ordering can still be maintained.

Unlike previous GPU SC proposals, our design does not require invasive core changes and additional per-core storage to classify read-only/private data. For workloads with inter- workgroup sharing overall performance is 29% better and energy is 25% less than in best previous GPU SC proposals, and within 7% of the best non-SC design.

# Paper Link

[Efficient Sequential Consistency in GPUs via Relativistic Cache Coherence](https://ieeexplore.ieee.org/abstract/document/7920861), in HPCA-2017.

# Building

This work was implemented upon [GPGPU-Sim](http://www.gpgpu-sim.org/). The original README of the GPGPU-Sim that we used can be found at [here](./README_GPGPUSim). 
