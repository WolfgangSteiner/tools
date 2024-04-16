#include "wst.h"


void nagbar(char* prompt, char* cmd)
{
    wst_string nagbarCmd = wst_string_init("i3-nagbar -m '");
    nagbarCmd = wst_string_append(nagbarCmd, prompt);
    nagbarCmd = wst_string_append(nagbarCmd, "' -t warning -B 'YES!' '");
    nagbarCmd = wst_string_append(nagbarCmd, cmd);
    nagbarCmd = wst_string_append(nagbarCmd, "'");
    system(nagbarCmd);
    wst_string_delete(nagbarCmd);
}


void menu_brightness()
{
    char* result = wst_dmenu("Brightness Level", "0.5,1,2,3,4,5,6,7,8,9,10");

    if (strlen(result))
    {
        char* cmd = wst_string_join("dim", result, " ");
        system(cmd);
    }
}


void menu_tools()
{
    char* result = wst_dmenu("Tools", "ping,speedtest");
    if (wst_string_equal(result, "ping"))
    {
        system("st -e ping 1.1.1.1");
    }
    else if (wst_string_equal(result, "speedtest"))
    {
        system("st -e bash -c \"speedtest;read\"");
    }
}


void menu_i3()
{
    char* result = wst_dmenu("i3", "Edit config,Reload config,Restart,Exit");
    
    if (wst_string_startsWith(result, "Edit"))
    {
        system("gvim ~/.config/i3/config");
    }
    else if (wst_string_startsWith(result, "Reload"))
    {
        system("i3-msg reload");
    }
    else if (wst_string_startsWith(result, "Restart"))
    {
        system("i3-msg restart");
    }
    else if (wst_string_startsWith(result, "Exit"))
    {
        nagbar("Exit i3?", "i3-msg exit");
    }
}


void lock_screen()
{
    system("i3lock -i ~/.dotfiles/img/lockscreen-black.png");
}


void menu_computer()
{
    char* result = wst_dmenu("Computer", "Lock screen,Suspend,Hibernate,Reboot");

    if (wst_string_startsWith(result, "Lock"))
    {
        lock_screen();
    }
    else if (wst_string_startsWith(result, "Suspend"))
    {
        system("systemctl hybrid-sleep");
    }
    else if (wst_string_startsWith(result, "Hibernate"))
    {
        system("systemctl hibernate");
    }
    else if (wst_string_equal(result, "Reboot"))
    {
        nagbar("Reboot?", "systemctl reboot");
    }
}


int main(void)
{
    char* result = wst_dmenu("", "Brightness,Tools,i3,Computer");

    if (wst_string_equal(result, "Brightness"))
    {
        menu_brightness();
    }
    else if (wst_string_equal(result, "Tools"))
    {
        menu_tools();
    }
    else if (wst_string_equal(result, "i3"))
    {
        menu_i3();
    }
    else if (wst_string_equal(result, "Computer"))
    {
        menu_computer();
    }
}
