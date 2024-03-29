import algorithm
import os
import osproc
import parseopt
import sequtils
import uuids
import re
import strutils
import strformat
import terminal
import times

################################################################################
## GLOBALS
################################################################################
const TODO_DIR : string = ".todo"
let TERM_WIDTH : int = terminalWidth()


################################################################################
## GIT
################################################################################
proc gitHasRepository() : bool =
  for l in execProcess("git status --porcelain").splitLines():
    if l.match(re"^fatal: not a git repository"):
      return false
  return true

proc assertGitRepository() = doAssert(gitHasRepository(), "No git respository found.")

proc gitAdd(path:string) =
  assertGitRepository()
  discard execCmd(&"git add {path}")

proc gitRemove(path:string) =
  assertGitRepository()
  discard execCmd(&"git rm {path}")

proc gitCommit(msg:string) =
  assertGitRepository()
  discard execCmd(&"git commit -m \"{msg}\"")

proc gitHasStagedFiles() : bool =
  assertGitRepository()
  for l in execProcess("git status --porcelain").splitLines():
    if l.match(re"^[AM]"):
      return true
  return false

proc gitIsFileModified(fn:string) : bool =
  assertGitRepository()
  for l in execProcess("git status --porcelain").splitLines():
    if l.match(re"^( M|M |MM)\s+") and l.contains(fn):
      return true
  return false

proc gitInit() =
  discard execCmd("git init")


################################################################################
## PRETTY PRINTNG
################################################################################
func separator(char='-', width=TERM_WIDTH) : string = "#" & char.repeat(width-1) & "\n"
func separator1(width=TERM_WIDTH) : string = separator('#', width=width)
func separator2(width=TERM_WIDTH) : string = separator('=', width=width)
func separator3(width=TERM_WIDTH) : string = separator('-', width=width)

func format_header(s: string, char='-', width=TERM_WIDTH) : string =
  separator(char, width) & "# " & s & "\n" & separator(char, width)


proc write_help() =
  echo "todo [new|list|done]"
  quit()


################################################################################
## ERROR HANDLING
################################################################################
proc check(condition:bool, message:string) =
  if not condition:
    echo message
    quit()

proc check_false(condition:bool, message:string) =
  if condition:
    echo message
    quit()


template check_implies(cond1:untyped, cond2:untyped, message:string) =
  check((cond1 and cond2) or not cond1, message)


proc generate_id() : string = ($genUUID()).replace("-","")[0..<16]
proc current_date() : string = now().format("yyyy-MM-dd'T'HH:mm:ss")

type TodoStatus = enum tsOpen = "open", tsResolved = "resolved"

type TodoItem = object
  id*: string
  title* : string
  tags*: string
  date*: string
  priority*: float
  status*: TodoStatus
  description*: string

func shortId(item: TodoItem) : string = return item.id[0..<7]

proc format_short(item: TodoItem) : string = 
  let separator_string = "  "
  let id_string = item.shortId
  let title_string =
    if len(id_string) + len(separator_string) + len(item.title) > TERM_WIDTH:
      let max_length = TERM_WIDTH - len(id_string) - len(separator_string) - 2
      item.title[0..<max_length] & ".."
    else:
      item.title
  return id_string & separator_string & title_string


func `$`(item: TodoItem) : string =
  return &"""title: {item.title}
date: {item.date}
tags: {item.tags}
priority: {item.priority}
status: {item.status}

{item.description}"""



proc format_verbose(item: TodoItem) : string =
  var r = separator3()
  r.add(&"# {item.shortId}  {item.title}\n")
#  r.add("#\n")
#  r.add(&"#          date:{item.date}  priority:{item.priority}  status:{item.status}  tags:{item.tags}\n")
  r.add(separator3())
  r.add(&"{item.description}")
  return r

func format_title(item:TodoItem, quoted=true) : string =
  if quoted:
    &"\"{item.title}\""
  else:
    &"{item.title}"


func format_message(item: TodoItem, quoted:bool=true) : string =
  &"{item.shortId}: {item.format_title(quoted)}"



proc format(item: TodoItem, verbose=false) : string =
  if verbose:
    item.format_verbose()
  else:
    item.format_short()


proc todo_item(title="", description="", tags="", priority=0.0) : TodoItem =
  TodoItem(id:generate_id(), title:title, description:description, priority:priority, status:tsOpen, date:current_date(), tags:tags)


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
    

proc items_for_id(id: string) : seq[TodoItem] =
  var r: seq[TodoItem]
  for fn in walkFiles(join_path(TODO_DIR, id & "*.todo")):
    r.add(read_item(fn))
  return r


proc items_for_ids(ids: seq[string]) : seq[TodoItem] =
  var r : seq[TodoItem]
  for id in ids:
    r.add(items_for_id(id))
  return r


proc load_all_items() : seq[TodoItem] =
  var r : seq[TodoItem]
  for fn in walkFiles(join_path(TODO_DIR, "*.todo")):
    r.add(read_item(fn))
  return r


proc list_items(items: seq[TodoItem], only_open=true, verbose=false) =
  for item in items:
    if only_open and item.status != tsOpen:
      continue
    echo item.format(verbose=verbose)


proc list_items(ids: seq[string], only_open = true) =
  var items =
    if len(ids) > 0:
      items_for_ids(ids)
    else:
      load_all_items()
  if len(items) == 0:
    echo "No todos found."
  else:
    items = algorithm.sorted(items, proc(a, b:TodoItem): int = cmp(a.date,b.date))
    list_items(items, only_open, verbose=len(ids)>0)


