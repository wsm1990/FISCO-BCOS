/*
 * @CopyRight:
 * FISCO-BCOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FISCO-BCOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FISCO-BCOS.  If not, see <http://www.gnu.org/licenses/>
 * (c) 2016-2018 fisco-dev contributors.
 */
/**
 * @brief basic data structure for block
 *
 * @file Block.h
 * @author: yujiechem
 * @date 2018-09-20
 */
#pragma once
#include "BlockHeader.h"
#include "Transaction.h"
#include "TransactionReceipt.h"
#include <libdevcore/Common.h>
#include <libdevcore/TrieHash.h>
namespace dev
{
namespace eth
{
class Block
{
public:
    ///-----constructors of Block
    Block() = default;
    explicit Block(bytesConstRef _data);
    explicit Block(bytes const& _data);
    /// copy constructor
    Block(Block const& _block);
    /// assignment operator
    Block& operator=(Block const& _block);
    ~Block() {}
    ///-----opearator overloads of Block
    /// operator ==
    bool equalAll(Block const& _block) const
    {
        return m_blockHeader == _block.blockHeader() && m_sigList == _block.sigList() &&
               m_transactions == _block.transactions();
    }

    bool equalWithoutSig(Block const& _block) const
    {
        return m_blockHeader == _block.blockHeader() && m_transactions == _block.transactions();
    }

    bool equalHeader(Block const& _block) const { return m_blockHeader == _block.blockHeader(); }

    explicit operator bool() const { return bool(m_blockHeader); }

    ///-----encode functions
    void encode(bytes& _out, bytesConstRef _header,
        std::vector<std::pair<u256, Signature>> const& sig_list) const;
    void encode(bytes& _out, bytesConstRef _header) const { encode(_out, _header, m_sigList); }
    void encode(bytes& _out, std::vector<std::pair<u256, Signature>> const& sig_list) const;
    void encode(bytes& _out) const { encode(_out, m_sigList); }

    ///-----decode functions
    void decode(bytesConstRef _block);

    ///-----get interfaces
    Transactions const& transactions() const { return m_transactions; }
    TransactionReceipts const& transactionReceipts() const { return m_transactionReceipts; }
    Transaction const& transaction(size_t const _index) const { return m_transactions[_index]; }
    BlockHeader const& blockHeader() const { return m_blockHeader; }
    BlockHeader& header() { return m_blockHeader; }
    h256 headerHash() const { return m_blockHeader.hash(); }
    std::vector<std::pair<u256, Signature>> const& sigList() const { return m_sigList; }

    ///-----set interfaces
    /// set m_transactions
    void setTransactions(Transactions const& _trans)
    {
        m_transactions = _trans;
        noteChange();
    }
    /// set m_transactionReceipts
    void setTransactionReceipts(TransactionReceipts const& transReceipt)
    {
        m_transactionReceipts = transReceipt;
    }
    /// append a single transaction to m_transactions
    void appendTransaction(Transaction const& _trans)
    {
        m_transactions.push_back(_trans);
        noteChange();
    }
    /// append transactions
    void appendTransactions(Transactions const& _trans_array)
    {
        for (auto trans : _trans_array)
            m_transactions.push_back(trans);
        noteChange();
    }
    /// set block header
    void setBlockHeader(BlockHeader const& _blockHeader) { m_blockHeader = _blockHeader; }
    /// set sig list
    void inline setSigList(std::vector<std::pair<u256, Signature>> const& _sigList)
    {
        m_sigList = _sigList;
    }
    /// get hash of block header
    h256 blockHeaderHash() { return m_blockHeader.hash(); }
    bool isSealed() const { return (m_blockHeader.sealer() != Invalid256); }
    size_t getTransactionSize() const { return m_transactions.size(); }

    /// get transactionRoot
    h256 const getTransactionRoot()
    {
        encodeTransactions();
        return m_txsRoot;
    }

    void resetCurrentBlock(BlockHeader& _parent)
    {
        m_blockHeader.populateFromParent(_parent);
        m_transactions.clear();
        m_transactionReceipts.clear();
        m_sigList.clear();
        m_txsCache.clear();
        m_txsMapCache.clear();
        m_txsRoot.clear();
    }

    void appendTransactionReceipt(TransactionReceipt const& _tran) { m_receipts.push_back(_tran); }

    const TransactionReceipts& getTransactionReceipts() const { return m_receipts; }

private:
    /// encode function
    inline void encode(bytes& _out, bytesConstRef block_header, h256 const& hash,
        std::vector<std::pair<u256, Signature>> const& sig_list) const;
    /// callback this function when transaction has changed
    void noteChange()
    {
        /// RecursiveGuard l(m_txsCacheLock);
        m_txsCache = bytes();
        m_txsMapCache = BytesMap();
    }

    bytes const& encodeTransactions() const;

private:
    /// block header of the block (field 0)
    BlockHeader m_blockHeader;
    /// transaction list (field 1)
    Transactions m_transactions;
    TransactionReceipts m_transactionReceipts;
    /// sig list (field 3)
    std::vector<std::pair<u256, Signature>> m_sigList;
    /// m_transactions converted bytes, when m_transactions changed,
    /// should refresh this catch when encode
    mutable bytes m_txsCache;
    /// mutable RecursiveMutex m_txsCacheLock;
    TransactionReceipts m_receipts;  ///< The corresponding list of transaction receipts.
    mutable BytesMap m_txsMapCache;
    mutable h256 m_txsRoot;
};
}  // namespace eth
}  // namespace dev
