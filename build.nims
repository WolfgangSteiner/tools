#!/usr/bin/env -S nim  
#
import strformat
from os import nil
#mode = ScriptMode.Verbose

let bin_path = os.expandTilde("~/bin")

for file in ["dim", "todo", "battery_watch", "i3menu"]:
  echo file
  exec &"nim c -d:release --hints:off --opt:size -o:{bin_path}/{file} nim/{file}.nim"

for file in ["status"]:
  echo file
  exec &"nim c -d:release --hints:off --threads:on --opt:size -o:{bin_path}/{file} {file}.nim"