proc item_for_id(id: string) : TodoItem =
  let items = toSeq(items_for_id(id))
  if len(items) == 0:
    echo &"Could not find any todos for id {id}."
    quit()
  elif len(items) == 1:
    return items[0]
  else:
    echo &"More than one todo found for id {id}:"
    list_items(items, only_open=false)
    quit()


proc filename_for_id(id: string) : string =
  discard item_for_id(id)
  let files = toSeq(walkFiles(join_path(TODO_DIR, &"{id}*.todo")))
  doAssert(len(files) == 1)
  return files[0]


proc gitAdd(item:TodoItem) =
  gitAdd(filename_for_id(item.id))

proc gitCommit(item:TodoItem, title:string) =
  gitCommit(&"{title} {item.format_message(quoted=false)}.")


proc resolve_items(ids:seq[string], commit:bool=false) =
  check_implies(commit, gitHasRepository(), "No git repository found!")

  check_implies(commit, gitHasStagedFiles(), "Please stage files before trying to resolve --commit.")
  check_implies(not commit, not (gitHasRepository() and gitHasStagedFiles()),
    "Can't resolve todo: non-empty git staging.")
 
  var resolved_ids : seq[string] = @[]
  var messages : seq[string] = @[]

  for id in ids:
    var item = item_for_id(id)
    if item.status == tsOpen:
      resolved_ids.add(id)
    else:
      echo &"Already resolved {item.format_message}. Aborting."
  
  for id in resolved_ids:
    var item = item_for_id(id)
    item.status = tsResolved
    item.write()
    messages.add(item.title)
    if gitHasRepository():
      item.gitAdd()
    echo &"Resolved {item.format_message}."

  if gitHasRepository():
    gitCommit(messages.join(", "))


proc create_item(title: string, description="", priority = 0.0) =
  var item = todo_item(title=title, priority=priority, description=description)
  check_implies(gitHasRepository(), not gitHasStagedFiles(), "Can't create todo: non-empty git staging.")
  item.write()
  if gitHasRepository():
    item.gitAdd()
    item.gitCommit("Add")
  echo &"Created item {item.format_message}"


proc edit_item(id: string) =
  check_implies(gitHasRepository(), not gitHasStagedFiles(), "Can't edit todo due to files in git staging.")
  let fn = filename_for_id(id)
  discard execCmd(&"vim {fn}")
  let item = read_item(fn)
  if gitHasRepository() and gitIsFileModified(fn):
    item.gitAdd()
    item.gitCommit("Update")


proc delete_item(id: string) =
  check_implies(gitHasRepository(), not gitHasStagedFiles(), "Can't delete todo due to files in git staging.")
  let fn = filename_for_id(id)
  let item = item_for_id(id)
  if gitHasRepository():
    gitRemove(fn)
    item.gitCommit(&"Remove")
  else:
    removeFile(fn)
  echo &"Deleted item {item.format_message}." 


proc check_cmd(command : string, legal_commands: seq[string], option: string) =
  if not legal_commands.contains(command):
    echo &"Invalid option {option} for command {command}"
    quit()


proc check_id_argument(cmd: string, ids: seq[string]) =
  if len(ids) < 1:
    echo &"You need to supply a valid id for command {cmd}"
    quit()


proc is_valid_id(id: string) : bool =
  return matchLen(id, re"[0-9a-f]+") == len(id)


################################################################################
## MAIN
################################################################################
var cmd_seq : seq[string]
var title, description : string
var priority = 0.0
var only_open = true
var cmd : string = "list"
var ids : seq[string]
var commit : bool = false
var no_commit: bool = false

var p = initOptParser("")

for kind, key, val in p.getopt():
  case kind:
    of cmdArgument:
      case key:
        of "resolve", "r" : cmd = "resolve"
        of "edit", "e" : cmd = "edit"
        of "list", "l" : cmd = "list"
        of "create", "c" : cmd = "create"
        of "delete", "d" : cmd = "delete"
        else:
          if cmd == "create":
            title = key
          elif not is_valid_id(key):
            echo &"Invalid id {key}"
            quit()
          ids.add(key)
    of cmdLongOption, cmdShortOption:
      case key:
        of "help", "h": writeHelp()
        of "title", "t": title = val
        of "priority", "p": priority = parseFloat(val)
        of "description", "d": description = val
        of "commit",:
          check_cmd(cmd, @["resolve"], "commit")
          commit = true
        of "no-commit":
          no_commit = true
        of "all", "a":
          check_cmd(cmd, @["list"], "all")
          only_open = false
    of cmdEnd: assert(false) # cannot happen
  

if len(cmd_seq) == 0:
  cmd_seq.add("list")


case cmd:
  of "create":
    check(len(title) > 0, "Please provide a title for the new todo item.")
    create_item(title=title, priority=priority, description=description)
  of "list":
    list_items(ids, only_open=only_open)
  of "resolve":
    check_id_argument("resolve", ids)
    resolve_items(ids, commit=commit)
  of "edit", "e":
    check_id_argument("edit", ids)
    for id in ids:
      edit_item(id)
  of "delete", "d":
    check_id_argument("delete", ids)
    for id in ids:
      delete_item(id)
  else:
    writeHelp()
    quit(QuitFailure)
