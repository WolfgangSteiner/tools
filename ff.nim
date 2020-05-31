import wst_tools
import os

var cmd = "firefox --new-window --kiosk"

if paramCount() == 1:
  cmd &= " " & paramStr(1)

shell_cmd cmd
