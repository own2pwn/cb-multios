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

#include "fortress.h"

cgc_CFortress::cgc_CFortress() : m_supply(50), m_day(1), m_numSuccessfulMissions(0), m_numTotalMissions(0),
                         m_totalSupplySpent(0), m_totalSupplyEarned(0)
{
    cgc_memset(m_owner, 0, sizeof(m_owner));
}

cgc_CFortress::~cgc_CFortress()
{
}

int cgc_CFortress::cgc_GetSupply()
{
    return m_supply;
}

bool cgc_CFortress::cgc_AddSupply(int n)
{
    if (n < 0 && m_supply + n > k_maxSupply)
        return false;
    m_supply += n;
    return true;
}

bool cgc_CFortress::cgc_SubSupply(int n)
{
    if (n < 0 || m_supply - n < 0)
        return false;
    m_supply -= n;
    return true;
}

int cgc_CFortress::cgc_GetDay()
{
    return m_day;
}

void cgc_CFortress::cgc_NextDay(int n)
{
    m_day += n;
    int i;
    cgc_CList<cgc_CMission *> toRemove;
    for (i = 0; i < m_missions.cgc_GetSize(); i++)
    {
        cgc_CMission *m = m_missions.cgc_GetAt(i);
        if (!m->cgc_IsAvail())
        {
            m->cgc_SubDuration(n);
            if (m->cgc_GetDuration() <= 0)
            {
                if (m->cgc_Complete())
                {
                    cgc_fprintf(stdout, "\n### MISSION SUCCESS :) ###\n");
                    cgc_fprintf(stdout, "+ + %s + +\n", m->cgc_GetName());
                    cgc_AddSupply(m->cgc_GetRewardSupply());
                    cgc_fprintf(stdout, " => +%d Supply\n", m->cgc_GetRewardSupply());
                    int j;
                    cgc_CList<cgc_CExplorer *>* exps = m->cgc_GetExplorers();
                    for (j = 0; j < exps->cgc_GetSize(); j++)
                    {
                        cgc_CExplorer *e = exps->cgc_GetAt(j);
                        bool lvlUp = e->cgc_GainExp(m->cgc_GetRewardExp());
                        cgc_fprintf(stdout, " => <%s> +%d Experience %s\n", e->cgc_GetName(), m->cgc_GetRewardExp(), lvlUp ? "[LEVEL UP!]" : "");
                    }
                }
                else
                {
                    cgc_fprintf(stdout, "\n### MISSION FAILED :( ###\n");
                    cgc_fprintf(stdout, "- - %s - -\n", m->cgc_GetName());
                }
                toRemove.cgc_Append(m);
            }
        }
    }
    for (i = 0; i < toRemove.cgc_GetSize(); i++)
    {
        cgc_CMission *m = toRemove.cgc_GetAt(i);
        m_missions.cgc_Remove(m);
        delete m;
    }
    if (m_supply < 1000)
        m_supply += 100;
}

cgc_CList<cgc_CMission *>& cgc_CFortress::cgc_GetMissions()
{
    return m_missions;
}

cgc_CList<cgc_CExplorer *>& cgc_CFortress::cgc_GetExplorers()
{
    return m_explorers;
}

cgc_CList<cgc_CSkill *>& cgc_CFortress::cgc_GetSkills()
{
    return m_skills;
}

int cgc_CFortress::cgc_GetNumAvailableMissions()
{
    int i, n = 0;
    for (i = 0; i < m_missions.cgc_GetSize(); i++)
        n += (m_missions.cgc_GetAt(i)->cgc_IsAvail() ? 1 : 0);
    return n;
}

int cgc_CFortress::cgc_GetNumAvailableExplorers()
{
    int i, n = 0;
    for (i = 0; i < m_explorers.cgc_GetSize(); i++)
        n += (m_explorers.cgc_GetAt(i)->cgc_IsAvail() && m_explorers.cgc_GetAt(i)->cgc_IsHired() ? 1 : 0);
    return n;
}

int cgc_CFortress::cgc_GetNumHiredExplorers()
{
    int i, n = 0;
    for (i = 0; i < m_explorers.cgc_GetSize(); i++)
        n += (m_explorers.cgc_GetAt(i)->cgc_IsHired() ? 1 : 0);
    return n;
}

int cgc_CFortress::cgc_GetAvgLevel()
{
    int i, n = 0, levelSum = 0;
    for (i = 0; i < m_explorers.cgc_GetSize(); i++)
    {
        if (m_explorers.cgc_GetAt(i)->cgc_IsHired())
        {
            levelSum += m_explorers.cgc_GetAt(i)->cgc_GetLevel();
            n++;
        }
    }
    if (n == 0 || levelSum / n == 0)
        return 1;
    return levelSum / n;
}

void cgc_CFortress::cgc_SetOwner(char *owner)
{
    owner[k_maxOwnerLen] = '\0';
    cgc_strcpy(m_owner, owner);
}
