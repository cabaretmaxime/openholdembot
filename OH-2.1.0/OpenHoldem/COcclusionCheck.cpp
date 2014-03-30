// Heuristic occlusion-check for rebuy.

#include "stdafx.h"
#include "CGameState.h"
#include "COcclusionCheck.h"
#include "CSCraper.h"
#include "CSymbols.h"

COcclusionCheck *p_occlusioncheck = NULL;


COcclusionCheck::COcclusionCheck()
{}

COcclusionCheck::~COcclusionCheck()
{}

bool COcclusionCheck::UserChairKnown()
{
	if (p_symbols->user_chair_confirmed())
	{
		return true;
	}
	else
	{
		write_log(2, "COcclusionCheck::UserChairKnown: false\n");
		return false;
	}
}

bool COcclusionCheck::UserBalanceNonZero()
{
	int userchair = p_symbols->sym()->userchair;
	if (UserChairKnown() && (p_symbols->sym()->balance > 0))
	{
		return true;
	}
	else
	{
		write_log(2, "COcclusionCheck::UserBalanceNonZero: false\n");
		return false;
	}
}

bool COcclusionCheck::UserNameKnown()
{	
	int Userchair = p_symbols->sym()->userchair;
	if ((Userchair < 0) || (Userchair > 9))
	{
		write_log(2, "COcclusionCheck::UserNameKnown: false; chair out of range\n");
		return false;
	}
	else if (UserChairKnown() && (p_scraper->player_name(Userchair) != ""))
	{	
		return true;
	}
	else
	{
		write_log(2, "COcclusionCheck::UserNameKnown: false\n");
		return false;
	}
}

bool COcclusionCheck::AnyOpponentNameKnown()
{
	int Userchair = p_symbols->sym()->userchair;
	for (int i=0; i<=9; i++)
	{
		if ((i != Userchair) && (p_scraper->player_name(i) != ""))
		{
			return true;
		}
	}
	write_log(2, "COcclusionCheck::AnyOpponentNameKnown: false\n");
	return false;
}

bool COcclusionCheck::AnyApponentBalanceNonZero()
{
	int Userchair = p_symbols->sym()->userchair;
	for (int i=0; i<=9; i++)
	{
		if ((i != Userchair) && (p_symbols->sym()->balance[i] > 0))
		{
			return true;
		}
	}
	write_log(2, "COcclusionCheck::AnyApponentBalanceNonZero: false\n");
	return false;
}

bool COcclusionCheck::UserBalanceOccluded()
{
	// Result:
	//  * true:  probably occlusion.
	//  * false: probably no occlusion.
	if (!UserChairKnown())
	{
		// Userchair not known; 
		// Something went completely wrong.
		// For sure the balance is not known;
		// We treat the table as "occluded".
		return true;
	}
	else if (UserBalanceNonZero())
	{
		// Balance known (non-zero).
		// Probably not occluded.
		return false;
	}
	else if (UserNameKnown())
	{
		// Username known, but balance zero.
		// Username (and probably balance) not occluded.
		return false;
	}
	else if (AnyOpponentNameKnown())
	{
		// Other name(s) known, but username not.
		// Username (and probably balance) occluded.
		return true;
	}
	else if(AnyApponentBalanceNonZero())
	{
		// No names known, at least one other balance known (non-zeor).
		// TM does probably not support names, no occlusion.
		return false;
	}
	else
	{
		// No names known, no other balances known or all zero.
		// Probably full table occluded.
		return true;
	}
}
