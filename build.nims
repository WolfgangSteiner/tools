#!/usr/bin/env -S nim  
#
import strformat

#mode = ScriptMode.Verbose


for file in ["dim", "todo", "battery_watch", "wetter", "arstechnica", "udacity", "youtube", "gmail", "i3menu", "ff", "switch_workspace"]:
  echo file
  exec &"nim c -d:release --hints:off --opt:size -o:~/bin/{file} {file}.nim"

for file in ["status"]:
  echo file
  exec &"nim c -d:release --hints:off --threads:on --opt:size -o:~/bin/{file} {file}.nim"
