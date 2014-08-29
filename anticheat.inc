#include <a_samp>
#include <a_http>
#include <mapandreas>

#if defined _ANTICHEAT_INCLUDED
    #endinput
#endif

#define _ANTICHEAT_INCLUDED


/**********************************
        Initialization
*********************************/

enum AC_p
{
    pMoney,
    bool:pWeapons[46],
    pAmmoNr[13],
    pShots[15],
    pLag,
    pAFK,
    pStartAFK,
    pLastUpdate,
    Float:pHealthNr,
    Float:pArmourNr,
    Float:pClicked[3],
    pPut[2],
    pMod[15],
    pLastAJ,
    Float:pLastVelo[3],
    Float:pLastUpdPos[3],
    Float:pLastCamera[3],
    Float:pDropJetpack[3],
    pSkyWarnings,
    bool:pDeathCount,
    bool:pSpectating,
    bool:pJetpack,
    // Lordzy
    pLastShotTick,

};

enum AC_cInfo
{
    weap1,
    weap2,
    weap3,
};

#if defined USE_COUNTRY_CHECKING
    enum E_QUERY_TYPES
    {
        QUERY_TYPE_CODE = 0,
        QUERY_TYPE_COUNTRY_NAME = 1
    };
#endif

new 
    bool:AC_pAlive[MAX_PLAYERS];

static
    AC_PlayerInfo[MAX_PLAYERS][AC_p],
    AC_Classes[300][AC_cInfo],
    AC_waitForSync[MAX_PLAYERS][2]
;

static stock const AC_magazines[] = {34, 17, 7, 1, 4, 7, 100, 30, 30, 50, 100, 1, 1};

/*********************************
        EXPORTED
*********************************/

#define PING_LIMIT                     (500)               // Ping maximal avant d'être kick pour éviter les problèmes de latence
#define MAX_AFK_TIME                   (1200)              // Temps en seconde maximal d'AFK avant d'être kick pour éviter de surcharger le serveur
#define MAX_PLAYER_HEALTH              (1000.0)            // Vie maximale que peut avoir un joueur sans se faire bannir (doit être différent de 100 et supérieur à 99). Utile dans le cas des aduty par exemple
#define MAX_PLAYER_ARMOUR              (1000.0)            // Armure maximale que peut avoir un joueur sans se faire bannir (doit être différent de 100 et supérieur à 99). Utile dans le cas des aduty par exemple
#define MAX_ATTEMPTS                   (3)                 // Nombre de secondes en unsynchro maximal avant de se faire timeout

#if defined USE_COUNTRY_CHECKING
    #define DEFAULT_COUNTRY_CHECKING   (QUERY_TYPE_CODE)   // Type de vérification à effectuer sur l'IP (choisir une des deux vérifications de l'enum)
#endif

#if defined USE_ANTI_VPN || defined USE_COUNTRY_CHECKING
    #define SERVER_URL                 ("s4t3kx.alwaysdata.net/")
#endif

#define CHEAT_SOBEIT                   (-2)                // Cheat ID -2 = Appel de la callback par le détecteur de s0beit. C'est pas un cheat (mais un outil pour cheater), mais ça rentre dans le cadre de la callback. [EXPERIMENTAL]
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
#define CHEAT_SPEEDHACK                (19)                // Cheat ID 19 = Cheat speedhack (le joueur utilise un cheat pour se déplacer plus vite) [EXPERIMENTAL]
#define CHEAT_RAPID_FIRE               (20)                // Cheat ID 20 = Cheat rapid fire (le joueur tire énormément de coups de feu en peu de temps)

forward OnPlayerCheat(playerid, cheatid);       // CETTE CALLBACK DOIT TOUJOURS RETOURNER 1

#if defined USE_ANTI_VPN
    forward OnPlayerVPN(playerid, ip_address[]);
#endif

#if defined USE_BADWORD
    forward OnPlayerBadword(playerid, badword[]);   // Retourner à cette callback ce que vous retourneriez à OnPlayerText (0 ou 1)
#endif

#if defined USE_COUNTRY_CHECKING
    forward OnPlayerForeignCountry(playerid, ip_address[], country_name[], E_QUERY_TYPES:query_type);
#endif

/******************************************
            HOOKED CALLBACKS
******************************************/

#if defined FILTERSCRIPT
    public OnFilterScriptInit()
    {
        #if defined AC_OnFilterScriptInit
            return AC_OnFilterScriptInit();
        #else
            MapAndreas_Init(MAP_ANDREAS_MODE_FULL);
            for(new i, j = GetMaxPlayers(); i< j; i++) 
            {
                if(!IsPlayerConnected(i)) continue;
                AC_PlayerInfo[i][pLastShotTick] = 0;
            }
            return true;
        #endif
    }

    #if defined _ALS_OnFilterScriptInit
        #undef OnFilterScriptInit
    #else
        #define _ALS_OnFilterScriptInit
    #endif
     
    #define OnFilterScriptInit AC_OnFilterScriptInit
    #if defined AC_OnFilterScriptInit
        forward AC_OnFilterScriptInit();
    #endif
#else
    public OnGameModeInit()
    {
        #if defined AC_OnGameModeInit
            return AC_OnGameModeInit();
        #else
            MapAndreas_Init(MAP_ANDREAS_MODE_FULL);
            for(new i, j = GetMaxPlayers(); i< j; i++) 
            {
                if(!IsPlayerConnected(i)) continue;
                AC_PlayerInfo[i][pLastShotTick] = 0;
            }
            return true;
        #endif
    }

    #if defined _ALS_OnGameModeInit
        #undef OnGameModeInit
    #else
        #define _ALS_OnGameModeInit
    #endif
     
    #define OnGameModeInit AC_OnGameModeInit
    #if defined AC_OnGameModeInit
        forward AC_OnGameModeInit();
    #endif
