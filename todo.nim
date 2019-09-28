import os
import osproc
import parseopt
import sequtils
import uuids
import strutils
import strformat
import times

const TODO_DIR : string = ".todo"


proc write_help() =
  echo "todo [new|list|done]"
  quit(QuitFailure)

var p = initOptParser("")

proc generate_id() : string = $genUUID()
proc current_date() : string = now().format("yyyy-MM-dd")

type TodoStatus = enum tsOpen = "open", tsResolved = "resolved"

type TodoItem = object
  id*: string
  title* : string
  tags*: string
  date*: string
  priority*: float
  status*: TodoStatus
  description*: string

func `$`(item: TodoItem) : string =
  return &"""title: {item.title}
date: {item.date}
tags: {item.tags}
priority: {item.priority}
status: {item.status}

{item.description}"""

proc todo_item(title="", description="", tags="", priority=0.0) : TodoItem =
  TodoItem(id:generate_id(), title:title, description:description, priority:priority, status:tsOpen, date:current_date(), tags:tags)

func shortId(item: TodoItem) : string =
    return item.id.split("-")[0]

proc write(item: TodoItem) =
  create_dir(TODO_DIR)
  write_file(join_path(TODO_DIR, item.id & ".todo"), $item)


proc read_item(fn: string) : TodoItem =
  let id = fn.splitFile().name
  let r = read_file(fn).split("\n\n")
  var item = TodoItem(id:id)
  item.description = if len(r) == 1: "" else: r[1..<r.len].join("\n\n")
  for l in r[0].splitLines():
    let a = l.split(": ", maxsplit=1)
    let key = a[0]
    let val = a[1]
    case key:
      of "title":
        item.title = val
      of "date":
        item.date = val
      of "priority":
        item.priority = parseFloat(val)
      of "tags":
        item.tags = val
      of "status":
        item.status = parseEnum[TodoStatus](val)
      else:
        echo &"Skipping file {fn}: unknown field {key}."
  return item
    
  #for l in s.splitLines():

proc list_items(files: seq[string], only_open = false) =
  for fn in files:
    let item = read_item(fn)
    if only_open and item.status != tsOpen:
      continue
    echo &"{item.shortId}  {item.title}"


proc list_items(only_open = true) =
  let files = toSeq(walkFiles(join_path(TODO_DIR, "*.todo")))
  if len(files) == 0:
    echo "No todos found."
  else:
    list_items(files, only_open)

proc filename_for_id(id: string) : string =
  let files = toSeq(walkFiles(join_path(TODO_DIR, id & "*.todo")))
  if len(files) == 0:
    echo &"Could not find any todos for id {id}."
    quit()
  elif len(files) == 1:
    return files[0]
  else:
    echo &"More than one todo found for id {id}:"
    list_items(files, only_open=false)
    quit()


proc item_for_id(id: string) : TodoItem = read_item(filename_for_id(id))

proc resolve_item(id: string) =
  var item = item_for_id(id)
  if item.status == tsOpen:
    echo &"Resolved {item.shortId}: \"{item.title}\""
    item.status = tsResolved
    item.write()
  else:
    echo &"Already resolved {item.shortId}: \"{item.title}\""

proc edit_item(id: string) =
  let fn = filename_for_id(id)
  discard execCmd(&"vim {fn}")


################################################################################
## MAIN
################################################################################
var cmd_seq : seq[string]
var title, description : string
var priority : float = 0.0


for kind, key, val in p.getopt():
  case kind:
    of cmdArgument:
      cmd_seq.add(key)
    of cmdLongOption, cmdShortOption:
      case key:
        of "help", "h": writeHelp()
        of "title", "t": title = val
        of "priority", "p": priority = parseFloat(val)
        of "description", "d": description = val
    of cmdEnd: assert(false) # cannot happen
  

if len(cmd_seq) == 0:
  cmd_seq.add("list")


case cmd_seq[0]:
  of "create", "c":
    if len(cmd_seq) > 1:
      title = cmd_seq[1]
    var item = todo_item(title=title, priority=priority, description=description)
    item.write()
  of "list", "l":
    list_items()
  of "resolve", "r":
    let id = cmd_seq[1]
    resolve_item(id)
  of "edit", "e":
    let id = cmd_seq[1]
    edit_item(id)
  else:
    writeHelp()
    quit(QuitFailure)
