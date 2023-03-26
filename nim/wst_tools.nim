import os, strformat, strutils, sequtils, osproc
import nimclipboard/libclipboard


const BROWSER = "firefox"


proc browser_name() : string =
  if os.existsEnv("BROWSER"):
    return os.getEnv("BROWSER")
  else:
    return BROWSER


proc shell_cmd*(cmd: string) =
  discard execShellCmd(&"{cmd} > /dev/null")


proc starts_with_any*(str: string, str_arr: openArray[string]) : bool =
  str_arr.any(func (s:string) : bool = str.starts_with(s))


func normalize_url*(url: string) : string =
  if not url.starts_with_any(["https://", "http://"]):
    "https://" & url
  else:
    url


proc web_app*(url: string) =
  if browser_name() == "chrome":
    shell_cmd &"chrome --new-window {normalize_url url} &"
  elif browser_name() == "firefox":
    shell_cmd &"firefox --new-window {normalize_url url} &"
  else:
    shell_cmd &"{browser_name()} {normalize_url url} &"


proc private_web_app*(url: string) =
    shell_cmd &"firefox --private-window {normalize_url url} &"


proc open_url*(url: string) =
  shell_cmd &"{BROWSER} {normalize_url url}"


proc get_clipboard*() : string =
  var cb = clipboard_new(nil)
  let str = $cb.clipboard_text()
  clipboard_free(cb)
  return str


proc get_selection_clipboard*() : string =
  var cb = clipboard_new(nil)
  let str = $cb.clipboard_text_ex(nil, LCB_PRIMARY)
  clipboard_free(cb)
  return str


proc dmenu*(prompt: string, str_arr: varargs[string]) : string =
  let args_str = "-e \"" & str_arr.join("\\n") & "\""
  execProcess("echo " & args_str & &" | dmenu -i -p \"{prompt}\"").strip(leading=false)