#endif


public OnPlayerSpawn(playerid)
{
    #if defined AC_OnPlayerSpawn
        return AC_OnPlayerSpawn(playerid);
    #else
        AC_pAlive[playerid] = true;
        SetPlayerHealth(playerid, 99.0);
        return  1;
    #endif
}

public OnPlayerConnect(playerid)
{
    #if defined AC_OnPlayerConnect
        return AC_OnPlayerConnect(playerid);
    #else
        // No vending machines, so no risk to ban innocents due to heal fail

        RemoveBuildingForPlayer(playerid, 1302, 0.0, 0.0, 0.0, 6000.0);
        RemoveBuildingForPlayer(playerid, 1209, 0.0, 0.0, 0.0, 6000.0);
        RemoveBuildingForPlayer(playerid, 955, 0.0, 0.0, 0.0, 6000.0);
        RemoveBuildingForPlayer(playerid, 956, 0.0, 0.0, 0.0, 6000.0);
        RemoveBuildingForPlayer(playerid, 1775, 0.0, 0.0, 0.0, 6000.0);
        RemoveBuildingForPlayer(playerid, 1776, 0.0, 0.0, 0.0, 6000.0);
        RemoveBuildingForPlayer(playerid, 1977, 0.0, 0.0, 0.0, 6000.0);

        AC_pAlive[playerid] = false;
        AC_ResetPlayerWeapons(playerid);

        #if defined USE_COUNTRY_CHECKING
            Lookup_ip(playerid, DEFAULT_COUNTRY_CHECKING);
        #endif

        // Lordzy

        AC_PlayerInfo[playerid][pLastShotTick] = 0;
        return  1;
    #endif
}

public OnPlayerRequestClass(playerid, classid)
{
    #if defined AC_OnPlayerRequestClass
        return AC_OnPlayerRequestClass(playerid, classid);
    #else
        AC_pAlive[playerid] = false;
        AC_PlayerInfo[playerid][pWeapons][AC_Classes[classid][weap1]] = true;
        AC_PlayerInfo[playerid][pWeapons][AC_Classes[classid][weap2]] = true;
        AC_PlayerInfo[playerid][pWeapons][AC_Classes[classid][weap3]] = true;
        return 1;
    #endif
}

public OnPlayerDeath(playerid, killerid, reason)
{
    #if defined AC_OnPlayerDeath
        return AC_OnPlayerDeath(playerid, killerid, reason);
    #else
        if(!AC_pAlive[playerid] && AC_PlayerInfo[playerid][pDeathCount]) CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_FAKE_KILL);
        AC_pAlive[playerid] = false;
        if(!AC_PlayerInfo[playerid][pDeathCount]) AC_PlayerInfo[playerid][pDeathCount] = true;
        new weapons[13], ammo, slot;
        slot = GetWeaponSlot(reason);
        for(new s = 0; s < 13; s++) GetPlayerWeaponData(killerid, s, weapons[s], ammo);
        if(weapons[slot] != reason) CallRemoteFunction("OnPlayerCheat", "ii", CHEAT_SPOOFED_WEAPON);
        AC_ResetPlayerWeapons(playerid);
        return 1;
    #endif
}

public OnPlayerDisconnect(playerid, reason)
{
    #if defined AC_OnPlayerDisconnect
        return AC_OnPlayerDisconnect(playerid, reason);
    #else
        static AC_blank[AC_p];
        AC_PlayerInfo[playerid] = AC_blank;
        if(GetPVarInt(playerid, "timeout") == 1)
        {
            new ip[16], cmd[25];
            GetPVarString(playerid, "ip", ip, 16);
            format(cmd, 25, "unbanip %s", ip);
            SendRconCommand(cmd);
            DeletePVar(playerid, "ip");
            DeletePVar(playerid, "timeout");
            return true;
        }
        return true;
    #endif
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    #if defined AC_OnPlayerKeyStateChange
        return AC_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
    #else
        if(AC_PlayerInfo[playerid][pAFK] != -1) AC_PlayerInfo[playerid][pAFK] = -1;
        new Float:z;
        MapAndreas_FindZ_For2DCoord(AC_PlayerInfo[playerid][pLastUpdPos][0], AC_PlayerInfo[playerid][pLastUpdPos][1], z);
        if((AC_PlayerInfo[playerid][pLastUpdPos][2] - 1.0 <= z <= AC_PlayerInfo[playerid][pLastUpdPos][2] + 1.0) && (newkeys & KEY_SECONDARY_ATTACK) && (AC_PlayerInfo[playerid][pJetpack]))
        {
            AC_PlayerInfo[playerid][pJetpack] = false;
            for(new i = 0; i < 3; i++) AC_PlayerInfo[playerid][pDropJetpack][i] = AC_PlayerInfo[playerid][pLastUpdPos][i];
        }
        return true;
    #endif
}

