/*
________________________________________________________________________________

								L_Lag - v1.1
								    By
								  Lordzy

This include allows to get a player to lag by slowing down their connections
with the server, which can result in increase over packet-loss and causes
ping fluctuations.

LICENSE:
      This Source Code Form is subject to the
      terms of the Mozilla Public License, v.
      2.0. If a copy of the MPL was not
      distributed with this file, You can
      obtain one at
      http://mozilla.org/MPL/2.0/.
      

native TogglePlayerLag(playerid, bool:lag_state, lag_delay = 3000);
native GetPlayerLagState(playerid);
native ReconnectPlayer(playerid);

ChangeLogs:

- Uses function BlockIpAddress for SA-MP versions 0.3z R2-2 and above.
- Adds new optional function : ReconnectPlayer
________________________________________________________________________________*/

#tryinclude <a_samp>


new static
	g_PlayerLagTimer[MAX_PLAYERS] = {-1, ...},
	g_PlayerIp[MAX_PLAYERS][16],
	bool:g_LagState[MAX_PLAYERS],
	bool:g_LagInit = false
;

static stock L_LagInit() {

	for(new i = 0, j = GetMaxPlayers(); i< j; i++) {
	
	    if(!IsPlayerConnected(i))
	        continue;
	        
		OnPlayerConnect(i);
	}
	g_LagInit = true;

	return 1;
}

public OnFilterScriptInit() {

	if(!g_LagInit)
	    L_LagInit();

	#if defined L_LAG_OnFS
	    return L_LAG_OnFS();
	#else
	    return 1;
	#endif
}

public OnGameModeInit() {

	if(!g_LagInit)
	    L_LagInit();

	#if defined L_LAG_OnGM
	    return L_LAG_OnGM();
	#else
	    return 1;
	#endif
}

public OnPlayerConnect(playerid) {

	if(g_PlayerLagTimer[playerid] != -1) {
	
	    KillTimer(g_PlayerLagTimer[playerid]);
	    g_PlayerLagTimer[playerid] = -1;
	}
	GetPlayerIp(playerid, g_PlayerIp[playerid], 16);
	
	g_LagState[playerid] = false;

	#if defined L_LAG_OnPlayerConnect
	    return L_LAG_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerDisconnect(playerid, reason) {

    if(g_PlayerLagTimer[playerid] != -1) {
    
        KillTimer(g_PlayerLagTimer[playerid]);
        g_PlayerLagTimer[playerid] = -1;
        
        if(reason != 2) {

		#if defined BlockIpAddress
		
		    UnBlockIpAddress(g_PlayerIp[playerid]);
		#else
	        new
	            temp_String[32];

			format(temp_String, sizeof(temp_String), "unbanip %s", g_PlayerIp[playerid]);
			SendRconCommand(temp_String);
		#endif
		}
	}
	#if defined L_LAG_OnPlayerDisconnect
	    return L_LAG_OnPlayerDisconnect(playerid, reason);
	#else
	    return 1;
	#endif
}

stock TogglePlayerLag(playerid, bool:toggle, lag_delay = 3000) {

	#if !defined BlockIpAddress
	new
	    temp_String[32];
	#endif
	
	if(toggle) {
	
		if(g_PlayerLagTimer[playerid] != -1)
		    return 0;


	#if defined BlockIpAddress
	    BlockIpAddress(g_PlayerIp[playerid], lag_delay);
	#else
		format(temp_String, sizeof(temp_String), "banip %s", g_PlayerIp[playerid]);
		SendRconCommand(temp_String);
	#endif
		g_LagState[playerid] = true;
		g_PlayerLagTimer[playerid] = SetTimerEx("L_LagPlayer", lag_delay, true, "dd", playerid, lag_delay);
	}
	else {
	
	    if(g_PlayerLagTimer[playerid] == -1)
	        return 0;
	        
		KillTimer(g_PlayerLagTimer[playerid]);
		g_PlayerLagTimer[playerid] = -1;

	#if defined BlockIpAddress
	    UnBlockIpAddress(g_PlayerIp[playerid]);
	#else
		format(temp_String, sizeof(temp_String), "unbanip %s", g_PlayerIp[playerid]);
		SendRconCommand(temp_String);
	#endif
		g_LagState[playerid] = false;

	}
	return 1;
}

stock GetPlayerLagState(playerid) {

	return (g_PlayerLagTimer[playerid] == -1 ? 0 : 1);
}


forward L_LagPlayer(playerid, lag_delay);

public L_LagPlayer(playerid, lag_delay) {

	#if !defined BlockIpAddress
	new
	    temp_String[32];
	#endif
	
	if(g_LagState[playerid]) {
	
 	#if defined BlockIpAddress
		UnBlockIpAddress(g_PlayerIp[playerid]);
	#else
	    format(temp_String, sizeof(temp_String), "unbanip %s", g_PlayerIp[playerid]);
	#endif
	    g_LagState[playerid] = false;
	}
	else {

	#if defined BlockIpAddress
	    BlockIpAddress(g_PlayerIp[playerid], lag_delay);
	#else
	    format(temp_String, sizeof(temp_String), "banip %s", g_PlayerIp[playerid]);
	#endif
		g_LagState[playerid] = true;
	}
	#if !defined BlockIpAddress
	SendRconCommand(temp_String);
	#endif
	
	return 1;
}

#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif

#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif


#define OnFilterScriptInit  L_LAG_OnFS
#define OnGameModeInit      L_LAG_OnGM
#define OnPlayerConnect     L_LAG_OnPlayerConnect
#define OnPlayerDisconnect  L_LAG_OnPlayerDisconnect


#if defined L_LAG_OnFS
	forward L_LAG_OnFS();
#endif

#if defined L_LAG_OnGM
	forward L_LAG_OnGM();
#endif

#if defined L_LAG_OnPlayerConnect
	forward L_LAG_OnPlayerConnect(playerid);
#endif

#if defined L_LAG_OnPlayerDisconnect
	forward L_LAG_OnPlayerDisconnect(playerid, reason);
#endif

#if !defined ReconnectPlayer
	#define ReconnectPlayer(%0) TogglePlayerLag(%0, true, 20000) //set to  20 seconds, but player will time out before that.
#endif

