#! /usr/bin/env python
import re, os, sys

dry_run = False
shorten_dates = False

if len(sys.argv) > 1:
    if sys.argv[1] in ("-n", "--dry-run"):
        dry_run = True
        sys.argv.pop(1)
    elif sys.argv[1] in ("--shorten-dates"):
        shorten_dates = True
        sys.argv.pop(1)

for filename in os.listdir("."):
    old_path = os.path.join(".", filename)
    
    # Check if it's a file (not a directory)
    if True or os.path.isfile(filename):
        nfn = filename.replace(' - ', '_')
        nfn = re.sub(r'([a-z])([A-Z])', r'\1_\2', nfn)
        nfn = re.sub(r'(\d{4})-(\d{2})(?:-(\d{2}))?', r'\1\2\3', nfn)
        if shorten_dates:
            nfn = re.sub(r'20(\d{2})(\d{2})(\d{2})?', r'\1\2\3', nfn)
        nfn = nfn.lower() 
        nfn = re.sub(r'[ -]', '_', nfn)
        nfn = re.sub(r'__+', '_', nfn)
        nfn = re.sub(r'[^a-zäöü0-9_.]', '', nfn)
        if filename != nfn:
            if dry_run:
                print(f"'{filename}' -> '{nfn}'")
            else:
                try:
                    os.rename(filename, nfn)
                    print(f"'{filename}' -> '{nfn}'")
                except Exception as e:
                    print(f"Failed to rename '{filename}': {e}")