public OnPlayerText(playerid, text[])
{
    #if defined AC_OnPlayerText
        return AC_OnPlayerText(playerid, text);
    #else
        if(AC_PlayerInfo[playerid][pAFK] != -1) AC_PlayerInfo[playerid][pAFK] = -1;
        #if defined USE_BADWORD
            for(new i = 0; i < sizeof(AC_badwords); i++)
            {
                if(strfind(text, AC_badwords[i], true) != -1) return CallRemoteFunction("OnPlayerBadword", "is", playerid, AC_badwords[i]);
            }
            new count;
            for(new i = 0; i < strlen(text); i++)
            {
                if((0 <= text[i] <= '9') && (text[i+1] == '.') && ('0' <= text[i+2]+1 <= '9')) count++, i++;
                if(count == 3) return CallRemoteFunction("OnPlayerBadword", "is", playerid, text);
            }
        #endif
        return true;
    #endif
}

// Anti VPN

#if defined USE_ANTI_VPN

    public OnIncomingConnection(playerid, ip_address[], port)
    {
        #if defined AC_OnIncomingConnection
            return OnIncomingConnection(playerid, ip_address, port);
        #else
            SetPVarString(playerid, "ip", ip_address);
            new buffer[19] = "ip=";
            memcpy(buffer, ip_address, 0, (strlen(ip_address)*4));
            new url[60];
            strcat(url, SERVER_URL);
            strcat(url, "vpn.php");
            HTTP(playerid, HTTP_POST, url, buffer, "OnVPNResponse");
            return 1;
        #endif
    }

    #if defined _ALS_OnIncomingConnection
        #undef OnIncomingConnection
    #else
        #define _ALS_OnIncomingConnection
    #endif
     
    #define OnIncomingConnection AC_OnIncomingConnection
    #if defined AC_OnIncomingConnection
        forward AC_OnIncomingConnection(playerid, ip_address[], port);
    #endif

    static OnVPNResponse(index, response, data[]);
    public OnVPNResponse(index, response, data[])
    {
        if(response != 200) return printf("Problème de traitement de la base de donnée d'IPs [Code d'erreur : %d]", response), 1;
        if(!strcmp(data, "entry", true))
        {
            new ip[16];
            GetPVarString(index, "ip", ip, 16);
            CallRemoteFunction("OnPlayerVPN", "is", index, ip);
        }
        return 1;
    }

#endif

#if defined _ALS_OnPlayerSpawn
    #undef OnPlayerSpawn
#else
    #define _ALS_OnPlayerSpawn
#endif
 
#define OnPlayerSpawn AC_OnPlayerSpawn
#if defined AC_OnPlayerSpawn
    forward AC_OnPlayerSpawn(playerid);
#endif


#if defined _ALS_OnPlayerConnect
    #undef OnPlayerConnect
#else
    #define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect AC_OnPlayerConnect
#if defined AC_OnPlayerConnect
    forward AC_OnPlayerConnect(playerid);
#endif


#if defined _ALS_OnPlayerRequestClass
    #undef OnPlayerRequestClass
#else
    #define _ALS_OnPlayerRequestClass
#endif
 
#define OnPlayerRequestClass AC_OnPlayerRequestClass
#if defined AC_OnPlayerRequestClass
    forward AC_OnPlayerRequestClass(playerid, classid);
#endif


#if defined _ALS_OnPlayerDeath
    #undef OnPlayerDeath
#else
    #define _ALS_OnPlayerDeath
#endif

#define OnPlayerDeath AC_OnPlayerDeath
#if defined AC_OnPlayerDeath
    forward AC_OnPlayerDeath(playerid, killerid, reason);
#endif

#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif
 
#define OnPlayerDisconnect AC_OnPlayerDisconnect
#if defined AC_OnPlayerDisconnect
    forward AC_OnPlayerDisconnect(playerid, reason);
#endif


#if defined _ALS_OnPlayerKeyStateChange
    #undef OnPlayerKeyStateChange
#else
    #define _ALS_OnPlayerKeyStateChange
#endif

#define OnPlayerKeyStateChange AC_OnPlayerKeyStateChange
#if defined AC_OnPlayerKeyStateChange
    forward AC_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
#endif

#if defined _ALS_OnPlayerText
    #undef OnPlayerText
#else
    #define _ALS_OnPlayerText
#endif

#define OnPlayerText AC_OnPlayerText
#if defined AC_OnPlayerText
    forward AC_OnPlayerText(playerid, text[]);
#endif


/***************************************
            MONEY MANAGING
***************************************/


stock AC_GivePlayerMoney(const playerid, const money)
{
    if(money < 0) AC_PlayerInfo[playerid][pMoney] -= money;
    else AC_PlayerInfo[playerid][pMoney] += money;
    return GivePlayerMoney(playerid, money);
}

stock AC_ResetPlayerMoney(const playerid)
{
    AC_PlayerInfo[playerid][pMoney] = 0;
    return ResetPlayerMoney(playerid);
}

stock AC_GetPlayerMoney(playerid)
{
    return AC_PlayerInfo[playerid][pMoney];
}


#if defined _ALS_GivePlayerMoney
    #undef GivePlayerMoney
#else
    #define _ALS_GivePlayerMoney
#endif
 
#define GivePlayerMoney AC_GivePlayerMoney

#if defined _ALS_ResetPlayerMoney
    #undef ResetPlayerMoney
#else
    #define _ALS_ResetPlayerMoney
#endif
 
#define ResetPlayerMoney AC_ResetPlayerMoney


/************************************
            ANTI HACK
            WEAPONS
************************************/

stock AC_GivePlayerWeapon(playerid, weaponid, ammo)
{
    AC_PlayerInfo[playerid][pWeapons][weaponid] = true;
    new s = GetWeaponSlot(weaponid);
    AC_PlayerInfo[playerid][pAmmoNr][s] = ammo;
    return GivePlayerWeapon(playerid, weaponid, ammo);
}

