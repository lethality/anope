/* NickServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

class CommandNSAList : public Command
{
 public:
	CommandNSAList() : Command("ALIST", 0, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		/*
		 * List the channels that the given nickname has access on
		 *
		 * /ns ALIST [level]
		 * /ns ALIST [nickname] [level]
		 *
		 * -jester
		 */

		const char *nick = NULL;
		const char *lev = NULL;

		NickAlias *na;

		int min_level = 0;
		int is_servadmin = u->nc->IsServicesOper();
		int lev_param = 0;

		if (!is_servadmin)
			/* Non service admins can only see their own levels */
			na = findnick(u->nick);
		else
		{
			/* Services admins can request ALIST on nicks.
			 * The first argument for service admins must
			 * always be a nickname.
			 */
			nick = params.size() ? params[0].c_str() : NULL;
			lev_param = 1;

			/* If an argument was passed, use it as the nick to see levels
			 * for, else check levels for the user calling the command */
			if (nick)
				na = findnick(nick);
			else
				na = findnick(u->nick);
		}

		/* If available, get level from arguments */
		lev = params.size() > lev_param ? params[lev_param].c_str() : NULL;

		/* if a level was given, make sure it's an int for later */
		if (lev) {
			if (!stricmp(lev, "FOUNDER"))
				min_level = ACCESS_FOUNDER;
			else if (!stricmp(lev, "SOP"))
				min_level = ACCESS_SOP;
			else if (!stricmp(lev, "AOP"))
				min_level = ACCESS_AOP;
			else if (!stricmp(lev, "HOP"))
				min_level = ACCESS_HOP;
			else if (!stricmp(lev, "VOP"))
				min_level = ACCESS_VOP;
			else
				min_level = atoi(lev);
		}

		if (!nick_identified(u))
			notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
		else if (is_servadmin && nick && !na)
			notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
		else if (na->status & NS_FORBIDDEN)
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
		else if (min_level <= ACCESS_INVALID || min_level > ACCESS_FOUNDER)
			notice_lang(s_NickServ, u, CHAN_ACCESS_LEVEL_RANGE, ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
		else
		{
			int i, level;
			int chan_count = 0;
			int match_count = 0;
			ChannelInfo *ci;

			notice_lang(s_NickServ, u, is_servadmin ? NICK_ALIST_HEADER_X : NICK_ALIST_HEADER, na->nick);

			for (i = 0; i < 256; ++i)
			{
				for (ci = chanlists[i]; ci; ci = ci->next)
				{
					if ((level = get_access_level(ci, na)))
					{
						++chan_count;

						if (min_level > level)
							continue;

						++match_count;

						if ((ci->flags & CI_XOP) || level == ACCESS_FOUNDER)
						{
							const char *xop;

							xop = get_xop_level(level);

							notice_lang(s_NickServ, u, NICK_ALIST_XOP_FORMAT, match_count, ci->flags & CI_NO_EXPIRE ? '!' : ' ', ci->name, xop, ci->desc ? ci->desc : "");
						}
						else
							notice_lang(s_NickServ, u, NICK_ALIST_ACCESS_FORMAT, match_count, ci->flags & CI_NO_EXPIRE ? '!' : ' ', ci->name, level, ci->desc ? ci->desc : "");
					}
				}
			}

			notice_lang(s_NickServ, u, NICK_ALIST_FOOTER, match_count, chan_count);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (u->nc && u->nc->IsServicesOper())
			notice_help(s_NickServ, u, NICK_SERVADMIN_HELP_ALIST);
		else
			notice_help(s_NickServ, u, NICK_HELP_ALIST);

		return true;
	}
};

class NSAList : public Module
{
 public:
	NSAList(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSAList(), MOD_UNIQUE);
	}
	void NickServHelp(User *u)
	{
		notice_lang(s_NickServ, u, NICK_HELP_CMD_ALIST);
	}
};

MODULE_INIT("ns_alist", NSAList)
