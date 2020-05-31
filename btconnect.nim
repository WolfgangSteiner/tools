import os
import system
import strformat
import wst_tools
import tables

proc connect(mac_address : string) =
  shellCmd(&"echo connect {mac_address} | bluetoothctl")


let devices = to_table(
  {"slim"  : "20:9b:a5:68:e7:d6",
   "slim2" : "e8:07:bf:69:19:74",
   "boost" : "00:22:37:4e:0c:08",
   "vortex": "00:1c:d8:e8:ac:14"})

let device = if paramCount() == 0: "slim2" else: paramStr(1)
connect(devices[device])
