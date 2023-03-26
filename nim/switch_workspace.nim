import osproc
import json
import strformat
import os

let jsonString = execProcess("i3-msg -t get_workspaces")
let jsonNode = parseJson(jsonString)

func inc_workspace(workspace_numbers : seq[int], focused_workspace_idx : int) : int =
  workspace_numbers[(focused_workspace_idx + 1) mod workspace_numbers.len]

func dec_workspace(workspace_numbers : seq[int], focused_workspace_idx : int) : int =
  workspace_numbers[(focused_workspace_idx + workspace_numbers.len - 1) mod workspace_numbers.len]




var focused_workspace_idx = 0
var workspace_numbers : seq[int] = @[]
var idx = 0

for n in jsonNode.items():
  let workspace_number = n["num"].getInt()
  workspace_numbers.add(workspace_number)
  if n["focused"].getBool() == true:
    focused_workspace_idx = idx
    echo &"Focssed workspace: {workspace_number}"
  idx += 1

let new_workspace =
  if paramStr(1) == "next":
    inc_workspace(workspace_numbers, focused_workspace_idx)
  elif paramStr(1) == "previous":
    dec_workspace(workspace_numbers, focused_workspace_idx)
  else:
    0

discard execCmd(&"i3-msg -t command workspace number {new_workspace}")
