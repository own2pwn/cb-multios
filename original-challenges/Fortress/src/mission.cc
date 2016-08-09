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
#include "mission.h"

extern int g_randIdx;
extern char *g_rand;

cgc_CMission::cgc_CMission(const char *name, const cgc_CRequirement &req, const cgc_CReward &reward) : m_req(req), m_reward(reward), m_avail(true), m_successRate(0)
{
    m_name = cgc_strndup(name, k_maxNameLength);
}

cgc_CMission::~cgc_CMission()
{
    cgc_free(m_name);
    delete m_exps;
}

void cgc_CMission::cgc_SetRequirement(const cgc_CRequirement &req)
{
    m_req = req;
}

cgc_CRequirement& cgc_CMission::cgc_GetRequirement()
{
    return m_req;
}

void cgc_CMission::cgc_SetReward(const cgc_CReward &reward)
{
    m_reward = reward;
}

cgc_CReward& cgc_CMission::cgc_GetReward()
{
    return m_reward;
}

bool cgc_CMission::cgc_IsAvail()
{
    return m_avail;
}

const char* cgc_CMission::cgc_GetName()
{
    return m_name;
}

int cgc_CMission::cgc_GetReqLevel()
{
    return m_req.m_level;
}

int cgc_CMission::cgc_GetReqGroup()
{
    return m_req.m_group;
}

int cgc_CMission::cgc_GetReqSupply()
{
    return m_req.m_supply;
}

int cgc_CMission::cgc_GetDuration()
{
    return m_req.m_duration;
}

char* cgc_CMission::cgc_GetReqTypeString()
{
    return cgc_CRequirement::cgc_ReqTypeToString(m_req.m_type);
}

cgc_CRequirement::Type cgc_CMission::cgc_EvalCounters(const cgc_CList<cgc_CExplorer *> &exps)
{
    int i;
    cgc_CRequirement::Type type, all = cgc_CRequirement::Type::NOTHING;
    for (i = 0; i < exps.cgc_GetSize(); i++)
        all |= exps.cgc_GetAt(i)->cgc_GetCounters();
    type = m_req.m_type & (~all);
    return type;
}

void cgc_CMission::cgc_Execute(const cgc_CList<cgc_CExplorer *> &explorers)
{
    m_avail = false;
    m_exps = new cgc_CList<cgc_CExplorer *>(explorers);
    int i;
    for (i = 0; i < m_exps->cgc_GetSize(); i++)
        m_exps->cgc_GetAt(i)->cgc_SetAvail(false);
}

bool cgc_CMission::cgc_Complete()
{
    cgc_CMissionEstimate* est = this->cgc_Check(*m_exps);
    int i, r = (g_rand[g_randIdx % 4096] & 0xFF) % 101;
    g_randIdx += 2;
    bool ret = false;
    if (est->cgc_GetSuccessRate() != 0 && r <= est->cgc_GetSuccessRate())
        ret = true;
    for (i = 0; i < m_exps->cgc_GetSize(); i++)
        m_exps->cgc_GetAt(i)->cgc_SetAvail(true);
    delete est;
    return ret;
}

cgc_CMissionEstimate* cgc_CMission::cgc_Check(const cgc_CList<cgc_CExplorer *> &explorers)
{
    cgc_CMissionEstimate *est = new cgc_CMissionEstimate();
    est->m_missing = cgc_EvalCounters(explorers);
    est->m_groupNeeded = m_req.m_group;
    est->m_groupSupplied = explorers.cgc_GetSize();
    est->m_successRate = 100;
    if (est->m_missing != cgc_CRequirement::Type::NOTHING)
    {
        int n = __builtin_popcount(static_cast<int>(est->m_missing));
        est->m_successRate -= n * 25;
    }
    int i;
    for (i = 0; i < explorers.cgc_GetSize(); i++)
    {
        int lvl = explorers.cgc_GetAt(i)->cgc_GetLevel();
        if (lvl < m_req.m_level)
            est->m_successRate -= ((m_req.m_level - lvl) * 5);
        est->m_successRate += 5;
    }
    if (est->m_successRate < 0 || est->m_groupSupplied < est->m_groupNeeded)
        est->m_successRate = 0;
    if (est->m_successRate > 100)
        est->m_successRate = 100;
    return est;
}

