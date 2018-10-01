#!/usr/bin/env python3

import subprocess as sp
import time as tm
import matplotlib.pyplot as plt
import numpy as np
import os.path


def run_binary(n: int, r: int, t: int):
    start_time = tm.perf_counter()
    result = sp.run(['cmake-build-release/pirv1',
                     '-n{}'.format(n),
                     '-r{}'.format(r),
                     '-t{}'.format(t),
                     ])
    if result.returncode != 0:
        return -1
    else:
        return (tm.perf_counter() - start_time) * 1000


Ts = [1, 2, 4]
Rs = [1, 2, 3, 5, 10, 20, 25, 30, 40, 50]
Ns = [200, 250, 300, 350, 400, 450, 500]


def skip_func(n: int, r: int):
    if n > 400 and r == 1:
        return True
    if n > 500 and r <= 2:
        return True
    if n > 1600 and r <= 5:
        return True
    return False


def additional_measure_count(time):
    return 1
    # uncomment at night :)
    # if time > 15000:
    #     return 0
    # if time > 8000:
    #     return 1
    # if time > 2000:
    #     return 2
    # if time > 900:
    #     return 3
    # else:
    #     return 5


def plot(results):
    for n in results.keys():
        line, = plt.plot(results[n].keys(), results[n].values())
        line.set_label('N = {}'.format(n))
    # plt.legend(["N = {}".format(x) for x in results.keys()])
    plt.legend()
    plt.ylabel('computation time, ms')
    plt.xlabel('block size (R)')
    plt.show()


if __name__ == '__main__':
    name = 'results.npy'
    if os.path.exists(name):
        res = np.load(name).item()
        plot(res)
        exit()
    T = 4 # hard-coded for now
    results = {}
    for N in Ns:
        results[N] = {}
        for R in Rs:
            if skip_func(N, R):
                continue
            time = run_binary(n=N, r=R, t=T)
            if time < 0:
                continue
            measure_count = additional_measure_count(time)
            times = [time] + [run_binary(N, R, T)
                              for i in range(0, measure_count)]
            time = sum(times) / len(times)

            print("for N={} R={} time={:.1f} (mc={})"
                  .format(N, R, time, measure_count + 1))
            results[N][R] = time
    print(results)
    plot(results)
    np.save('results.npy', results)
