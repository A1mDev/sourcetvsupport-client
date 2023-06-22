#ifndef _INCLUDE_L4D2_CONSTANTS_VSP_H_
#define _INCLUDE_L4D2_CONSTANTS_VSP_H_

// Constants that exist in the game (except for constant MAX_NUMBER_TEAMS)
#define TEAM_ANY				-1
#define TEAM_INVALID			-1

#define TEAM_UNASSIGNED			0 // not assigned to a team

#define TEAM_SPECTATOR			1 // spectator team
#define TEAM_SURVIVOR			2
#define TEAM_ZOMBIE				3
#define TEAM_L4D1_SURVIVOR		4 // Used for maps where there are survivors from the first chapter and from the second, for example c7m3_port

enum ZombieClassType
{
	zClass_Common = 0,
	zClass_Smoker = 1,
	zClass_Boomer,
	zClass_Hunter,
	zClass_Spitter,
	zClass_Jockey,
	zClass_Charger,
	zClass_Witch,
	zClass_Tank,
	zClass_Survivor,

	zClass_Size //10 size
};

#endif // _INCLUDE_L4D2_CONSTANTS_VSP_H_