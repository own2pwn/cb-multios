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

#pragma once

#include <cstdint.h>

#include "req.h"
#include "reward.h"
#include "explorer.h"
#include "list.h"

class cgc_CMissionEstimate
{
    friend class cgc_CMission;

    public:
        cgc_CMissionEstimate() {};
        ~cgc_CMissionEstimate() {};

        int cgc_GetSuccessRate() { return m_successRate; }
        int cgc_GetGroupNeeded() { return m_groupNeeded; }
        int cgc_GetGroupSupplied() { return m_groupSupplied; }
        cgc_CRequirement::Type cgc_GetMissingTypes() { return m_missing; }

    private:
        int m_successRate;
        cgc_CRequirement::Type m_missing;
        int m_groupNeeded;
        int m_groupSupplied;
};

class cgc_CMission
{
    public:
        cgc_CMission(const char *name, const cgc_CRequirement &req, const cgc_CReward &reward);
        ~cgc_CMission();

        void cgc_SetRequirement(const cgc_CRequirement &req);
        cgc_CRequirement& cgc_GetRequirement();
        void cgc_SetReward(const cgc_CReward &reward);
        cgc_CReward& cgc_GetReward();
        cgc_CList<cgc_CExplorer *>* cgc_GetExplorers() { return m_exps; }
        bool cgc_IsAvail();
        int cgc_GetSuccessRate() { return m_successRate; }
        void cgc_SetSuccessRate(int sr) { m_successRate = sr; }

        const char* cgc_GetName();
        int cgc_GetReqLevel();
        int cgc_GetReqGroup();
        int cgc_GetReqSupply();
        int cgc_GetDuration();
        void cgc_SubDuration(int n) { m_req.m_duration -= n; }
        char* cgc_GetReqTypeString();

        int cgc_GetRewardExp() { return m_reward.m_exp; }
        int cgc_GetRewardSupply() { return m_reward.m_supply; }

        void cgc_Execute(const cgc_CList<cgc_CExplorer *> &explorers);
        cgc_CMissionEstimate* cgc_Check(const cgc_CList<cgc_CExplorer *> &explorers);
        bool cgc_Complete();

    private:
        cgc_CRequirement::Type cgc_EvalCounters(const cgc_CList<cgc_CExplorer *> &exps);

        static const int k_maxNameLength = 32;

        char *m_name;
        cgc_CRequirement m_req;
        cgc_CReward m_reward;
        bool m_avail;
        int m_successRate;
        cgc_CList<cgc_CExplorer *> *m_exps;
};
