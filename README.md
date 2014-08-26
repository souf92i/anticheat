Here's my anti cheat.

Here's a list of handled cheats at the moment :

#define CHEAT_AFK                      (-1)                // Cheat ID -1 = Appel de la callback par l'Anti AFK. Ce n'est pas un cheat mais je me voyais mal faire une callback pour ça
#define CHEAT_MONEY                    (0)                 // Cheat ID 0 = Cheat argent
#define CHEAT_WEAPON                   (1)                 // Cheat ID 1 = Cheat arme
#define CHEAT_HEALTH                   (2)                 // Cheat ID 2 = Cheat vie
#define CHEAT_ARMOUR                   (3)                 // Cheat ID 3 = Cheat armure
#define CHEAT_BLOCK                    (4)                 // Cheat ID 4 = Cheat block munitions (munitions infinies)
#define CHEAT_MUNI                     (5)                 // Cheat ID 5 = Cheat munitions (ajout de munitions)
#define CHEAT_TUNING                   (6)                 // Cheat ID 6 = Cheat tuning
#define CHEAT_INVULNERABLE             (7)                 // Cheat ID 7 = Cheat invulnerable
#define CHEAT_FLY                      (8)                 // Cheat ID 8 = Cheat anti fly (quand la personne nage en l'air)
#define CHEAT_AIRBREAK                 (9)                 // Cheat ID 9 = Cheat anti airbreak (anti gravité)
#define CHEAT_SKYDIVING                (10)                // Cheat ID 10 = Cheat skydiving (chute du haut du ciel pour déplacements plus rapides)
#define CHEAT_TELEPORT_MAP             (11)                // Cheat ID 11 = Cheat teleport map (le joueur clique droit sur la carte et s'y téléporte)
#define CHEAT_TELEPORT_VEH             (12)                // Cheat ID 12 = Cheat teleport veh (le joueur se téléporte dans un véhicule)
#define CHEAT_REMOTE_JACKING           (13)                // Cheat ID 13 = Cheat remote jacking (le joueur se téléporte dans le veh d'un autre, fout le bordel avec et revient à son ancienne place)
#define CHEAT_UNREG_ANIM               (14)                // Cheat ID 14 = Cheat unregistrated animation (le joueur utilise un mod qui lui fournit des animations non natives et/ou non détectables par SAMP)
#define CHEAT_SPOOFED_WEAPON           (15)                // Cheat ID 15 = Cheat spoofed weapon (le joueur fait semblant de se faire tuer par une arme que le tueur n'a pas)
#define CHEAT_FAKE_KILL                (16)                // Cheat ID 16 = Cheat fake kill (le joueur fait semblant de se faire tuer)
#define CHEAT_SPECTATING               (17)                // Cheat ID 17 = Cheat spectating (le joueur se met à spec les autres joueurs)
#define CHEAT_JETPACK                  (18)                // Cheat ID 18 = Cheat jetpack (le joueur spawn un jetpack cheaté, aucun risque de faux positif théorique)

And here's a list of things I highly recommend to edit in the include or to #undef and to re-define by yourself in your script.


#define PING_LIMIT                     (500)               // Ping maximal avant d'être kick pour éviter les problèmes de latence
#define MAX_AFK_TIME                   (1200)              // Temps en seconde maximal d'AFK avant d'être kick pour éviter de surcharger le serveur
#define MAX_PLAYER_HEALTH              (1000.0)            // Vie maximale que peut avoir un joueur sans se faire bannir (doit être différent de 100 et supérieur à 99). Utile dans le cas des aduty par exemple
#define MAX_PLAYER_ARMOUR              (1000.0)            // Armure maximale que peut avoir un joueur sans se faire bannir (doit être différent de 100 et supérieur à 99). Utile dans le cas des aduty par exemple
#define MAX_ATTEMPTS                   (3)                 // Nombre de secondes en unsynchro maximal avant de se faire timeout

This is the main point of the include. It calls the callback "OnPlayerCheat(playerid, cheatid)", which MUST return 1.

To differenciate the used cheats, simply use the defined macros in the include (basically, to put something else as ban reason than the simple "Cheat" :

#include <a_samp>
#include <anticheat>

public OnPlayerCheat(playerid, cheatid)
{
      switch(cheatid)
      {
            case CHEAT_AFK: KickWithMessage(playerid, -1, "AFK +%d minuts !", MAX_AFK_TIME/60);
            case CHEAT_MONEY: KickWithMessage(playerid, -1, "Cheat money !");
            // etc, I won't do it all
      }
      return 1; // I insist, returning 0 to this callback will have high chances to unsynchronize the player
}

It also includes an anti badword/anti pub.
This detections calls the callback "OnPlayerBadword(playerid, badword[])" and is used like that :

pawn Code:
#include <a_samp>
#include <anticheat>

stock const AC_badwords[][] = // You declare your own list of forbidden words (please use stock const, it's far more optimized)
{
      {"s0beit"},
      {"ip"}, // to help the anti pub
      {"sobeit"}, //
      {"venez sur mon serveur"} // don't put commas here
};

public OnPlayerBadword(playerid, badword[])
{
      SendClientMessageToAll_(-1, "Player %s told a forbidden word [%s]", playerid, badword);
      return 1; // Return here what you'd return to OnPlayerText
}

Some precisions :

• Inclure the anti cheat to EACH SCRIPT loaded in your server and which might act to anything related to the cheats listed above. It's not necessary to add it to a textdraw editor for example.

• You can do customizable actions in each cheat in each filterscript loaded (plus the gamemode ofc), because the callback is called through CallRemoteFunction.

• The anti AFK is made as best as I could. Each time a player presses a key detected by SAMP excepted the arrows (up, down, left, right), moves his camera or speaks in the cheat, he isn't considered as AFK any more, but is re-considered again the next second (Not the most optimized, but I haven't found better).

[Needs further translation]
