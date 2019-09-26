import osproc
import strutils
import strformat
import os

const SAFE_PERCENT = 20.0
const DANGER_PERCENT = 10.0
const CRITICAL_PERCENT = 5.0

var nagbar : Process = nil
var display_percent : float = 0.0


proc battery_percent() : float =
  readfile("/sys/class/power_supply/BAT0/capacity").strip().parsefloat()


proc is_battery_discharging() : bool =
  readfile("/sys/class/power_supply/BAT0/status").strip() == "Discharging"


proc kill_nagbar() =
  if not nagbar.isNil and nagbar.running():
    nagbar.terminate()
  nagbar = nil


proc update_nagbar(nagbar_type: string, battery_percent: float) =
  if battery_percent != display_percent or nagbar.isNil or not nagbar.running():
    kill_nagbar()
    discard execCmd("i3-msg fullscreen disable")
    nagbar = startProcess(
      "/usr/bin/i3-nagbar",
      args=[
        "-t", nagbar_type,
        "-m", &"Battery at {battery_percent}%",
        "-b", "Hibernate!",  "systemctl hybrid-sleep"])
    display_percent = battery_percent


while true:
  if is_battery_discharging():
    let percent = battery_percent()
    if percent < CRITICAL_PERCENT:
      discard execCmd("echo systemctl hybrid-sleep")
    elif percent < DANGER_PERCENT:
      update_nagbar("error", percent)
    elif percent < SAFE_PERCENT:
      update_nagbar("warning", percent)
  else:
    kill_nagbar()

  sleep(1000)
