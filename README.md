Anticheat include
-----------------

This is an anticheat system currently on alpha, developped by S4t3K. It handles also some interessant features such as **a badword detector**, **an Anti VPN** (working with a big list of "blacklisted" ips) and a **foreign country detector**.
It's a public release under the MIT License : do whatever you want (edit, sell, hack, love) since you keep my credits on it.


Setting up
----------

#### Without the php files

Download the *anticheat.inc* and move it to your `pawno/includes` folder. Rebuild all your scripts (filterscripts and gamemode) but adding the *#include <anticheat>* line to each file (it's not needed on filterscripts such as Textdraws editors, Map Mover, etc). Put the callback *OnPlayerCheat* whereever you want (multiple places allowed (in each filterscript but not in the gamemode for example)) and do actions when a cheater is spotted !


#### With the php files

Download the whole source (*anticheat.inc* and the `php` directory). Put only the **content** of the php directory (thus not directly the php folder, but the vpn.php AND the geoip folder) in a webspace. Edit the line 90 of the **anticheat.inc** and put your own domain name without the *http://* but with the final */*. Save the file. Then follow the instructions from the above part.


Issues
------

#### Solving an error 025: function heading differs from prototype about OnPlayerTakeDamage for those who use YSI

A fix has been released. You can find it here : http://forum.sa-mp.com/showthread.php?t=488198


#### Solving a warning 213: tag mismatch on TogglePlayerSpectating 

It happens when you directly put the the "toggle" parameter as a decimal value (number) such as shown here :

<pre>
TogglePlayerSpectating(playerid, 0);
TogglePlayerSpectating(playerid, 1);
</pre>

Simply solve it by using the boolean equivalents of the decimal values above :

<pre>
TogglePlayerSpectating(playerid, false);
TogglePlayerSpectating(playerid, true);
</pre>


Special thanks
--------------

1. cessil : Anti cheat tips'n'tricks
2. Kalcor : MapAndreas plugin
3. Lordzy : OnPlayerRapidFire (OPRF.inc)
4. Nicow, Emmet_, Wallegi, Alexis (Chipardeur), neloiw : Advices, help, tricks, testing


## Original thread (in French)

http://forum.sa-mp.com/showthread.php?t=529686
