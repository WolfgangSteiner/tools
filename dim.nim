import os, strutils, parseutils, strformat, math, times
import wst_tools

const MAX_LEVEL = 10
##    LEVELS      0     1     2     3     4     5     6     7     8     9    10
const LIGHT = [   1,    1,    2,    5,    5,   10,   10,   25,   50,  100,  100]
const TEMP =  [1000, 1500, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500]
const DIM =   [   0,  100]


proc get_value_for_level(arr: openArray[int], level: float): int =
  assert(level >= 0.0)
  if floor(level) == level:
    let idx = min(len(arr) - 1, int(level))
    return arr[idx]
  elif level > float(len(arr)):
    return high(arr)
  else:
    let idx1 = min(len(arr) - 1, int(floor(level)))
    let idx2 = min(len(arr) - 1, int(ceil(level)))
    let v1 = float(arr[idx1])
    let v2 = float(arr[idx2])
    let a = level - floor(level)
    return int(v1 + a * (v2 - v1))


proc set_light(level: int) =
  assert(level >= 0 and level <= 100)
  shell_cmd fmt"light -S {level}"


proc print_usage() =
  echo "Usage: dim LEVEL\n"
  echo fmt"LEVEL must be a float between 0 and {MAX_LEVEL}.\n"


proc set_redshift(temp: int, dim: int) =
  let dim_f = float(dim) / 100.0
  shell_cmd fmt"redshift -P -O {temp} -b {dim_f}"


var level:float = 5

if paramCount() == 0:
  let hour = now().hour
  level = if hour in (7..18): 9 elif hour in (19..20): 3 else: 1

elif paramCount() == 1:
  let res = parsefloat(paramStr(1), level)

  if res == 0:
    print_usage()
    quit(QuitFailure)

  if level < 0 or level > MAX_LEVEL:
    print_usage()
    quit(QuitFailure)


let light = get_value_for_level(LIGHT, level)
let temp = get_value_for_level(TEMP, level)
let dim = get_value_for_level(DIM, level)

set_redshift(temp, dim)
set_light(light)
