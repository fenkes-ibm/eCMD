#!/usr/bin/env python
from fileinput import input
import re

# Read in a Python module generated by SWIG, extract all parameter names starting with o_
# and output a list of SWIG %apply statements to map them to output parameters.

names = set()

def output(totype, fromtype):
    print("%%apply %s { %s };" % (totype, ", ".join(fromtype + name for name in sorted(names))))

for l in input():
    for word in re.split(r"\W+", l):
        if word.startswith("o_"):
            names.add(word)

output("int &OUTPUT", "enum SWIGTYPE &")
output("int &OUTPUT", "uint32_t &")
output("int &OUTPUT", "uint64_t &")
output("std::string &OUTPUT", "std::string &")