stock AC_ResetPlayerWeapons(playerid)
{
    for(new i = 1; i < 45; i++) AC_PlayerInfo[playerid][pWeapons][i] = false;
    for(new s = 0; s < 13; s++) AC_PlayerInfo[playerid][pAmmoNr][s] = 0;
    return ResetPlayerWeapons(playerid);
}

stock AC_AddPlayerClass(modelid, Float:spawn_x, Float:spawn_y, Float:spawn_z, Float:z_angle, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo)
{
    new classid = AddPlayerClass(modelid, spawn_x, spawn_y, spawn_z, z_angle, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
    AC_Classes[classid][weap1] = weapon1;
    AC_Classes[classid][weap2] = weapon2;
    AC_Classes[classid][weap3] = weapon3;
    return classid;
}

stock AC_AddPlayerClassEx(teamid, modelid, Float:spawn_x, Float:spawn_y, Float:spawn_z, Float:z_angle, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo)
{
    new classid = AddPlayerClassEx(teamid, modelid, spawn_x, spawn_y, spawn_z, z_angle, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
    AC_Classes[classid][weap1] = weapon1;
    AC_Classes[classid][weap2] = weapon2;
    AC_Classes[classid][weap3] = weapon3;
    return classid;
}

stock AC_SetSpawnInfo(playerid, team, skin, Float:x, Float:y, Float:z, Float:rotation, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo)
{
    AC_PlayerInfo[playerid][pWeapons][weapon1] = true;
    AC_PlayerInfo[playerid][pWeapons][weapon2] = true;
    AC_PlayerInfo[playerid][pWeapons][weapon3] = true;
    return SetSpawnInfo(playerid, team, skin, x, y, z, rotation, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
}

stock AC_SetPlayerAmmo(playerid, weaponslot, ammo)
{
    AC_PlayerInfo[playerid][pAmmoNr][weaponslot] = ammo;
    return SetPlayerAmmo(playerid, weaponslot, ammo);
}

#if defined _ALS_GivePlayerWeapon
    #undef GivePlayerWeapon
#else
    #define _ALS_GivePlayerWeapon
#endif

#if defined _ALS_ResetPlayerWeapons
    #undef ResetPlayerWeapons
#else
    #define _ALS_ResetPlayerWeapons
#endif

#if defined _ALS_AddPlayerClass
    #undef AddPlayerClass
#else
    #define _ALS_AddPlayerClass
#endif

#if defined _ALS_AddPlayerClassEx
    #undef AddPlayerClassEx
#else
    #define _ALS_AddPlayerClassEx
#endif

#if defined _ALS_SetSpawnInfo
    #undef SetSpawnInfo
#else
    #define _ALS_SetSpawnInfo
#endif

#if defined _ALS_SetPlayerAmmo
    #undef SetPlayerAmmo
#else
    #define _ALS_SetPlayerAmmo
#endif


#define GivePlayerWeapon AC_GivePlayerWeapon
#define ResetPlayerWeapons AC_ResetPlayerWeapons
#define AddPlayerClass AC_AddPlayerClass
#define AddPlayerClassEx AC_AddPlayerClassEx
#define SetSpawnInfo AC_SetSpawnInfo
#define SetPlayerAmmo AC_SetPlayerAmmo


/**************************************
            ANTI AMMO HACK
**************************************/

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
    #if defined AC_OnPlayerWeaponShot
        return AC_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, fX, fY, fZ);
    #else
    // Anti block munitions

    if((22 <= weaponid <= 34) && (++AC_PlayerInfo[playerid][pShots][weaponid-22] > AC_magazines[weaponid-22])) CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_BLOCK);

    // Anti ammo hack

    if(16 <= weaponid <= 43)
    {
        new s = GetWeaponSlot(weaponid);
        AC_PlayerInfo[playerid][pAmmoNr][s]--;
    }

    // Lordzy

    new
        tick = GetTickCount();

    if((((22 <= weaponid <= 27) || (29 <= weaponid <= 31)) && ((tick - AC_PlayerInfo[playerid][pLastShotTick]) < 35)) || \
        (((weaponid == 28 || weaponid == 32)) && ((tick - AC_PlayerInfo[playerid][pLastShotTick]) < 30)) || \
        (((33 <= weaponid <= 34)) && ((tick - AC_PlayerInfo[playerid][pLastShotTick]) < 500))) CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_RAPID_FIRE);

    AC_PlayerInfo[playerid][pLastShotTick] = tick;
    return true;
    #endif
}

#if defined _ALS_OnPlayerWeaponShot
    #undef OnPlayerWeaponShot
#else
    #define _ALS_OnPlayerWeaponShot
#endif

#define OnPlayerWeaponShot AC_OnPlayerWeaponShot
#if defined AC_OnPlayerWeaponShot
    forward AC_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
#endif

/********************************************
            ANTI HEALTH/ARMOUR HACK
********************************************/

stock AC_SetPlayerHealth(playerid, Float:health)
{
    if(health == 100.0) health = 99.0;
    AC_PlayerInfo[playerid][pHealthNr] = health;
    return SetPlayerHealth(playerid, health);
}

stock AC_SetPlayerArmour(playerid, Float:armour)
{
    if(armour == 100.0) armour = 99.0;
    AC_PlayerInfo[playerid][pArmourNr] = armour;
    return SetPlayerArmour(playerid, armour);
}

public OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart)
{
    #if defined AC_OnPlayerTakeDamage
        return AC_OnPlayerTakeDamage(playerid, issuerid, amount, weaponid, bodypart);
    #else
        if(amount <= AC_PlayerInfo[playerid][pArmourNr]) AC_PlayerInfo[playerid][pArmourNr] -= amount, AC_waitForSync[playerid][0] = SetTimerEx("WaitForSync", PING_LIMIT*2, true, "ii", playerid, 0);
        else AC_PlayerInfo[playerid][pHealthNr] -= amount, AC_waitForSync[playerid][1] = SetTimerEx("WaitForSync", PING_LIMIT*2, true, "ii", playerid, 1);
        return 1;
    #endif
}

#if defined _ALS_SetPlayerHealth
    #undef SetPlayerHealth
#else
    #define _ALS_SetPlayerHealth
#endif

#if defined _ALS_SetPlayerArmour
    #undef SetPlayerArmour
#else
    #define _ALS_SetPlayerArmour
#endif

#if defined _ALS_OnPlayerTakeDamage
    #undef OnPlayerTakeDamage
#else
    #define _ALS_OnPlayerTakeDamage
#endif

#define SetPlayerHealth AC_SetPlayerHealth
#define SetPlayerArmour AC_SetPlayerArmour
#define OnPlayerTakeDamage AC_OnPlayerTakeDamage
#if defined AC_OnPlayerTakeDamage  
    forward AC_OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart);
#endif

static WaitForSync(playerid, type);
public WaitForSync(playerid, type)
{
    static nbr;
    nbr++;
    new Float:fl;
    switch(type)
    {
        case 0:
        {
            GetPlayerArmour(playerid, fl);
            if((AC_PlayerInfo[playerid][pArmourNr] != fl) && (nbr == 2))
            {
                KillTimer(AC_waitForSync[playerid][0]);
                CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_INVULNERABLE);
                nbr = 0;
                return;
            }
        }
        case 1:
        {
            GetPlayerHealth(playerid, fl);
            if((AC_PlayerInfo[playerid][pHealthNr] != fl) && (nbr == 2))
            {
                KillTimer(AC_waitForSync[playerid][1]);
                CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_INVULNERABLE);
                nbr = 0;
                return;
            }
        }
    }
}

/**************************************
            CHEAT CLICK MAP
**************************************/

public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
    #if defined AC_OnPlayerClickMap
        return AC_OnPlayerClickMap(playerid, fx, fy, fz);
    #else
        AC_PlayerInfo[playerid][pClicked][0] = fX;
        AC_PlayerInfo[playerid][pClicked][1] = fY;
        MapAndreas_FindZ_For2DCoord(fX, fY, AC_PlayerInfo[playerid][pClicked][2]);
        return true;
    #endif
}

#if defined _ALS_OnPlayerClickMap
    #undef OnPlayerClickMap
#else
    #define _ALS_OnPlayerClickMap
#endif

#define OnPlayerClickMap AC_OnPlayerClickMap
#if defined AC_OnPlayerClickMap
    forward AC_OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ);
#endif

/***************************************
        ANTI CHEAT SPECTATING
***************************************/

stock AC_TogglePlayerSpectating(playerid, bool:toggle)
{
    AC_PlayerInfo[playerid][pSpectating] = toggle;
    return TogglePlayerSpectating(playerid, toggle);
}

stock AC_PlayerSpectatePlayer(playerid, targetplayerid, mode = SPECTATE_MODE_NORMAL)
{
    AC_PlayerInfo[playerid][pSpectating] = true;
    return PlayerSpectatePlayer(playerid, targetplayerid, mode);
}

stock AC_PlayerSpectateVehicle(playerid, targetvehicleid, mode = SPECTATE_MODE_NORMAL)
{
    AC_PlayerInfo[playerid][pSpectating] = true;
    return PlayerSpectateVehicle(playerid, targetvehicleid, mode);
}

#if defined _ALS_TogglePlayerSpectating
    #undef TogglePlayerSpectating
#else
    #define _ALS_TogglePlayerSpectating
#endif

#if defined _ALS_PlayerSpectatePlayer
    #undef PlayerSpectatePlayer
#else
    #define _ALS_PlayerSpectatePlayer
#endif

#if defined _ALS_PlayerSpectateVehicle
    #undef PlayerSpectateVehicle
#else
    #define _ALS_PlayerSpectateVehicle
#endif

#define TogglePlayerSpectating AC_TogglePlayerSpectating
#define PlayerSpectatePlayer AC_PlayerSpectatePlayer
#define PlayerSpectateVehicle AC_PlayerSpectateVehicle

/**************************************
            FOREIGN COUNTRY
**************************************/

#if defined USE_COUNTRY_CHECKING

    static stock Lookup_ip(playerid, E_QUERY_TYPES:query_type, bool:ipv6 = false)
    {
        new buffer[35] = "ip=";
        GetPlayerIp(playerid, buffer[3], 16);
        SetPVarString(playerid, "ip", buffer[3]);
        SetPVarInt(playerid, "query", query_type);
        strcat(buffer, "&query=");
        format(buffer, sizeof(buffer), "%s%d", buffer, query_type);
        strcat(buffer, "&ipv6=");
        format(buffer, sizeof(buffer), "%s%d", buffer, ipv6);

        new url[60];
        strcat(url, SERVER_URL);
        strcat(url, "geoip/_geoip.php");
        return HTTP(playerid, HTTP_POST, url, buffer, "OnGeoIPResponse");
    }

    static OnGeoIPResponse(index, response, data[]);
    public OnGeoIPResponse(index, response, data[])
    {
        new query = GetPVarInt(playerid, "query");
        for(new i = 0; i < sizeof(AC_allowed_countries); ++i)
        {
            if(!strcmp(AC_allowed_countries[i][query], data, true)) return 1;
        }
        new ip[16];
        GetPVarString(index, "ip", ip, 16);
        CallRemoteFunction("OnPlayerForeignCountry", "issi", index, ip, data, query);
        DeletePVar(playerid, "ip");
        DeletePVar(playerid, "query");
        return 1;
    }

