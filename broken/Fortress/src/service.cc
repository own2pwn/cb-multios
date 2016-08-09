/*
 * Copyright (c) 2015 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <cstdio.h>
#include <cstring.h>

#include "req.h"
#include "data.h"
#include "fortress.h"

int g_randIdx = 0;
char *g_rand = NULL;
bool g_hidden = true;

cgc_CFortress g_fort;
const char *banner = "\
\n\
 _______  _______  _______ _________ _______  _______  _______  _______\n\
(  ____ \\(  ___  )(  ____ )\\__   __/(  ____ )(  ____ \\(  ____ \\(  ____ \\\n\
| (    \\/| (   ) || (    )|   ) (   | (    )|| (    \\/| (    \\/| (    \\/\n\
| (__    | |   | || (____)|   | |   | (____)|| (__    | (_____ | (_____\n\
|  __)   | |   | ||     __)   | |   |     __)|  __)   (_____  )(_____  )\n\
| (      | |   | || (\\ (      | |   | (\\ (   | (            ) |      ) |\n\
| )      | (___) || ) \\ \\__   | |   | ) \\ \\__| (____/\\/\\____) |/\\____) |\n\
|/       (_______)|/   \\__/   )_(   |/   \\__/(_______/\\_______)\\_______)\n\
                                                                  v0.1 %c\n\
\n\
";

void cgc_CreateMoreMissions()
{
    int i, j;
    int r = (g_rand[g_randIdx % 4096] & 0xFF) % 4 + 1;
    g_randIdx += 2;
    int n = sizeof(g_missions) / sizeof(g_missions[0]);
    for (i = 0; i < r; i++)
    {
        int nn = sizeof(g_skills) / sizeof(g_skills[0]);
        int group = (g_rand[g_randIdx % 4096] & 0xFF) % 3 + 1;
        g_randIdx += 2;
        cgc_CRequirement::Type type = cgc_CRequirement::Type::NOTHING;
        for (j = 0; j < group; j++)
        {
            type |= g_skills[(g_rand[g_randIdx % 4096] & 0xFF) % nn].counter;
            g_randIdx += 2;
        }
        int supply = (g_rand[g_randIdx % 4096] & 0xFF) % 30 + 1;
        g_randIdx += 2;
        int duration = (g_rand[g_randIdx % 4096] & 0xFF) % 10 + 1;
        g_randIdx += 2;
        if (group != 1)
            group--;
        cgc_CRequirement req (type, g_fort.cgc_GetAvgLevel(), supply, duration, group);
        cgc_CReward reward (g_fort.cgc_GetAvgLevel() * group * 100, supply * (supply % 4));
        cgc_CMission *mission = new cgc_CMission(g_missions[(g_rand[g_randIdx % 4096] & 0xFF) % n].name, req, reward);
        g_randIdx += 2;
        g_fort.cgc_GetMissions().cgc_Append(mission);
    }
}

void cgc_InitializeMissions()
{
    cgc_CreateMoreMissions();
}

void cgc_InitializeSkills()
{
    int i;
    for (i = 0; i < sizeof(g_skills) / sizeof(g_skills[0]); i++)
    {
        cgc_CSkill *skill = new cgc_CSkill(g_skills[i].name, g_skills[i].counter, g_skills[i].price);
        g_fort.cgc_GetSkills().cgc_Append(skill);
    }
}

void cgc_InitializeExplorers()
{
    int i;
    cgc_CSkill *no_skill = new cgc_CSkill("No Skill", cgc_CRequirement::Type::NOTHING, 0);
    for (i = 0; i < sizeof(g_explorers) / sizeof(g_explorers[0]); i++)
    {
        cgc_CExplorer *explorer = new cgc_CExplorer(g_explorers[i].name);
        cgc_CList<cgc_CSkill *>& skills = g_fort.cgc_GetSkills();
        int r = (g_rand[g_randIdx % 4096] & 0xFF) % (skills.cgc_GetSize());
        g_randIdx += 2;
        explorer->cgc_ReplaceSkill(skills.cgc_GetAt(r), 0);
        explorer->cgc_ReplaceSkill(no_skill, 1);
        g_fort.cgc_GetExplorers().cgc_Append(explorer);
    }
}

void cgc_PrintWelcomeBanner(char c)
{
    cgc_fprintf(stdout, banner, c);
}

void cgc_PrintMainMenu()
{
    int n, numMissions;
    cgc_fbuffered(stdout, 1);
    numMissions = g_fort.cgc_GetMissions().cgc_GetSize() - g_fort.cgc_GetNumAvailableMissions();
    n = cgc_fprintf(stdout, "\nDay-%03d => [%d mission] | [%d explorer] | [%d supply]\n", g_fort.cgc_GetDay(), numMissions, g_fort.cgc_GetNumHiredExplorers(), g_fort.cgc_GetSupply());
    for (; n > 1; n--)
        cgc_fprintf(stdout, "-");
    cgc_fprintf(stdout, "\n");
    cgc_fprintf(stdout, "0. Next day!\n");
    cgc_fprintf(stdout, "1. Missions\n");
    cgc_fprintf(stdout, "2. Explorers\n");
    cgc_fprintf(stdout, "3. Give up\n");
    cgc_fbuffered(stdout, 0);
}

void cgc_PrintMissionMenu()
{
    cgc_fbuffered(stdout, 1);
    cgc_fprintf(stdout, "\n]]] Missions [[[\n");
    cgc_fprintf(stdout, "1. Send mission (%d available)\n", g_fort.cgc_GetNumAvailableMissions());
    cgc_fprintf(stdout, "2. View current missions\n");
    cgc_fprintf(stdout, "3. Back\n");
    cgc_fbuffered(stdout, 0);
}

void cgc_PrintExplorerMenu()
{
    cgc_fbuffered(stdout, 1);
    cgc_fprintf(stdout, "\n]]] Explorers [[[\n");
    cgc_fprintf(stdout, "1. Hire explorer\n");
    cgc_fprintf(stdout, "2. View current explorers\n");
    cgc_fprintf(stdout, "3. cgc_Remove explorer\n");
    cgc_fprintf(stdout, "4. Learn skills\n");
    cgc_fprintf(stdout, "5. Back\n");
    if (!g_hidden)
        cgc_fprintf(stdout, "6. Change explorer name\n");
    cgc_fbuffered(stdout, 0);
}

void cgc_PrintMissions(cgc_CList<cgc_CMission *>& missions)
{
    int i;
    for (i = 0; i < missions.cgc_GetSize(); i++)
    {
        cgc_CMission *m = missions.cgc_GetAt(i);
        char *s = m->cgc_GetReqTypeString();
        cgc_fprintf(stdout, "%d. %s [LVL %d] [GRP %d] [DUR %d] [COST %d] <%s>\n", i, m->cgc_GetName(), m->cgc_GetReqLevel(), m->cgc_GetReqGroup(), m->cgc_GetDuration(), m->cgc_GetReqSupply(), s);
        cgc_fprintf(stdout, "   ==> +%d supply, +%d experience\n", m->cgc_GetRewardSupply(), m->cgc_GetRewardExp());
        cgc_free(s);
    }
}

void cgc_PrintExplorers(cgc_CList<cgc_CExplorer *>& explorers)
{
    int i;
    cgc_fbuffered(stdout, 1);
    for (i = 0; i < explorers.cgc_GetSize(); i++)
    {
        cgc_CExplorer *e = explorers.cgc_GetAt(i);
        int totalExp = e->cgc_GetExp() + e->cgc_GetNextExp();
        cgc_fprintf(stdout, "%d. %s [LVL %d] [EXP %d/%d] (%s)\n", i, e->cgc_GetName(), e->cgc_GetLevel(), e->cgc_GetExp(), totalExp, e->cgc_IsHired() ? "hired" : "not-hired");
        cgc_CSkill** skills = e->cgc_GetSkills();
        char *s = cgc_CRequirement::cgc_ReqTypeToString(skills[0]->cgc_GetCounter());
        cgc_fprintf(stdout, "   ==> Skill #1: %s <%s>\n", skills[0]->cgc_GetName(), cgc_strcmp(s, "") == 0 ? "None" : s);
        cgc_free(s);
        s = cgc_CRequirement::cgc_ReqTypeToString(skills[1]->cgc_GetCounter());
        cgc_fprintf(stdout, "   ==> Skill #2: %s <%s>\n", skills[1]->cgc_GetName(), cgc_strcmp(s, "") == 0 ? "None" : s);
        cgc_free(s);
    }
    cgc_fbuffered(stdout, 0);
}

void cgc_HandleSendMission()
{
    char buf[64];
    if (g_fort.cgc_GetNumAvailableMissions() == 0)
    {
        cgc_fprintf(stdout, "\nNo missions available. cgc_Check back later.\n");
        return;
    }
    int i;
    cgc_CList<cgc_CMission *>& missions = g_fort.cgc_GetMissions();
    cgc_CList<cgc_CMission *> availMissions;
    for (i = 0; i < missions.cgc_GetSize(); i++)
    {
        cgc_CMission *m = missions.cgc_GetAt(i);
        if (m->cgc_IsAvail())
            availMissions.cgc_Append(m);
    }
    cgc_fprintf(stdout, "\n... Available missions ...\n");
    cgc_PrintMissions(availMissions);
    if (g_fort.cgc_GetNumAvailableExplorers() == 0)
    {
        cgc_fprintf(stdout, "\nNo explorers available.\n");
        return;
    }
    cgc_fprintf(stdout, "\nselect> ");
    if (cgc_freaduntil(buf, sizeof(buf), '\n', stdin) < 0)
        cgc_exit(0);
    int idx = cgc_strtoul(buf, NULL, 10);
    if (idx >= availMissions.cgc_GetSize())
        cgc_fprintf(stdout, "Invalid mission.\n");
    else
    {
        int n;
        cgc_CMission *m = availMissions.cgc_GetAt(idx);
        if (m->cgc_GetReqSupply() > g_fort.cgc_GetSupply())
        {
            cgc_fprintf(stdout, "You don't have enough supply.\n");
            return;
        }
        cgc_CList<cgc_CExplorer *> selectedExps;
        cgc_CList<cgc_CExplorer *> availExps;
        cgc_CList<cgc_CExplorer *>& explorers = g_fort.cgc_GetExplorers();
        for (i = 0; i < explorers.cgc_GetSize(); i++)
        {
            cgc_CExplorer *e = explorers.cgc_GetAt(i);
            if (e->cgc_IsAvail() && e->cgc_IsHired())
                availExps.cgc_Append(e);
        }
        cgc_fprintf(stdout, "\n... Available explorers ...\n");
        cgc_PrintExplorers(availExps);
        cgc_fprintf(stdout, "\nselect> ");
        if (cgc_freaduntil(buf, sizeof(buf), '\n', stdin) < 0)
            cgc_exit(0);
        char *tmp = buf, *id = NULL;
        while ((id = cgc_strsep(&tmp, ",")))
        {
            if (cgc_strlen(id) == 0)
                continue;
            idx = cgc_strtoul(id, NULL, 10);
            if (idx >= availExps.cgc_GetSize())
                continue;
            cgc_CExplorer *e = availExps.cgc_GetAt(idx);
            if (!selectedExps.cgc_Has(e))
                selectedExps.cgc_Append(e);
        }
        if (selectedExps.cgc_GetSize() == 0)
        {
            cgc_fprintf(stdout, "No explorer is selected.\n");
            return;
        }

        char *s;
        cgc_fbuffered(stdout, 1);
        cgc_CMissionEstimate* est = m->cgc_Check(selectedExps);
        cgc_fprintf(stdout, "\n////////////////////////////\n");
        cgc_fprintf(stdout, "%s\n", m->cgc_GetName());
        cgc_fprintf(stdout, "////////////////////////////\n");
        cgc_fprintf(stdout, " - Success: %d%%\n", est->cgc_GetSuccessRate());
        s = cgc_CRequirement::cgc_ReqTypeToString(est->cgc_GetMissingTypes());
        cgc_fprintf(stdout, " - Missing: %s\n", cgc_strcmp(s, "") == 0 ? "None" : s);
        cgc_free(s);
        cgc_fprintf(stdout, " - Needed: %d\n", est->cgc_GetGroupNeeded());
        cgc_fprintf(stdout, " - Supplied: %d\n", est->cgc_GetGroupSupplied());
        cgc_fprintf(stdout, "////////////////////////////\n");
        cgc_fprintf(stdout, "confirm?> ");
        cgc_fbuffered(stdout, 0);
        if (cgc_freaduntil(buf, sizeof(buf), '\n', stdin) < 0)
            cgc_exit(0);
        if (cgc_strcmp(buf, "y") == 0 || cgc_strcmp(buf, "Y") == 0)
        {
            m->cgc_Execute(selectedExps);
            m->cgc_SetSuccessRate(est->cgc_GetSuccessRate());
            g_fort.cgc_SubSupply(m->cgc_GetReqSupply());
        }
        delete est;
    }
}

void cgc_HandleViewMissions()
{
    int i, n = 0;
    cgc_CList<cgc_CMission *>& missions = g_fort.cgc_GetMissions();
    cgc_fprintf(stdout, "\n... Current missions ...\n");
    for (i = 0; i < missions.cgc_GetSize(); i++)
    {
        cgc_CMission *m = missions.cgc_GetAt(i);
        if (!m->cgc_IsAvail())
        {
            cgc_fprintf(stdout, "%d. %s [%d%%] -- %d days left\n", n, m->cgc_GetName(), m->cgc_GetSuccessRate(), m->cgc_GetDuration());
            cgc_fprintf(stdout, "   ==> +%d supply, +%d experience\n", m->cgc_GetRewardSupply(), m->cgc_GetRewardExp());
            n++;
        }
    }
    if (n == 0)
        cgc_fprintf(stdout, "No missions :(\n");
}

void cgc_HandleHireExplorer()
{
    int i;
    char buf[32];
    cgc_CList<cgc_CExplorer *>& exps = g_fort.cgc_GetExplorers();
    cgc_fprintf(stdout, "\n... Available explorers ...\n");
    if (exps.cgc_GetSize() == 0 || g_fort.cgc_GetNumHiredExplorers() == exps.cgc_GetSize())
    {
        cgc_fprintf(stdout, "No explorers to hire :(\n");
        return;
    }
    if (g_fort.cgc_GetNumHiredExplorers() == g_fort.k_maxNumExplorers)
    {
        cgc_fprintf(stdout, "Can't hire more :(\n");
        return;
    }
    cgc_CList<cgc_CExplorer *> unhiredExps;
    for (i = 0; i < exps.cgc_GetSize(); i++)
    {
        cgc_CExplorer *e = exps.cgc_GetAt(i);
        if (!e->cgc_IsHired())
            unhiredExps.cgc_Append(e);
    }
    cgc_PrintExplorers(unhiredExps);
    cgc_fprintf(stdout, "\nselect> ");
    if (cgc_freaduntil(buf, sizeof(buf), '\n', stdin) < 0)
        cgc_exit(0);
    int idx = cgc_strtoul(buf, NULL, 10);
    if (idx >= unhiredExps.cgc_GetSize())
    {
        cgc_fprintf(stdout, "Invalid explorer.\n");
        return;
    }
    cgc_CExplorer *e = unhiredExps.cgc_GetAt(idx);
    e->cgc_SetHired(true);
}

void cgc_HandleViewExplorers()
{
    int i;
    cgc_CList<cgc_CExplorer *>& exps = g_fort.cgc_GetExplorers();
    cgc_CList<cgc_CExplorer *> hiredExps;
    for (i = 0; i < exps.cgc_GetSize(); i++)
    {
        cgc_CExplorer *e = exps.cgc_GetAt(i);
        if (e->cgc_IsHired())
            hiredExps.cgc_Append(e);
    }
    cgc_fprintf(stdout, "\n... Current explorers (%d/%d) ...\n", hiredExps.cgc_GetSize(), g_fort.k_maxNumExplorers);
    if (hiredExps.cgc_GetSize() == 0)
    {
        cgc_fprintf(stdout, "No explorers :(\n");
        return;
    }
    cgc_PrintExplorers(hiredExps);
}

void cgc_HandleRemoveExplorer()
{
    int i;
    char buf[8];
    cgc_CList<cgc_CExplorer *>& exps = g_fort.cgc_GetExplorers();
    cgc_CList<cgc_CExplorer *> hiredExps;
    for (i = 0; i < exps.cgc_GetSize(); i++)
    {
        cgc_CExplorer *e = exps.cgc_GetAt(i);
        if (e->cgc_IsHired() && e->cgc_IsAvail())
            hiredExps.cgc_Append(e);
    }
    cgc_fprintf(stdout, "\n... Removable explorers ...\n");
    if (hiredExps.cgc_GetSize() == 0)
    {
        cgc_fprintf(stdout, "No explorers :(\n");
        return;
    }
    cgc_PrintExplorers(hiredExps);
    cgc_fprintf(stdout, "\nselect> ");
    if (cgc_freaduntil(buf, sizeof(buf), '\n', stdin) < 0)
        cgc_exit(0);
    int idx = cgc_strtoul(buf, NULL, 10);
    if (idx >= hiredExps.cgc_GetSize())
    {
        cgc_fprintf(stdout, "Invalid explorer.\n");
        return;
    }
    cgc_CExplorer *e = hiredExps.cgc_GetAt(idx);
    e->cgc_SetHired(false);
}

void cgc_HandleLearnSkills()
{
    int i;
    char buf[8];
    cgc_CList<cgc_CExplorer *>& exps = g_fort.cgc_GetExplorers();
    cgc_CList<cgc_CExplorer *> availExps;
    for (i = 0; i < exps.cgc_GetSize(); i++)
    {
        cgc_CExplorer *e = exps.cgc_GetAt(i);
        if (e->cgc_IsHired() && e->cgc_IsAvail())
            availExps.cgc_Append(e);
    }
    cgc_fprintf(stdout, "\n... Available explorers ...\n");
    if (availExps.cgc_GetSize() == 0)
    {
        cgc_fprintf(stdout, "No explorers :(\n");
        return;
    }
    cgc_PrintExplorers(availExps);
    cgc_fprintf(stdout, "\nselect> ");
    if (cgc_freaduntil(buf, sizeof(buf), '\n', stdin) < 0)
        cgc_exit(0);
    int idx = cgc_strtoul(buf, NULL, 10);
    if (idx >= availExps.cgc_GetSize())
    {
        cgc_fprintf(stdout, "Invalid explorer.\n");
        return;
    }
    cgc_CExplorer *e = availExps.cgc_GetAt(idx);
    cgc_CList<cgc_CSkill *>& skills = g_fort.cgc_GetSkills();
    cgc_fprintf(stdout, "\n... Skills ...\n");
    for (i = 0; i < skills.cgc_GetSize(); i++)
    {
        cgc_CSkill *s = skills.cgc_GetAt(i);
        char *str = cgc_CRequirement::cgc_ReqTypeToString(s->cgc_GetCounter());
        cgc_fprintf(stdout, "%d. [%s] (counters <%s>) - %d supply\n", i, s->cgc_GetName(), str, s->cgc_GetPrice());
        cgc_free(str);
    }
    cgc_fprintf(stdout, "\nselect> ");
    if (cgc_freaduntil(buf, sizeof(buf), '\n', stdin) < 0)
        cgc_exit(0);
    idx = cgc_strtoul(buf, NULL, 10);
    if (idx >= skills.cgc_GetSize())
    {
        cgc_fprintf(stdout, "Invalid skill.\n");
        return;
    }
    cgc_CSkill *s = skills.cgc_GetAt(idx);
    if (s->cgc_GetPrice() > g_fort.cgc_GetSupply())
    {
        cgc_fprintf(stdout, "Insufficient supply.\n");
        return;
    }
    cgc_fprintf(stdout, "\nslot? (1,2)> ");
    if (cgc_freaduntil(buf, sizeof(buf), '\n', stdin) < 0)
        cgc_exit(0);
    idx = cgc_strtoul(buf, NULL, 10);
    if (idx != 1 && idx != 2)
    {
        cgc_fprintf(stdout, "Invalid slot.\n");
        return;
    }
    e->cgc_ReplaceSkill(s, idx - 1);
    g_fort.cgc_SubSupply(s->cgc_GetPrice());
}

void cgc_HandleChangeName()
{
    int i;
    char buf[8], name[512];
    cgc_CList<cgc_CExplorer *>& exps = g_fort.cgc_GetExplorers();
    cgc_CList<cgc_CExplorer *> availExps;
    for (i = 0; i < exps.cgc_GetSize(); i++)
    {
        cgc_CExplorer *e = exps.cgc_GetAt(i);
        if (e->cgc_IsHired() && e->cgc_IsAvail())
            availExps.cgc_Append(e);
    }
    cgc_fprintf(stdout, "\n... Available explorers ...\n");
    if (availExps.cgc_GetSize() == 0)
    {
        cgc_fprintf(stdout, "No explorers :(\n");
        return;
    }
    for (i = 0; i < availExps.cgc_GetSize(); i++)
        cgc_fprintf(stdout, "%d. %s\n", i, availExps.cgc_GetAt(i)->cgc_GetName());
    cgc_fprintf(stdout, "\nselect> ");
    if (cgc_freaduntil(buf, sizeof(buf), '\n', stdin) < 0)
        cgc_exit(0);
    int idx = cgc_strtoul(buf, NULL, 10);
    if (idx >= availExps.cgc_GetSize())
    {
        cgc_fprintf(stdout, "Invalid explorer.\n");
        return;
    }
    cgc_CExplorer *e = availExps.cgc_GetAt(idx);
    cgc_fprintf(stdout, "\nNew name? ");
    if (cgc_freaduntil(name, sizeof(name), '\n', stdin) < 0)
        cgc_exit(0);
#ifdef PATCHED_1
    name[cgc_CExplorer::k_maxNameLength] = '\0';
#endif
    e->cgc_ChangeName(name);
}

extern "C" int __attribute__((fastcall)) main(int secret_page_i, char *unused[])
{
    char buf[6], nameBuf[32];

    g_rand = (char *) secret_page_i;

    if (cgc_freaduntil(nameBuf, sizeof(nameBuf), '\n', stdin) < 0)
        cgc_exit(0);
    g_fort.cgc_SetOwner(nameBuf);

    cgc_PrintWelcomeBanner(g_fort.cgc_GetOwner()[0]);
    cgc_InitializeMissions();
    cgc_InitializeSkills();
    cgc_InitializeExplorers();

    while (1)
    {
        cgc_PrintMainMenu();
        g_hidden = !(g_fort.cgc_GetSupply() == 1337 && g_fort.cgc_GetDay() == 1);
        if (cgc_freaduntil(buf, sizeof(buf), '\n', stdin) < 0)
            break;
        if (cgc_strcmp(buf, "0") == 0)
        {
            g_fort.cgc_NextDay(1);
            if (g_fort.cgc_GetNumAvailableMissions() < 3)
                cgc_CreateMoreMissions();
        }
        else if (cgc_strtoul(buf, NULL, 10) == 1)
        {
            while (1)
            {
                cgc_PrintMissionMenu();
                if (cgc_freaduntil(buf, sizeof(buf), '\n', stdin) < 0)
                    break;
                if (cgc_strcmp(buf, "1") == 0)
                    cgc_HandleSendMission();
                else if (cgc_strcmp(buf, "2") == 0)
                    cgc_HandleViewMissions();
                else if (cgc_strcmp(buf, "3") == 0)
                    break;
                else
                    cgc_fprintf(stdout, "Invalid option.\n");
            }
        }
        else if (cgc_memcmp(buf, "2\0", 2) == 0)
        {
            while (1)
            {
                cgc_PrintExplorerMenu();
                if (cgc_freaduntil(buf, sizeof(buf), '\n', stdin) < 0)
                    break;
                if (cgc_strcmp(buf, "1") == 0)
                    cgc_HandleHireExplorer();
                else if (cgc_strcmp(buf, "2") == 0)
                    cgc_HandleViewExplorers();
                else if (cgc_strcmp(buf, "3") == 0)
                    cgc_HandleRemoveExplorer();
                else if (cgc_strcmp(buf, "4") == 0)
                    cgc_HandleLearnSkills();
                else if (cgc_strcmp(buf, "5") == 0)
                    break;
                else
                {
                    if (!g_hidden && cgc_strcmp(buf, "6") == 0)
                        cgc_HandleChangeName();
                    else
                        cgc_fprintf(stdout, "Invalid option.\n");
                }
            }
        }
        else if (cgc_strcmp(buf, "3") == 0)
        {
            cgc_fprintf(stdout, "Boo.\n");
            cgc_exit(0);
        }
        else
        {
            if (buf[0] == '-' && cgc_memcmp(&buf[1], "1337\0", 5) == 0)
            {
                if (g_fort.cgc_GetSupply() < cgc_strtoul(&buf[1], NULL, 10))
                    g_fort.cgc_AddSupply(1287);
                if (cgc_freaduntil(buf, sizeof(buf), '\n', stdin) < 0)
                    break;
                g_fort.cgc_NextDay(cgc_strtoul(buf, NULL, 10));
            }
            else
                cgc_fprintf(stdout, "Invalid option.\n");
        }
    }
    return 0;
}
