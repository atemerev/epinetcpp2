#!/usr/bin/env python3


import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.optimize import brentq
import seaborn as sns
import sys


N0 = 1                                                      # initial number of infected individuals
T_IMMUNITY = 200                                            # the timescale at which immunity decays
T_RECOVERY = 20                                             # average recovery time (in days)
T_EQUILIBRIUM = 500                                         # how long it takes to reach equilibrium
TMAX = 730                                                  # the simulation timespan (in days)


def one_run(beta, N, progress_step_in_percents = 1):
    tot_vals = []                                           # the time development of the number of infected individuals
    new_vals = []                                           # the time development of the number of new infections
    infected_when = []                                      # for each infection, we store how many time steps are there since the previous infection (only in equilibrium - when t >= T_EQUILIBRIUM)
    healthy_since = np.zeros(N)                             # time step when the agent last became healthy
    healthy_since[:] = -1e6                                 # we start with immune-naive population
    s = np.zeros(N, dtype = np.int8)                        # agent states (0 = susceptible, 1 = infected)
    imm = np.zeros(N)                                       # agent immunity values (0 = no immunity, 1 = total immunity)
    s[np.random.choice(N, size = N0, replace = False)] = 1  # N0 agents are initially infected
    tot_vals.append(N0)
    new_vals.append(0)
    for t in range(TMAX):
        # if t % (progress_step_in_percents * TMAX // 100) == 0:
        #     print('{:.0f}%: t = {}, {} infected ({:.1f}%)'.format(100 * t / TMAX, t, s.sum(), 100 * s.sum() / N))
        print(f'{t},{new_vals[-1]},{s.sum()}')
        n_I = s.sum()                                   # current number of infected individuals
        num_new = 0
        for n in range(N):                                  # loop over all agents
            if s[n] == 0:                                   # susceptible individual can get infected
                imm[n] = np.exp(-(t - healthy_since[n]) / T_IMMUNITY)   # current agent immunity
                beta_n = beta * (1 - imm[n])                # individual (one-to-one) infection probability for this individual
                p_I = 1 - np.exp(-beta_n * n_I)             # probability of getting infected [we assume here that beta_n is very small, which it is when everyone is interacting with everyone; (1 - beta_n) is then approximately exp(-beta_n)]
                if np.random.rand() < p_I:
                    s[n] = 1                                # the individual gets infected
                    num_new += 1
                    if t >= T_EQUILIBRIUM:
                        infected_when.append(int(t - healthy_since[n]))
            else:                                           # infected individuals can recover
                if np.random.rand() < 1 / T_RECOVERY:       # Poisson recovery
                    s[n] = 0                                # the individual recovers
                    healthy_since[n] = t                    # store when the last recovery took place
        tot_vals.append(s.sum())
        new_vals.append(num_new)
    print('simulation finished: {:.0f} infected on average (last 100 steps)\n'.format(np.mean(tot_vals[-100:])))
    return tot_vals, new_vals, healthy_since, infected_when           # return the interesting results back to the main loop


N, mult = 10000, 0.2
np.random.seed(0)
tmp, tmp2, tmp3 = pd.DataFrame(), pd.DataFrame(), pd.DataFrame()
beta = mult / N                                             # 1-to-1 infection probability
for run in range(5):                                        # do 5 independent runs
    print('N = {}, beta * N = {:.1f}, run {}'.format(N, beta * N, run))
    nI_vals, newI_vals, healthy_since, infected_when = one_run(beta, N)
    tmp['run_{}'.format(run)] = TMAX - np.array(healthy_since, dtype = int)
    tmp2['run_{}'.format(run)] = nI_vals
    tmp3['run_{}'.format(run)] = newI_vals
# tmp.to_csv('healthy_since_N_{}.dat'.format(N), sep = '\t', index = False)
tmp2.index.name = 'time'
tmp2.to_csv('xxx-nI-N_{}_mult_{}.dat'.format(N, mult), sep = '\t')
tmp3.index.name = 'time'
tmp3.to_csv('xxx-newI-N_{}_mult_{}.dat'.format(N, mult), sep = '\t')