#endif

/**************************************
            ANTI CHEAT JETPACK
**************************************/

stock AC_SetPlayerSpecialAction(playerid, actionid)
{
    if(actionid == SPECIAL_ACTION_USEJETPACK) AC_PlayerInfo[playerid][pJetpack] = true;
    if(actionid == SPECIAL_ACTION_NONE) AC_PlayerInfo[playerid][pJetpack] = false;
    return SetPlayerSpecialAction(playerid, actionid);
}

#if defined _ALS_SetPlayerSpecialAction
    #undef SetPlayerSpecialAction
#else
    #define _ALS_SetPlayerSpecialAction
#endif

#define SetPlayerSpecialAction AC_SetPlayerSpecialAction

/***************************************
            ANTI CHEAT TUNING
***************************************/

stock AC_AddVehicleComponent(vehicleid, componentid)
{
    new slot = GetVehicleComponentType(componentid);
    for(new i = 0, l = GetMaxPlayers(); i < l; i++)
    {
        if(!IsPlayerConnected(i)) continue;
        if(GetPlayerVehicleID(i) == vehicleid) AC_PlayerInfo[i][pMod][slot] = 2, AC_PlayerInfo[i][pMod][14] = vehicleid;
    }
    return AddVehicleComponent(vehicleid, componentid);
}

stock AC_RemoveVehicleComponent(vehicleid, componentid)
{
    new slot = GetVehicleComponentType(componentid)
    for(new i = 0, l = GetMaxPlayers(); i < l; i++)
    {
        if(!IsPlayerConnected(i)) continue;
        if(GetPlayerVehicleID(i) == vehicleid) AC_PlayerInfo[i][pMod][slot] = -1, AC_PlayerInfo[i][pMod][14] = vehicleid;
    }
    return RemoveVehicleComponent(vehicleid, componentid);
}

public OnVehicleMod(playerid, vehicleid, componentid)
{
    new slotid = GetVehicleComponentType(componentid);
    if(GetPlayerInterior(playerid) == 0 && AC_PlayerInfo[playerid][pMod][14] != vehicleid && (AC_PlayerInfo[playerid][pMod][slotid] != 2 && AC_PlayerInfo[playerid][pMod] != -1))
        CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_TUNING);
}

#if defined _ALS_AddVehicleComponent
    #undef AddVehicleComponent
#else
    #define _ALS_AddVehicleComponent
#endif

#if defined _ALS_RemoveVehicleComponent
    #undef RemoveVehicleComponent
#else
    #define _ALS_RemoveVehicleComponent
#endif

#if defined _ALS_OnVehicleMod
    #undef OnVehicleMod
#else
    #define _ALS_OnVehicleMod
#endif

#define AddVehicleComponent AC_AddVehicleComponent
#define RemoveVehicleComponent AC_RemoveVehicleComponent
#define OnVehicleMod AC_OnVehicleMod
#if defined AC_OnVehicleMod
    forward AC_OnVehicleMod(playerid, vehicleid, componentid);
#endif

/************************************
            ANTI TP VEH
************************************/

stock AC_PutPlayerInVehicle(playerid, vehicleid, seatid)
{
    AC_PlayerInfo[playerid][pPut][0] = vehicleid;
    AC_PlayerInfo[playerid][pPut][1] = seatid;
    return PutPlayerInVehicle(playerid, vehicleid, seatid);
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
    #if defined AC_OnPlayerEnterVehicle
        return AC_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
    #else
        AC_PlayerInfo[playerid][pPut][0] = vehicleid;
        AC_PlayerInfo[playerid][pPut][1] = ispassenger;
        return true;
    #endif
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
    #if defined AC_OnPlayerStateChange
        return AC_OnPlayerStateChange(playerid, newstate, oldstate);
    #else
        if(((newstate == PLAYER_STATE_DRIVER) || (newstate == PLAYER_STATE_PASSENGER)) && ((AC_PlayerInfo[playerid][pPut][0] != GetPlayerVehicleID(playerid)) || ((AC_PlayerInfo[playerid][pPut][1] == 0 && GetPlayerVehicleSeat(playerid) != 0) || (AC_PlayerInfo[playerid][pPut][1] != 0 && GetPlayerVehicleSeat(playerid) == 0))))
            CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_TELEPORT_VEH);
        if((newstate == PLAYER_STATE_SPECTATING) && (!AC_PlayerInfo[playerid][pSpectating]))
            CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_SPECTATING);
        return true;
    #endif
}

#if defined _ALS_PutPlayerInVehicle
    #undef PutPlayerInVehicle
#else
    #define _ALS_PutPlayerInVehicle
#endif

#if defined _ALS_OnPlayerEnterVehicle
    #undef OnPlayerEnterVehicle
#else
    #define _ALS_OnPlayerEnterVehicle
#endif

#if defined _ALS_OnPlayerStateChange
    #undef OnPlayerStateChange
