#pragma once

enum GameStates
{
	//Empty State
	S_Default,

	//Print States
	S_PrintPlayerName,
	S_PrintClassQuestion,
	S_PrintStatsRequest,
	S_PrintEnemyStats,
	S_PrintWelcomeMessage,
	S_PrintStats,
	S_PrintPlayerTurnMenu,
	S_YouWin,
	S_YouLose,

	S_AttackMissed,

	S_WaitingForStart,
	S_GameBegin,
	S_WaitingForTurn,
	S_PlayerAttack,
	S_PlayerDefend,

	//Packet Handling States
	S_SendStatsRequest,

	S_ReceivingStats,

	//Input States
	S_NameInput,
	S_ClassInput,
	S_PlayerTurnInput,
	S_AttackChoice
};
