# File:     organizer.py
# Author:   Andrew Woska (agwoska@bu.edu)
# Date:     04/23/2024

import pandas as pd
import numpy as np
import os
import sys
from re import search


def extract_number(s):
    match = search(r'\d+', s)
    if match:
        return int(match.group(0))
    else:
        return None

def get_data(fp):
    """
    Function to read in the data from the stats txt file
    """
    i = 0 # to reduce regex calls
    prompts = [
        'board.cache_hierarchy.ruby_system.l1_controllers0.L1Dcache.m_demand_misses',
        'board.cache_hierarchy.ruby_system.l1_controllers0.L1Dcache.m_demand_accesses',
        'board.cache_hierarchy.ruby_system.l1_controllers0.L1Icache.m_demand_misses',
        'board.cache_hierarchy.ruby_system.l1_controllers0.L1Icache.m_demand_accesses',
        'board.cache_hierarchy.ruby_system.l1_controllers1.L1Dcache.m_demand_misses',
        'board.cache_hierarchy.ruby_system.l1_controllers1.L1Dcache.m_demand_accesses',
        'board.cache_hierarchy.ruby_system.l1_controllers1.L1Icache.m_demand_misses',
        'board.cache_hierarchy.ruby_system.l1_controllers1.L1Icache.m_demand_accesses',
        'board.cache_hierarchy.ruby_system.l2_controllers0.L2cache.m_demand_misses',
        'board.cache_hierarchy.ruby_system.l2_controllers0.L2cache.m_demand_accesses',
        'board.cache_hierarchy.ruby_system.l2_controllers1.L2cache.m_demand_misses',
        'board.cache_hierarchy.ruby_system.l2_controllers1.L2cache.m_demand_accesses',
        'board.processor.switch0.core.cpi'
    ]
    data = []
    data.append(fp)
    with open(fp, 'r') as f:
        for line in f:
            if i > len(prompts)-1:
                break
            if line.startswith(prompts[i]):
                if i == len(prompts) - 1:
                    data.append(float(line[len(prompts[i]):].split()[0]))
                else:
                    data.append(int(line[len(prompts[i]):].split()[0]))
                i += 1
    return data


files = [
    'stats/500.perlbench_r_1.stats.txt',
    'stats/500.perlbench_r_2.stats.txt',
    'stats/500.perlbench_r_4.stats.txt',
    'stats/502.gcc_r_1.stats.txt',
    'stats/502.gcc_r_2.stats.txt',
    'stats/502.gcc_r_4.stats.txt',
    'stats/505.mcf_r_1.stats.txt',
    'stats/505.mcf_r_2.stats.txt',
    'stats/505.mcf_r_4.stats.txt',
    'stats/520.omnetpp_r_1.stats.txt',
    'stats/520.omnetpp_r_2.stats.txt',
    'stats/520.omnetpp_r_4.stats.txt',
    'stats/523.xalancbmk_r_1.stats.txt',
    'stats/523.xalancbmk_r_2.stats.txt',
    'stats/523.xalancbmk_r_4.stats.txt',
    'stats/525.x264_r_1.stats.txt',
    'stats/525.x264_r_2.stats.txt',
    'stats/525.x264_r_4.stats.txt',
    'stats/531.deepsjeng_r_1.stats.txt',
    'stats/531.deepsjeng_r_2.stats.txt',
    'stats/531.deepsjeng_r_4.stats.txt',
    'stats/541.leela_r_1.stats.txt',
    'stats/541.leela_r_2.stats.txt',
    'stats/541.leela_r_4.stats.txt',
    'stats/548.exchange2_r_1.stats.txt',
    'stats/548.exchange2_r_2.stats.txt',
    'stats/548.exchange2_r_4.stats.txt',
    'stats/557.xz_r_1.stats.txt',
    'stats/557.xz_r_2.stats.txt',
    'stats/557.xz_r_4.stats.txt'
]

file_data = []
for file in files:
    file_data.append(get_data(file))
    
df = pd.DataFrame(file_data, columns=['name', 
            'l1d0_misses', 'l1d0_accesses', 'l1d1_misses', 'l1d1_accesses', 
            'l1i0_misses', 'l1i0_accesses', 'l1i1_misses', 'l1i1_accesses',
            'l20_misses', 'l20_accesses', 'l21_misses', 'l21_accesses', 'cpi'])
# df['l1d0_miss_rate'] = df['l1d0_misses'] / df['l1d0_accesses']
# df['l1d1_miss_rate'] = df['l1d1_misses'] / df['l1d1_accesses']
# df['l1d_miss_rate'] = (df['l1d0_misses'] + df['l1d1_misses']) / (df['l1d0_accesses'] + df['l1d1_accesses'])
# df['l1i_miss_rate'] = (df['l1i0_misses'] + df['l1i1_misses']) / (df['l1i0_accesses'] + df['l1i1_accesses'])
df['l1_miss_rate'] = (df['l1d0_misses'] + df['l1d1_misses'] + df['l1i0_misses'] + df['l1i1_misses']) / (df['l1d0_accesses'] + df['l1d1_accesses'] + df['l1i0_accesses'] + df['l1i1_accesses'])
# df['l1i0_miss_rate'] = df['l1i0_misses'] / df['l1i0_accesses']
# df['l20_miss_rate'] = df['l20_misses'] / df['l20_accesses']
# df['l21_miss_rate'] = df['l21_misses'] / df['l21_accesses']
df['l2_miss_rate'] = (df['l20_misses'] + df['l21_misses']) / (df['l20_accesses'] + df['l21_accesses'])
df['ipc'] = 1 / df['cpi']

# only name, cpi, l1 and l2 miss rates
df = df[['name', 'cpi', 'l1_miss_rate', 'l2_miss_rate', 'ipc']]

df.to_csv('organizer.csv', index=False)
