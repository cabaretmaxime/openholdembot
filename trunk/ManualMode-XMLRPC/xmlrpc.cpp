#include "stdafx.h"

#include "xmlrpc.h"
#include "ManualModeDlg.h"

xmlrpc_c::serverAbyss* xServer = NULL;

class MMDlgMethod: public xmlrpc_c::method
{
	protected:
		CManualModeDlg* dlg;
	public:
		MMDlgMethod(CManualModeDlg* pdlg)
		{
			dlg = pdlg;
		}
};

class SetSeated: public MMDlgMethod
{
	public:
		SetSeated(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			int const chair(paramList.getInt(0));
			// FIXME: Check if chair number is correct.
			bool const seated(paramList.getBoolean(1));
			dlg->SetSeated(chair, seated);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetActive: public MMDlgMethod
{
	public:
		SetActive(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			int const chair(paramList.getInt(0));
			// FIXME: Check if chair number is correct.
			bool const active(paramList.getBoolean(1));
			dlg->SetActive(chair, active);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetDealer: public MMDlgMethod
{
	public:
		SetDealer(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			int const chair(paramList.getInt(0));
			// FIXME: Check if chair number is correct.
			dlg->SetDealer(chair);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetBalance: public MMDlgMethod
{
	public:
		SetBalance(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			int const chair(paramList.getInt(0));
			// FIXME: Check if chair number is correct.
			double const balance(paramList.getDouble(1));
			dlg->SetBalance(chair, balance);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetBet: public MMDlgMethod
{
	public:
		SetBet(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			int const chair(paramList.getInt(0));
			// FIXME: Check if chair number is correct.
			double const bet(paramList.getDouble(1));
			dlg->SetBet(chair, bet);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetSBlind: public MMDlgMethod
{
	public:
		SetSBlind(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			double const sb(paramList.getDouble(0));
			dlg->SetSBlind(sb);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetBBlind: public MMDlgMethod
{
	public:
		SetBBlind(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			double const bb(paramList.getDouble(0));
			dlg->SetBBlind(bb);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetBBet: public MMDlgMethod
{
	public:
		SetBBet(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			double const bbet(paramList.getDouble(0));
			dlg->SetBBet(bbet);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetAnte: public MMDlgMethod
{
	public:
		SetAnte(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			double const ante(paramList.getDouble(0));
			dlg->SetAnte(ante);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetPot: public MMDlgMethod
{
	public:
		SetPot(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			double const pot(paramList.getDouble(0));
			dlg->SetPot(pot);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetGType: public MMDlgMethod
{
	public:
		SetGType(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			std::string const gt(paramList.getString(0));
			dlg->SetGType(gt);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetHandNumber: public MMDlgMethod
{
	public:
		SetHandNumber(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			int const h(paramList.getInt(0));
			dlg->SetHandNumber(h);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetNetwork: public MMDlgMethod
{
	public:
		SetNetwork(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			std::string const net(paramList.getString(0));
			dlg->SetNetwork(net);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetTournament: public MMDlgMethod
{
	public:
		SetTournament(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			bool const t(paramList.getBoolean(0));
			dlg->SetTournament(t);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class PostSB: public MMDlgMethod
{
	public:
		PostSB(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			int const chair(paramList.getInt(0));
			dlg->PostSB(chair);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class PostBB: public MMDlgMethod
{
	public:
		PostBB(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			int const chair(paramList.getInt(0));
			dlg->PostBB(chair);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetButton: public MMDlgMethod
{
	public:
		SetButton(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			std::string const button(paramList.getString(0));
			bool const a(paramList.getBoolean(1));
			dlg->SetButton(button[0], a);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetCards: public MMDlgMethod
{
	public:
		SetCards(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			int const chair(paramList.getInt(0));
			// FIXME: Check if chair number is correct.
			std::string const c1(paramList.getString(1));
			std::string const c2(paramList.getString(2));
			dlg->SetCards(chair, c1, c2);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetFlopCards: public MMDlgMethod
{
	public:
		SetFlopCards(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			std::string const c1(paramList.getString(0));
			std::string const c2(paramList.getString(1));
			std::string const c3(paramList.getString(2));
			dlg->SetFlopCards(c1, c2, c3);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetTurnCard: public MMDlgMethod
{
	public:
		SetTurnCard(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			std::string const c1(paramList.getString(0));
			dlg->SetTurnCard(c1);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetRiverCard: public MMDlgMethod
{
	public:
		SetRiverCard(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			std::string const c1(paramList.getString(0));
			dlg->SetRiverCard(c1);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class SetName: public MMDlgMethod
{
	public:
		SetName(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			int const chair(paramList.getInt(0));
			// FIXME: Check if chair number is correct.
			std::string const name(paramList.getString(1));
			dlg->SetName(chair, name);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class DoFold: public MMDlgMethod
{
	public:
		DoFold(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			int const chair(paramList.getInt(0));
			// FIXME: Check if chair number is correct.
			dlg->DoFold(chair);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class DoCall: public MMDlgMethod
{
	public:
		DoCall(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			int const chair(paramList.getInt(0));
			// FIXME: Check if chair number is correct.
			dlg->DoCall(chair);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class DoRaise: public MMDlgMethod
{
	public:
		DoRaise(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			double raise_amt = 0;
			int const chair(paramList.getInt(0));
			// FIXME: Check if chair number is correct.
			if(paramList.size() > 1)
				raise_amt = paramList.getDouble(1);

			dlg->DoRaise(chair, raise_amt);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class DoAllin: public MMDlgMethod
{
	public:
		DoAllin(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			int const chair(paramList.getInt(0));
			// FIXME: Check if chair number is correct.
			dlg->DoAllin(chair);
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class Refresh: public MMDlgMethod
{
	public:
		Refresh(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			dlg->Refresh();
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

class ProvideEventsHandling: public MMDlgMethod
{
	public:
		ProvideEventsHandling(CManualModeDlg* pdlg): MMDlgMethod(pdlg){}

		void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP)
		{
			dlg->ProvideEventsHandling();
			*retvalP = xmlrpc_c::value_boolean(true);
		}
};

void xServerThread(void* dlg)
{
	xmlrpc_c::registry* myRegistry = NULL;
	myRegistry = new xmlrpc_c::registry;

	xmlrpc_c::methodPtr const SetSeatedP(new SetSeated((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetSeated", SetSeatedP);

	xmlrpc_c::methodPtr const SetActiveP(new SetActive((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetActive", SetActiveP);

	xmlrpc_c::methodPtr const SetDealerP(new SetDealer((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetDealer", SetDealerP);

	xmlrpc_c::methodPtr const SetBalanceP(new SetBalance((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetBalance", SetBalanceP);

	xmlrpc_c::methodPtr const SetBetP(new SetBet((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetBet", SetBetP);

	xmlrpc_c::methodPtr const SetSBlindP(new SetSBlind((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetSBlind", SetSBlindP);

	xmlrpc_c::methodPtr const SetBBlindP(new SetBBlind((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetBBlind", SetBBlindP);

	xmlrpc_c::methodPtr const SetBBetP(new SetBBet((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetBBet", SetBBetP);

	xmlrpc_c::methodPtr const SetAnteP(new SetAnte((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetAnte", SetAnteP);

	xmlrpc_c::methodPtr const SetPotP(new SetPot((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetPot", SetPotP);

	xmlrpc_c::methodPtr const SetGTypeP(new SetGType((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetGType", SetGTypeP);

	xmlrpc_c::methodPtr const SetHandNumberP(new SetHandNumber((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetHandNumber", SetHandNumberP);

	xmlrpc_c::methodPtr const SetNetworkP(new SetNetwork((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetNetwork", SetNetworkP);

	xmlrpc_c::methodPtr const SetTournamentP(new SetTournament((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetTournament", SetTournamentP);

	xmlrpc_c::methodPtr const PostSBP(new PostSB((CManualModeDlg*) dlg));
	myRegistry->addMethod("PostSB", PostSBP);

	xmlrpc_c::methodPtr const PostBBP(new PostBB((CManualModeDlg*) dlg));
	myRegistry->addMethod("PostBB", PostBBP);

	xmlrpc_c::methodPtr const SetButtonP(new SetButton((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetButton", SetButtonP);

	xmlrpc_c::methodPtr const SetCardsP(new SetCards((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetCards", SetCardsP);

	xmlrpc_c::methodPtr const SetFlopCardsP(new SetFlopCards((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetFlopCards", SetFlopCardsP);

	xmlrpc_c::methodPtr const SetTurnCardP(new SetTurnCard((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetTurnCard", SetTurnCardP);

	xmlrpc_c::methodPtr const SetRiverCardP(new SetRiverCard((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetRiverCard", SetRiverCardP);

	xmlrpc_c::methodPtr const SetNameP(new SetName((CManualModeDlg*) dlg));
	myRegistry->addMethod("SetName", SetNameP);

	xmlrpc_c::methodPtr const DoFoldP(new DoFold((CManualModeDlg*) dlg));
	myRegistry->addMethod("DoFold", DoFoldP);

	xmlrpc_c::methodPtr const DoCallP(new DoCall((CManualModeDlg*) dlg));
	myRegistry->addMethod("DoCall", DoCallP);

	xmlrpc_c::methodPtr const DoRaiseP(new DoRaise((CManualModeDlg*) dlg));
	myRegistry->addMethod("DoRaise", DoRaiseP);

	xmlrpc_c::methodPtr const DoAllinP(new DoAllin((CManualModeDlg*) dlg));
	myRegistry->addMethod("DoAllin", DoAllinP);

	xmlrpc_c::methodPtr const RefreshP(new Refresh((CManualModeDlg*) dlg));
	myRegistry->addMethod("Refresh", RefreshP);

	xmlrpc_c::methodPtr const ProvideEventsHandlingP(new ProvideEventsHandling((CManualModeDlg*) dlg));
	myRegistry->addMethod("ProvideEventsHandling", ProvideEventsHandlingP);

	xServer = new xmlrpc_c::serverAbyss(
			*myRegistry,
			9092              // TCP port on which to listen
			);

	xServer->run();
}

