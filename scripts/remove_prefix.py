#! /usr/bin/env python

import os, sys
import shutil

for fn in sys.argv[1:]:
    if "_" in fn:
        new_fn = "_".join(fn.split("_")[1:])
        print(f"{fn} -> {new_fn}")
        shutil.move(fn, new_fn)
