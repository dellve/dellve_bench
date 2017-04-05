import gpu_info
import math

def calculate_nchw(mem):
    c = 1
    n = 75

    mem = mem/c
    mem = mem/n

    h = int(math.sqrt(mem))
    w = mem/h

    return n,c,h,w

def calculate_nchw_forward(device_id, mem_util):
    # TODO: Limit Mem_Util to .5 < mem_util < .95
    overhead = 71*1000*1000
    mem = int(gpu_info.get_total_mem(device_id) * mem_util)
    mem -= overhead
    mem /= 8

    return calculate_nchw(mem)

def calculate_nchw_activation_backward(device_id, mem_util):
    # TODO: Limit Mem_Util
    # Overhead counted twice so half it from forward
    overhead = (71*1000*1000)/2
    mem = int((gpu_info.get_total_mem(device_id)/2) * mem_util)
    mem -= overhead
    mem /= 8

    return calculate_nchw(mem)

def calculate_nchw_softmax_backward(device_id, mem_util):
    # TODO: Limit Mem_Util
    # Overhead counted roughly 2/3 (calc 2611/1763 =~ 3/2)
    overhead = ((71*1000*1000)*2)/3
    mem = int(((gpu_info.get_total_mem(device_id)*2)/3) * mem_util)
    mem -= overhead
    mem /= 8

    return calculate_nchw(mem)