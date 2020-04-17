import os, strutils, parseutils, strformat, math, times
import wst_tools

const MAX_LEVEL = 10
##    LEVELS      0        1     2     3     4     5     6     7     8     9    10
const LIGHT = [   0.25, 0.50,    1,    1,    1,    1,   10,   25,   50,  100,  100]
const TEMP =  [   1500, 1500, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500]
const dim_file = expandTilde("~/.dim")

proc set_light(level: int) =
    assert(level >= 0 and level <= 100)
    shell_cmd fmt"light -S {level}"


proc print_usage() =
    echo "Usage: dim LEVEL\n"
    echo fmt"LEVEL must be a float between 0 and {MAX_LEVEL}.\n"


proc calc_dim(light: float) : float =
    if light >= 1.0:
      return 1.0
    else:
      return light

proc calc_light(light: float) : float =
    return max(1.0, light)


proc apply_dim(temp: int, light: float) =
    shell_cmd fmt"redshift -P -O {temp} -b {calc_dim(light)}"
    shell_cmd fmt"light -S {calc_light(light)}"

proc read_current_level() : int =
    if fileExists(dim_file):
        var current_level: int
        let res = parseutils.parseInt(readFile(dim_file), current_level)
        if res != 0:
          return current_level
    return 5

proc write_current_level(level: int) =
    writeFile(dim_file, $level)


var level:int = 5

level = read_current_level()

if paramCount() == 0:
  let hour = now().hour
  level = if hour in (7..18): 9 elif hour in (19..20): 3 else: 1

elif paramCount() == 1:
    if paramStr(1) == "inc":
      level = min(MAX_LEVEL, read_current_level() + 1)
    elif paramStr(1) == "dec":
      level = max(0, read_current_level() - 1)
    else:
      let res = parseint(paramStr(1), level)
      if res == 0:
        print_usage()
        quit(QuitFailure)

      if level < 0 or level > MAX_LEVEL:
        print_usage()
        quit(QuitFailure)

let light = LIGHT[level]
let temp = TEMP[level]
apply_dim(temp, light)
write_current_level(level)
