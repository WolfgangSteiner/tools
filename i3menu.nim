import wst_tools
import osproc
import strformat

proc main_menu()


proc display_brightness() =
  let level = dmenu("Brightness", "0.5", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10")
  if level == "":
    main_menu()
  else:
    discard execCmd(&"dim {level}")


proc display_tools() =
  case dmenu("Tools", "ping", "speedtest"):
    of "ping":
      discard execCmd("st -e ping 1.1.1.1")
    of "speedtest":
      discard execCmd("st -e bash -c \"speedtest;read\"")
    else:
      main_menu()


proc lock_screen() =
  discard execCmd("i3lock -i ~/.dotfiles/img/lockscreen-black.png")


proc display_i3() =
  case dmenu("i3", "Edit config", "Reload config", "Restart", "Exit"):
    of "Edit config":
      discard execCmd("gvim ~/.config/i3/config")
    of "Reload config":
      discard execCmd("i3-msg reload")
    of "Restart":
      discard execCmd("i3-msg restart")
    of "Exit":
      discard execCmd("i3-nagbar -m 'Exit i3?' -t warning -B 'YES!' 'i3-msg exit'")
    else:
      main_menu()


proc display_computer() =
  case dmenu("Computer", "Lock screen", "Suspend", "Hibernate", "Reboot"):
    of "Lock screen":
      lock_screen()
    of "Suspend":
      discard execCmd("systemctl hybrid-sleep")
    of "Hibernate":
      discard execCmd("systemctl hibernate")
    of "Reboot":
      discard execCmd("i3-nagbar -m 'Reboot?' -t warning -B 'YES!' 'systemctl reboot'")
    else:
      main_menu()


proc main_menu() =
  case dmenu("", "Brightness", "Tools", "i3", "Computer"):
    of "Brightness":
      display_brightness()
    of "Tools":
      display_tools()
    of "i3":
      display_i3()
    of "Computer":
      display_computer()


################################################################################
## MAIN
################################################################################
main_menu()
