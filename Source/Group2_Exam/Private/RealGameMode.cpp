/*
#include "RealGameMode.h"

ARealGameMode::ARealGameMode()
{
	Funds = 200;
}

void ARealGameMode::AddFunds(int32 Amount)
{
	Funds += Amount;
}

bool ARealGameMode::SpendFunds(int32 Amount)
{
	if (Amount <= 0) return true;
	if (Funds < Amount) return false;
	Funds -= Amount;
	return true;
}
*/