#else
    #define _ALS_OnPlayerStateChange
#endif

#define PutPlayerInVehicle AC_PutPlayerInVehicle
#define OnPlayerEnterVehicle AC_OnPlayerEnterVehicle
#define OnPlayerStateChange AC_OnPlayerStateChange

#if defined AC_OnPlayerEnterVehicle
    forward AC_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
#endif

#if defined AC_OnPlayerStateChange
    forward AC_OnPlayerStateChange(playerid, newstate, oldstate);
#endif

/************************************
        INTERNAL FUNCTIONS
************************************/

stock Timeout(playerid)
{
    new ip[16], cmd[23];
    GetPVarString(playerid, "ip", ip, 16);
    SetPVarInt(playerid, "timeout", 1);
    SendRconCommand(cmd);
    return true;
}

stock GetWeaponSlot(weaponid)
{
    switch(weaponid)
    {
        case 0,1: return 0;
        case 2..9: return 1;
        case 10..15: return 10;
        case 16, 17, 18, 39: return 8;
        case 22..24: return 2;
        case 25..27: return 3;
        case 28, 29, 32: return 4;
        case 30, 31: return 5;
        case 33, 34: return 6;
        case 35..38: return 7;
        case 40: return 12;
        case 41..43: return 9;
        case 44..46: return 11;
    }
    return -1;
}

stock GetWeaponBySlot(playerid, slot)
{
    new w[2];
    GetPlayerWeaponData(playerid, slot, w[0], w[1]);
    return w[0];
}

static stock GetFreeCell(const array[], def = 0)
{
    for(new i = 0; i < 15; i++)
    {
        if(array[i] == def) return i;
    }
    return -1;
}

static stock CheckRemoteJacking(playerid)
{
    new currstate;
    currstate = GetPlayerVehicleID(playerid);
    
    if ((currstate != AC_PlayerInfo[playerid][pLastAJ]) && (currstate) && (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)) 
    {
        new Float:pos[3];
        GetVehiclePos(GetPlayerVehicleID(playerid), pos[0], pos[1], pos[2]);
        
        new 
            Float:dist = VectorSize(pos[0]-AC_PlayerInfo[playerid][pLastUpdPos][0], pos[1]-AC_PlayerInfo[playerid][pLastUpdPos][1], pos[2]-AC_PlayerInfo[playerid][pLastUpdPos][2]),
            model = GetVehicleModel(GetPlayerVehicleID(playerid)),
            Float:radi = 10.0;
        
        if ((model == 577) || (model == 592)) radi = 25.0; // larger values for AT 400 & Andromada (larger models)
        
        if (dist > radi) CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_REMOTE_JACKING);

        GetPlayerPos(playerid, AC_PlayerInfo[playerid][pLastUpdPos][0], AC_PlayerInfo[playerid][pLastUpdPos][1], AC_PlayerInfo[playerid][pLastUpdPos][2]);
        AC_PlayerInfo[playerid][pLastAJ] = currstate;
        return true;
    }
    return false;
}

stock IsPlayerFalling(playerid)
{
    new Float:z, Float:zv;
    GetPlayerPos(playerid, z, z, z);
    GetPlayerVelocity(playerid, zv, zv, zv);
    if((z < AC_PlayerInfo[playerid][pLastUpdPos][2]) && (zv - 0.7 <= AC_PlayerInfo[playerid][pLastUpdPos][2] - z <= zv + 0.7)) return true;
    return false;
}

/**********************************
            CHECKING
**********************************/


