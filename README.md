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


Detections
----------

Here's a list of things that are actually detected by the anti-cheat. It also lists you the macros to use as "cheatid" :

<pre>
#define CHEAT_AFK                      (-1)                // Cheat ID -1 = OnPlayerCheat is also used for AFK detection. 
#define CHEAT_MONEY                    (0)                 // Cheat ID 0 = Money cheat
#define CHEAT_WEAPON                   (1)                 // Cheat ID 1 = Drop gun cheat
#define CHEAT_HEALTH                   (2)                 // Cheat ID 2 = Health cheat
#define CHEAT_ARMOUR                   (3)                 // Cheat ID 3 = Armour cheat
#define CHEAT_BLOCK                    (4)                 // Cheat ID 4 = Block ammo cheat (more ammo than authorized)
#define CHEAT_MUNI                     (5)                 // Cheat ID 5 = Ammo cheat (adding ammo)
#define CHEAT_TUNING                   (6)                 // Cheat ID 6 = Tuning cheat
#define CHEAT_INVULNERABLE             (7)                 // Cheat ID 7 = Invulnerable cheat
#define CHEAT_FLY                      (8)                 // Cheat ID 8 = Fly cheat (player swims in the air)
#define CHEAT_AIRBREAK                 (9)                 // Cheat ID 9 = Airbreak cheat (no gravity)
#define CHEAT_SKYDIVING                (10)                // Cheat ID 10 = Skydiving cheat (falling from the air to move faster)
#define CHEAT_TELEPORT_MAP             (11)                // Cheat ID 11 = Teleport map cheat (player right clicks on the map and got teleported to the point)
#define CHEAT_TELEPORT_VEH             (12)                // Cheat ID 12 = Teleport veh cheat (player teleports into a vehicle)
#define CHEAT_REMOTE_JACKING           (13)                // Cheat ID 13 = Remote jacking cheat (player messes up with the nearby vehicles and come back to where he was)
#define CHEAT_UNREG_ANIM               (14)                // Cheat ID 14 = Unregistred animation cheat (player uses a non-SAMP animation)
#define CHEAT_SPOOFED_WEAPON           (15)                // Cheat ID 15 = Spoofed weapon cheat
#define CHEAT_FAKE_KILL                (16)                // Cheat ID 16 = Fake kill cheat
#define CHEAT_SPECTATING               (17)                // Cheat ID 17 = Spectating cheat (player spectates other players without being allowed by the script)
#define CHEAT_JETPACK                  (18)                // Cheat ID 18 = Jetpack cheat (player uses a non-legit jetpack)
#define CHEAT_SPEEDHACK                (19)                // Cheat ID 19 = Speedhack (player moves very faster) [EXPERIMENTAL]
#define CHEAT_RAPID_FIRE               (20)                // Cheat ID 20 = Rapidfire cheat
#define CHEAT_AIMBOT                   (21)                // Cheat ID 21 = Aimbot cheat
</pre>

When one of these cheats is detected, it calls the `OnPlayerCheat(playerid, cheatid)` callback.
<pre>
playerid = the cheater id
cheatid = the detected cheat id (use the macros)
</pre>


Things you should change
------------------------

<pre>
#define PING_LIMIT                     (500)               // Maximal ping a player can have before getting kicked
#define MAX_AFK_TIME                   (1200)              // Maximal AFK time in minutes seconds. Once this time is reached, the OnPlayerCheat is called with CHEAT_AFK as cheatid.
#define MAX_PLAYER_HEALTH              (1000.0)            // Maximal health a player can have without being detected as cheater. MUST BE different from 100 and superior to 99. Useful for aduties.
#define MAX_PLAYER_ARMOUR              (1000.0)            // Maximal armour a player can have without being detected as cheater. MUST BE different from 100 and superior to 99. Useful for aduties.
#define MAX_ATTEMPTS                   (2)                 // Maximal unsynchro time in seconds before being timed out.
</pre>

Edit those as much as you want, either directly in the include or by undefining it and redefining it in your script.


Additionnal things
------------------

As stated before, the anti cheat also includes some features.

#### Anti badword

To use it, add this line **BEFORE** including the anti cheat :
<pre>
#define USE_BADWORD_DETECTION
</pre>

And create your own list of forbidden words using this model :

<pre>
stock const AC_badwords[][] =
{
    {"cheat"},
    {"porn"},
    {"hack"},
    {"hacking"},
    {"gay"}
};
</pre>

Each time one of your "forbidden" words will be detected in direct chat (OnPlayerText), the callback `OnPlayerBadword(playerid, badword[])` will be called.
<pre>
playerid = the player who said the bad word
badword[] = the badword who've been said
</pre>

#### Anti VPN

To use it, add this line **BEFORE** including the anti cheat :
<pre>
#define USE_ANTI_VPN
</pre>

It works with a big list of VPN ips already known (and don't worry about any lag, it doesn't slow down/freeze the server), and each time a player will connect with one of these IPs, the callback `OnPlayerVPN(playerid, ip_address[])` will be called.
<pre>
playerid = the player who've connected under VPN
ip_address[] = the VPN ip address
</pre>

*It doesn't kick/ban immediately because you can allow some players to connect under VPN if you want. If you have more VPNs IP that you want to add to the anti cheat for public use, simply give me them either through an issue or via private message on SAMP forums.*

#### Anti foreign country

To use it, add this line **BEFORE** including the anti cheat :
<pre>
#define USE_COUNTRY_CHECKING
</pre>

And define your own list of foreign countries using this model :

<pre>
stock const AC_allowed_countries[][][] = 
{
      {"France", "FR"}, // Name, country code
      {"Canada", "CA"}, // Name, country code
      {"Belgium", "BE"}, // Name, country code
      {"Algeria", "DZ"}, // Name, country code
      {"Morocco", "MO"}, // Name, country code
      {"Tunisia", "TN"}    // Name, country code
       
};</pre>

*This is a pre-made list for French servers. Note that the VPN detection (if you wanna use both the Anti VPN AND the foreign country checker) is made BEFORE the foreign country check. So don't worry of allowing a big amount of countries if you're afraid of some of the countries being a big VPN host*.
<br />
<br />
<br />

Each time a player will connect your server with a foreign IP (foreign from your allowed countries list), the callback `OnPlayerForeignCountry(playerid, ip_address[], country[], query_type)` will be called.
<pre>
playerid = the "foreign" player id
ip_address[] = the "foreign" ip address
country[] = either the "foreign" country name or the "foreign" country code (depends of what query_type's value is)
query_type = the type of query that was done. QUERY_TYPE_CODE will make "country" containing the country code of the foreign country (for example for United States, "US"). QUERY_TYPE_COUNTRY_NAME will make "country" containing the whole country name (for example for United States, "United States"). Check the "geoip" folder for more details.
</pre>

It doesn't kick/ban immediately for the same reasons as for the Anti VPN.

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
2. JernejL : CheckRemoteJacking(playerid)
3. Kalcor : MapAndreas plugin
4. Lordzy : OnPlayerRapidFire (OPRF.inc)
5. Kyance : Aimbot detector ("Silent" Aimbot detector)
6. Maxmind : GeoIP API 
7. Nicow, Emmet_, Wallegi, Alexis (Chipardeur), neloiw : Advices, help, tricks, testing

If you want to contribute to the anti cheat, just submit either an issue or a pull request.

## Original thread (in French)

http://forum.sa-mp.com/showthread.php?t=529686
