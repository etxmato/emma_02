
# Simple Emma 02 Help File Validator
# Usage: python validate_help.py <folder>

import os, re, sys

folder=sys.argv[1]

for root,_,files in os.walk(folder):
    for f in files:
        if f.endswith((".htd",".html",".htm")):
            p=os.path.join(root,f)
            t=open(p,encoding="utf-8",errors="ignore").read()

            if re.search(r"<p>\s*<ul",t,re.I):
                print("UL inside P:",p)

            if re.search(r"<li>\s*\n",t):
                print("Multiline LI:",p)

            if re.search(r"<img",t):
                print("Lowercase img tag:",p)

            if "emulator running" in t.lower():
                print("Terminology issue:",p)
