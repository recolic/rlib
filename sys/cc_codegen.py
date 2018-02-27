#!/bin/env python3

def genDefList(idarr):
    s = '#if'
    cter = 1
    for i in idarr:
        s += ' defined(' + i + ')'
        if cter != len(idarr):
            s += ' ||'
        cter += 1
    return s

print('// Generated by cc_codegen.py. Do not edit it by hand.')

with open("cc_list") as fd:
    osarr=fd.read().split('\n')
    for i in osarr:
        if i == '':
            continue
        iarr=i.split(' ')
        if len(iarr) < 2:
            continue
        print('#ifndef RLIB_COMPILER_ID')
        print(genDefList(iarr[:-1:]))
        print('#define RLIB_COMPILER_ID', iarr[-1])
        print('#endif')
        print('#endif')
        print('')

print('#ifndef RLIB_COMPILER_ID')
print('#define RLIB_COMPILER_ID UNKNOWN')
print('#endif')

