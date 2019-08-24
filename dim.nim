import os, strutils, streams, parseutils, system, strformat


const MAX_LEVEL = 9
const LIGHT = [   1,    1,    1,    5,    5,   10,   25,   50,  100]
const TEMP =  [1500, 1500, 1500, 2000, 3000, 4000, 4500, 5500]
const DIM =   [  50,   75,  100]


proc get_value_for_level(arr: openArray[int], level: int): int =
  assert(level >= 0)
  let idx = min(len(arr) - 1, level)
  arr[idx]


proc system(cmd: string) =
  discard execShellCmd(&"{cmd} > /dev/null")


proc reset_redshift() =
  system "redshift -x"


proc set_light(level: int) =
  assert(level >= 0 and level <= 100)
  system fmt"light -S {level}"


proc print_usage() =
  echo "Usage: dim LEVEL\n"
  echo "LEVEL must be an integer between 1 and 7.\n"


proc set_redshift(temp: int, dim: int) =
  let dim_f = float(dim) / 100.0
  reset_redshift()
  system fmt"redshift -O {temp} -b {dim_f}"


if paramCount() == 0:
  reset_redshift()
  set_light(50)

elif paramCount() == 1:
  var level: int
  let res = parseInt(paramStr(1), level)

  if res == 0:
    print_usage()
    quit(QuitFailure)

  if level < 1 or level > MAX_LEVEL:
    print_usage()
    quit(QuitFailure)

  level -= 1

  let light = get_value_for_level(LIGHT, level)
  let temp = get_value_for_level(TEMP, level)
  let dim = get_value_for_level(DIM, level)

  set_redshift(temp, dim)
  set_light(light)
