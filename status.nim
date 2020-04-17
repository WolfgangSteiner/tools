import os
import osproc
import wst_tools
import strformat, strutils
import sequtils
import json
import times
import system


func grep(sa: seq[string], s:string) : seq[string] = sa.filter(proc(l:string) : bool = s in l)
proc shellCommand(c:string) : seq[string] = execProcess(c).strip(chars = {'\n'}).splitLines()
proc readLines(fn: string) : seq[string] = readFile(fn).strip(chars = {'\n'}).splitLines()
func getColumn(s: string, n: int) : string = s.strip().splitWhiteSpace()[n]
func head(sa: seq[string], offset: int = 0) : string = sa[offset]
func tail(sa: seq[string], offset: int = 0) : string = sa[sa.high - offset]


const COLOR_RED = "#ff0000"
const COLOR_GREEN = "#00ff00"
const COLOR_YELLOW = "#ffbf00"
const COLOR_TEXT = "#ffffff"
const MAX_PING = 1.0e9
const MAX_MISSED_PINGS = 10
var missed_pings : int = 0
var ping_value : float = MAX_PING

var measure_ping_thread : Thread[void]

proc measure_ping() {.thread.} =
  while true:
    let r = shellCommand("ping -c 1 -W 1 8.8.8.8")
    if "unreachable" in r[0] or len(r.grep("rtt")) == 0:
      missed_pings = min(MAX_MISSED_PINGS, missed_pings + 1)
      if missed_pings == MAX_MISSED_PINGS:
        ping_value = MAX_PING
    else:
      let new_value = r.grep("rtt").head().getColumn(3).split('/')[0].parseFloat()
      missed_pings = max(0, missed_pings - 1)
      const a = 0.1
      if ping_value == MAX_PING:
        ping_value = new_value
      else:
        ping_value = new_value * a + ping_value * (1 - a)
    os.sleep(500)


func fmtStatus(s: string, c: string = COLOR_TEXT): JsonNode =
  %* {"full_text": s, "color": c }


proc ip() : JsonNode =
  shellCommand("ip address").grep("wlp").tail().strip().getColumn(1).split('/')[0].fmtStatus()


proc wifi_quality() : float =
  const NOISE_FLOOR_DBM = -100.0
  const SIGNAL_MAX_DBM = -50.0
  var xbm = readLines("/proc/net/wireless").tail().getColumn(3).parseFloat()
  xbm = max(NOISE_FLOOR_DBM, min(SIGNAL_MAX_DBM, xbm))
  #let quality = int(100 - 70 * ((SIGNAL_MAX_DBM - xbm) / (SIGNAL_MAX_DBM - NOISE_FLOOR_DBM)) + 0.5);
  2 * (xbm + 100) + 0.5


proc has_wifi_connection() : bool =
  len(readLines("/proc/net/wireless").grep("wlp")) > 0


proc wifi_ssid() : string =
  shellCommand("nmcli con show --active").grep("wifi").head().getColumn(0)


func percent_bar(percent: float, segments: int = 5, symbols: string = " -+*#") : string =
  let num_segments = percent / 100 * float(segments)
  result = "["
  for i in 1..segments:
    let residual = min(max(0, num_segments + 1 - float(i)), float(len(symbols) - 1))
    let idx = min(len(symbols) - 1, max(0, int(residual * float(len(symbols)))))
    result.add(symbols[idx])

  result.add("]")


proc wifi() : seq[JsonNode] =
  if has_wifi_connection():
    const output =
      [("[+++]", COLOR_GREEN),
       ("[++ ]", COLOR_GREEN),
       ("[+  ]", COLOR_YELLOW),
       ("[   ]", COLOR_RED)]

#const output =
#  [("[++O++]", COLOR_GREEN),
#   ("[ +O+ ]", COLOR_GREEN),
#   ("[  O  ]", COLOR_YELLOW),
#   ("[  .  ]", COLOR_RED)]

    let idx = 3 - min(3, int(wifi_quality() / 25))
    let (_, color) = output[idx]
    let indicator = percent_bar(wifi_quality(), segments=3, symbols=" -+*#")
    @[(wifi_ssid()&indicator).fmtStatus(color)]

  else:
    @["no connection".fmtStatus(COLOR_RED)]