public OnPlayerUpdate(playerid)
{
    #if defined AC_OnPlayerUpdate
        return AC_OnPlayerUpdate(playerid);
    #else

    if(GetPVarInt(playerid, "timeout") == 1) return true;

    new time = gettime();
    if(time == AC_PlayerInfo[playerid][pLastUpdate]) return true;

    // Prevent fake checking
    if(!AC_pAlive[playerid]) return true;
    if(GetPlayerPing(playerid) >= PING_LIMIT && ++AC_PlayerInfo[playerid][pLag] == MAX_ATTEMPTS) return Timeout(playerid);
    //if(!IsPlayerSynced(playerid)) return Timeout(playerid);

    AC_PlayerInfo[playerid][pLastUpdate] = time;
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    // Anti AFK
    new Float:cam[3];
    GetPlayerCameraFrontVector(playerid, cam[0], cam[1], cam[2]);
    if((AC_PlayerInfo[playerid][pLastCamera][0] != cam[0]) || (AC_PlayerInfo[playerid][pLastCamera][1] != cam[1]) || (AC_PlayerInfo[playerid][pLastCamera][2] != cam[2])) AC_PlayerInfo[playerid][pAFK] = -1;
    else if((AC_PlayerInfo[playerid][pLastCamera][0] == cam[0]) && (AC_PlayerInfo[playerid][pLastCamera][1] == cam[1]) && (AC_PlayerInfo[playerid][pLastCamera][2] == cam[2]))
    {
        AC_PlayerInfo[playerid][pAFK] = time;
        if(AC_PlayerInfo[playerid][pStartAFK] == -1) AC_PlayerInfo[playerid][pStartAFK] = time;
        if((AC_PlayerInfo[playerid][pAFK] - AC_PlayerInfo[playerid][pStartAFK] >= MAX_AFK_TIME) || (time - AC_PlayerInfo[playerid][pLastUpdate] >= MAX_AFK_TIME)) CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_AFK);
    }

    // Anti unreg anim
    new idx = GetPlayerAnimationIndex(playerid);
    if(idx < 0) CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_UNREG_ANIM); // BE CARREFUL ! May bug with some of the SEX animations
                                                                                       // But you must be a weird man to use them, really

    // Anti hack munitions
    new s = GetWeaponSlot(GetPlayerWeapon(playerid));
    if((16 <= GetPlayerWeapon(playerid) <= 44) && ((GetPlayerAmmo(playerid) == -1) || (GetPlayerAmmo(playerid) != AC_PlayerInfo[playerid][pAmmoNr][s]))) CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_MUNI);


    // Anti hack argent
    if(AC_PlayerInfo[playerid][pMoney] != GetPlayerMoney(playerid)) CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_MONEY);

    // Anti hack vie/armure

    new Float:health, Float:armour;
    GetPlayerHealth(playerid, health);
    GetPlayerArmour(playerid, armour);
    if(((99.0 < health) || (health > MAX_PLAYER_HEALTH)) || (health != AC_PlayerInfo[playerid][pHealthNr])) CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_HEALTH);
    if(((99.0 < armour) || (armour > MAX_PLAYER_ARMOUR)) || (armour != AC_PlayerInfo[playerid][pArmourNr])) CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_ARMOUR);

    //Anti click tp

    if((!IsPlayerInRangeOfPoint(playerid, 2.0, x, y, z)) && (IsPlayerInRangeOfPoint(playerid, 3.0, AC_PlayerInfo[playerid][pClicked][0], AC_PlayerInfo[playerid][pClicked][1], AC_PlayerInfo[playerid][pClicked][2])))
        CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_TELEPORT_MAP);

    // Anti airbreak & anti fly

    new Float:z1, Float:velo[3];
    MapAndreas_FindZ_For2DCoord(x, y, z1);
    if(!IsPlayerInAnyVehicle(playerid)) GetPlayerVelocity(playerid, velo[0], velo[1], velo[2]);
    else GetVehicleVelocity(GetPlayerVehicleID(playerid), velo[0], velo[1], velo[2]);

    if((z1 - 1.0 > AC_PlayerInfo[playerid][pLastUpdPos][2]) || (!IsPlayerInRangeOfPoint(playerid, 1.0, AC_PlayerInfo[playerid][pLastUpdPos][0], AC_PlayerInfo[playerid][pLastUpdPos][1], AC_PlayerInfo[playerid][pLastUpdPos][2]) && (velo[0] <= 1.0 && velo[1] <= 1.0 && velo[2] <= 1.0)))
        CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_AIRBREAK);

    if((idx == 1538 || idx == 1539 || idx == 1543) && (velo[2]-5.0 <= AC_PlayerInfo[playerid][pLastVelo][2] <= velo[2]+5.0) && (++AC_PlayerInfo[playerid][pSkyWarnings] == 3)) CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_FLY), AC_PlayerInfo[playerid][pSkyWarnings] = 0;


    // Anti remote-jacking

    CheckRemoteJacking(playerid); // JernejL

    // anti skydiving

    new anim[2][25];
    GetAnimationName(idx, anim[0], 25, anim[1], 25);
    if((!strcmp(anim[0], "GYMNASIUM", true)) && (!strcmp(anim[1], "gym_thread_jog", true)) && (!IsPlayerFalling(playerid))) CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_SKYDIVING);

    // anti cheat jetpack
    if((GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_USEJETPACK) && (IsPlayerInRangeOfPoint(playerid, 1.0, AC_PlayerInfo[playerid][pDropJetpack][0], AC_PlayerInfo[playerid][pDropJetpack][1], AC_PlayerInfo[playerid][pDropJetpack][2]))) AC_PlayerInfo[playerid][pJetpack] = true;
    if((!AC_PlayerInfo[playerid][pJetpack]) && (GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_USEJETPACK)) CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_JETPACK);

    // EXPERIMENTAL : anti speed hack

    if((VectorSize(AC_PlayerInfo[playerid][pLastUpdPos][0] - x, AC_PlayerInfo[playerid][pLastUpdPos][1] - y, AC_PlayerInfo[playerid][pLastUpdPos][2] - z) > (IsPlayerInAnyVehicle(playerid) ? 180.0 : 100.0))) CallRemoteFunction("OnPlayerCheat", "ii", playerid, CHEAT_SPEEDHACK);

    // saving

    AC_PlayerInfo[playerid][pLastUpdPos][0] = x;
    AC_PlayerInfo[playerid][pLastUpdPos][1] = y;
    AC_PlayerInfo[playerid][pLastUpdPos][2] = z;
    AC_PlayerInfo[playerid][pLastVelo][0] = velo[0];
    AC_PlayerInfo[playerid][pLastVelo][1] = velo[1];
    AC_PlayerInfo[playerid][pLastVelo][2] = velo[2];
    return true;

    #endif
}

#if defined _ALS_OnPlayerUpdate
    #undef OnPlayerUpdate
#else
    #define _ALS_OnPlayerUpdate
#endif
 
#define OnPlayerUpdate AC_OnPlayerUpdate
#if defined AC_OnPlayerUpdate
    forward AC_OnPlayerUpdate(playerid);
#endif

/*********************************

        Script de test :

public OnPlayerCheat(playerid, cheatid)
{
    new player_name[21];
    GetPlayerName(playerid, player_name, 21);
    printf("Joueur ID : %d || Cheat ID : %d || Nom du joueur : %s", playerid, cheatid, player_name);
    return 1;
}

*********************************/