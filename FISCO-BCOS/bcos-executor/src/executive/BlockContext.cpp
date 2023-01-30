/*
 *  Copyright (C) 2021 FISCO BCOS.
 *  SPDX-License-Identifier: Apache-2.0
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * @brief block level context
 * @file BlockContext.h
 * @author: xingqiangbai
 * @date: 2021-05-27
 */

#include "BlockContext.h"
#include "../vm/Precompiled.h"
#include "ExecutiveStackFlow.h"
#include "TransactionExecutive.h"
#include "bcos-codec/abi/ContractABICodec.h"
#include "bcos-executor/src/precompiled/common/Common.h"
#include "bcos-executor/src/precompiled/common/Utilities.h"
#include "bcos-framework/protocol/Exceptions.h"
#include "bcos-framework/storage/StorageInterface.h"
#include "bcos-framework/storage/Table.h"
#include <bcos-utilities/Error.h>
#include <boost/core/ignore_unused.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>
#include <string>

using namespace bcos::executor;
using namespace bcos::protocol;
using namespace bcos::precompiled;
using namespace std;

BlockContext::BlockContext(std::shared_ptr<storage::StateStorageInterface> storage,
    LedgerCache::Ptr ledgerCache, crypto::Hash::Ptr _hashImpl,
    bcos::protocol::BlockNumber blockNumber, h256 blockHash, uint64_t timestamp,
    uint32_t blockVersion, const VMSchedule& _schedule, bool _isWasm, bool _isAuthCheck)
  : m_blockNumber(blockNumber),
    m_blockHash(blockHash),
    m_timeStamp(timestamp),
    m_blockVersion(blockVersion),
    m_schedule(_schedule),
    m_isWasm(_isWasm),
    m_isAuthCheck(_isAuthCheck),
    m_storage(std::move(storage)),
    m_hashImpl(_hashImpl),
    m_ledgerCache(ledgerCache)
{}

BlockContext::BlockContext(std::shared_ptr<storage::StateStorageInterface> storage,
    LedgerCache::Ptr ledgerCache, crypto::Hash::Ptr _hashImpl,
    protocol::BlockHeader::ConstPtr _current, const VMSchedule& _schedule, bool _isWasm,
    bool _isAuthCheck, std::shared_ptr<std::set<std::string, std::less<>>> _keyPageIgnoreTables)
  : BlockContext(storage, ledgerCache, _hashImpl, _current->number(), _current->hash(),
        _current->timestamp(), _current->version(), _schedule, _isWasm, _isAuthCheck)
{
    m_keyPageIgnoreTables = std::move(_keyPageIgnoreTables);
}


ExecutiveFlowInterface::Ptr BlockContext::getExecutiveFlow(std::string codeAddress)
{
    bcos::ReadGuard l(x_executiveFlows);
    auto it = m_executiveFlows.find(codeAddress);
    if (it == m_executiveFlows.end())
    {
        /*
        bool success;
        std::tie(it, success) =
            m_executiveFlows.emplace(codeAddress, std::make_shared<ExecutiveStackFlow>());

            */
        return nullptr;
    }
    return it->second;
}

void BlockContext::setExecutiveFlow(
    std::string codeAddress, ExecutiveFlowInterface::Ptr executiveFlow)
{
    bcos::ReadGuard l(x_executiveFlows);
    m_executiveFlows.emplace(codeAddress, executiveFlow);
}

void BlockContext::suicide(std::string_view contract2Suicide)
{
    {
        bcos::WriteGuard l(x_suicides);
        m_suicides.emplace(contract2Suicide);
    }

    EXECUTOR_LOG(TRACE) << LOG_BADGE("SUICIDE")
                        << "Add suicide: " << LOG_KV("table2Suicide", contract2Suicide)
                        << LOG_KV("suicides.size", m_suicides.size())
                        << LOG_KV("blockNumber", m_blockNumber);
}

void BlockContext::killSuicides()
{
    bcos::ReadGuard l(x_suicides);
    if (m_suicides.empty())
    {
        return;
    }

    auto emptyCodeHash = m_hashImpl->hash("");
    for (std::string_view table2Suicide : m_suicides)
    {
        auto contractTable = storage()->openTable(table2Suicide);

        if (contractTable)
        {
            // set codeHash
            bcos::storage::Entry emptyCodeHashEntry;
            emptyCodeHashEntry.importFields({emptyCodeHash.asBytes()});
            contractTable->setRow(ACCOUNT_CODE_HASH, std::move(emptyCodeHashEntry));

            // delete binary
            bcos::storage::Entry emptyCodeEntry;
            emptyCodeEntry.importFields({std::move("")});
            contractTable->setRow(ACCOUNT_CODE, std::move(emptyCodeEntry));
        }

        EXECUTOR_LOG(TRACE) << LOG_BADGE("SUICIDE")
                            << "Kill contract: " << LOG_KV("contract2Suicide", table2Suicide)
                            << LOG_KV("blockNumber", m_blockNumber);
    }
}