proc spotify(): seq[JsonNode] =
  let r = shellCommand("dbus-send --print-reply --dest=org.mpris.MediaPlayer2.spotify /org/mpris/MediaPlayer2 org.freedesktop.DBus.Properties.Get string:org.mpris.MediaPlayer2.Player string:Metadata")
  return @[JsonNode()]


proc df() : JsonNode =
  let s = shellCommand("df -h /").tail().getColumn(3)
  let unit = s[^1]
  let color =
    if unit != 'G':
      COLOR_RED
    else:
      let size = s[0..^2].parseFloat()
      if size >= 8.0:
        COLOR_GREEN
      elif size >= 2.0:
        COLOR_YELLOW
      else:
        COLOR_RED

  s.fmtStatus(color)


proc clock() : JsonNode =
  let hour = now().hour
  let minute = now().minute
  let color =
    if hour < 6 or hour >= 23:
      COLOR_RED
    elif hour >= 22:
      COLOR_YELLOW
    else:
      COLOR_TEXT

  now().format("HH:mm").fmtStatus(color)

proc display_ping() : JsonNode =
  let color =
    if ping_value == MAX_PING or missed_pings >= 8:
      COLOR_RED
    elif ping_value > 50.0 or missed_pings >= 4:
      COLOR_YELLOW
    else:
      COLOR_GREEN

  let ping_str =
    if ping_value == MAX_PING:
      "-----"
    else: fmt"{int(ping_value + 0.5)} ms"

  ping_str.fmtStatus(color)


proc display_cpu_load() : JsonNode =
  let load = readfile("/proc/loadavg").strip().getColumn(0)
  fmt"CPU {load}".fmtStatus()


proc is_battery_discharging() : bool =
  readfile("/sys/class/power_supply/BAT0/status").strip() == "Discharging"


proc is_charger_connected() : bool =
  readfile("/sys/class/power_supply/AC/online").strip().parseInt() == 1


proc battery_charge_full() : float =
  readfile("/sys/class/power_supply/BAT0/charge_full").strip().parseFloat()


proc battery_charge_now() : float =
  readfile("/sys/class/power_supply/BAT0/charge_now").strip().parseFloat()


proc battery_current_now() : float =
  try:
    const fn = "/sys/class/power_supply/BAT0/current_now"
    readfile(fn).strip().parseFloat()
  except IOError:
    0.0


proc battery_percent() : float =
  battery_charge_now() / battery_charge_full() * 100



var avg_current : float = 1.0e9
proc battery_current_avg() : float =
  if avg_current == 1.0e9:
    avg_current = battery_current_now()
  else:
    const a = 0.05
    avg_current += a * (battery_current_now() - avg_current)
  return avg_current


proc battery_time_remaining() : float =
  battery_charge_now() / battery_current_avg()


proc charge_time_remaining() : float =
  let remaining_charge = battery_charge_full() - battery_charge_now()
  remaining_charge / battery_current_avg()


func format_time(t: float) : string =
  let hours = int(t)
  let minutes = int((t - float(hours)) * 60)
  fmt"{hours:02d}:{minutes:02d}"


proc display_battery() : JsonNode =
  let percent = battery_percent()
  if is_battery_discharging():
    let color =
      if percent > 20:
        COLOR_GREEN
      elif percent > 10:
        COLOR_YELLOW
      else:
        COLOR_RED
    fmt"BAT {percent:.2f}% ({format_time(battery_time_remaining())})".fmtStatus(color)
  else:
    if percent == 100.0:
      fmt"CHR {percent:.2f}%".fmtStatus(COLOR_GREEN)
    else:
      fmt"CHR {percent:.2f}% ({format_time(charge_time_remaining())})".fmtStatus(COLOR_YELLOW)


################################################################################
# MAIN
################################################################################

createThread[void](measure_ping_thread, measure_ping)

let header = %* { "version" : 1 }
echo header
echo "["

while true:
  let s =  display_battery() & wifi() & display_ping() & display_cpu_load() & df() & clock()
  echo "["
  echo s.join(", ")
  echo "],"
  os.sleep(1000)
