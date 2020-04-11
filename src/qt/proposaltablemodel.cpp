// Copyright (c) 2018 The Phore developers
// Copyright (c) 2018 The Curium developers
// Copyright (c) 2017-2018 The Bulwark Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "proposaltablemodel.h"

#include "guiconstants.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "proposalrecord.h"
#include "masternode-budget.h"
#include "masternode-payments.h"
#include "masternodeconfig.h"
#include "masternodeman.h"
#include "rpc/server.h"

#include "obfuscation.h"
//#include "governance-vote.h"
//#include "governance-object.h"

#include "core_io.h"
//#include "validation.h"
#include "sync.h"
#include "uint256.h"
#include "util.h"
 
#include <cmath>
#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QIcon>
#include <QList>
#include <univalue.h>

static int column_alignments[] = {
    Qt::AlignLeft|Qt::AlignVCenter,
    Qt::AlignRight|Qt::AlignVCenter,
    Qt::AlignRight|Qt::AlignVCenter,
    Qt::AlignRight|Qt::AlignVCenter,
    Qt::AlignRight|Qt::AlignVCenter,
    Qt::AlignRight|Qt::AlignVCenter,
    Qt::AlignRight|Qt::AlignVCenter,
    Qt::AlignRight|Qt::AlignVCenter,
    Qt::AlignRight|Qt::AlignVCenter,
    Qt::AlignRight|Qt::AlignVCenter
};

ProposalTableModel::ProposalTableModel(QObject *parent) : QAbstractTableModel(parent)
{
    columns << tr("Proposal") << tr("Amount") << tr("Start Block") << tr("End Block") << tr("Payments") << tr("Remaining") << tr("Yes") << tr("No") << tr("Abstain") << tr("Votes Needed");

    networkManager = new QNetworkAccessManager(this);

    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onResult(QNetworkReply*)));

    refreshProposals();
}

ProposalTableModel::~ProposalTableModel()
{
}



void budgetToST(CBudgetProposal* pbudgetProposal, UniValue& bObj)
{
    CTxDestination address;
    ExtractDestination(pbudgetProposal->GetPayee(), address);

    bObj.push_back(pbudgetProposal->GetName());
    bObj.push_back(pbudgetProposal->GetURL());
    bObj.push_back(pbudgetProposal->GetHash().ToString());
    bObj.push_back(pbudgetProposal->nFeeTXHash.ToString());
    bObj.push_back(pbudgetProposal->GetBlockStart());
    bObj.push_back(pbudgetProposal->GetBlockEnd());
    bObj.push_back(pbudgetProposal->GetTotalPaymentCount());
    bObj.push_back(pbudgetProposal->GetRemainingPaymentCount());
    bObj.push_back(EncodeDestination(address));
    bObj.push_back(pbudgetProposal->GetYeas());
    bObj.push_back(pbudgetProposal->GetNays());
    bObj.push_back(pbudgetProposal->GetAbstains());
    bObj.push_back(ValueFromAmount(pbudgetProposal->GetAmount() * pbudgetProposal->GetTotalPaymentCount()));
    bObj.push_back(pbudgetProposal->GetAmount());
    bObj.push_back(pbudgetProposal->IsEstablished());

    std::string strError = "";
    bObj.push_back(Pair("IsValid", pbudgetProposal->IsValid(strError)));
    bObj.push_back(Pair("IsValidReason", strError.c_str()));
    bObj.push_back(Pair("fValid", pbudgetProposal->fValid));
}

void ProposalTableModel::setProposalType(const int &type)
{
    proposalType = type;
    refreshProposals();
}

void ProposalTableModel::refreshProposals()
{
    beginResetModel();
    proposalRecords.clear();

    int mnCount = mnodeman.CountEnabled();
    std::vector<CBudgetProposal*> bObj;
    
    if (proposalType == 0)
    {
        bObj = budget.GetAllProposals();
    }
    else
    {
        bObj = budget.GetBudget();
    }


    for (CBudgetProposal* pbudgetProposal : bObj)
    {
        UniValue o(UniValue::VOBJ);
        budgetToST(pbudgetProposal, o);

        //UniValue objResult(UniValue::VOBJ);
        //UniValue dataObj(UniValue::VOBJ);
        //objResult.read(pbudgetProposal->GetDataAsPlainString()); // not need as time being

        //std::vector<UniValue> arr1 = objResult.getValues();
        //std::vector<UniValue> arr2 = arr1.at( 0 ).getValues();
        //dataObj = arr2.at( 1 );

        UniValue bObj(UniValue::VOBJ);
        budgetToST(pbudgetProposal, bObj);

        int votesNeeded = 0;
        int voteGap = 0;

        if(mnCount > 0) {
            voteGap = ceil( (mnCount / 10) - (pbudgetProposal->GetYeas() - pbudgetProposal->GetNays()) );
            votesNeeded = (voteGap < 0) ? 0 : voteGap;
        };

        proposalRecords.append(new ProposalRecord(
                        QString::fromStdString(pbudgetProposal->GetHash().ToString()),
                        pbudgetProposal->GetBlockStart(),
                        pbudgetProposal->GetBlockEnd(),
                        pbudgetProposal->GetTotalPaymentCount(),
                        pbudgetProposal->GetRemainingPaymentCount(),
                        QString::fromStdString(pbudgetProposal->GetURL()),
                        QString::fromStdString(pbudgetProposal->GetName()),
                        (long long)pbudgetProposal->GetYeas(),
                        (long long)pbudgetProposal->GetNays(),
                        (long long)pbudgetProposal->GetAbstains(),
                        (long long)pbudgetProposal->GetAmount(),
                        (long long)votesNeeded));
    }
    endResetModel();
}

void ProposalTableModel::onResult(QNetworkReply *result) {
    /**/
}

int ProposalTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return proposalRecords.size();
}

int ProposalTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant ProposalTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    ProposalRecord *rec = static_cast<ProposalRecord*>(index.internalPointer());

    switch(role)
    {
        case Qt::DisplayRole:
            switch (index.column())
            {
                case Proposal:
                    return rec->name;
                case YesVotes:
                    return rec->yesVotes;
                case NoVotes:
                    return rec->noVotes;
                case AbstainVotes:
                    return rec->abstainVotes;
                case StartDate:
                    return rec->start_epoch;
                case EndDate:
                    return rec->end_epoch;
                case TotalPaymentCount:
                    return rec->totalPaymentCount;
                case RemainingPaymentCount:
                    return rec->remainingPaymentCount;
                case VotesNeeded:
                    return QString("%1").arg(rec->votesNeeded);
                case Amount:
                    return BitcoinUnits::format(BitcoinUnits::PHR, rec->amount);
            }
            break;
        case Qt::EditRole:
            switch (index.column())
            {
                case Proposal:
                    return rec->name;
                case StartDate:
                    return rec->start_epoch;
                case EndDate:
                    return rec->end_epoch;
                case TotalPaymentCount:
                    return rec->totalPaymentCount;
                case RemainingPaymentCount:
                    return rec->remainingPaymentCount;
                case YesVotes:
                    return rec->yesVotes;
                case NoVotes:
                    return rec->noVotes;
                case AbstainVotes:
                    return rec->abstainVotes;
                case Amount:
                    return qint64(rec->amount);
                case VotesNeeded:
                    return rec->votesNeeded;
            }
            break;
        case Qt::TextAlignmentRole:
            return column_alignments[index.column()];
        case Qt::ForegroundRole:
            if(index.column() == VotesNeeded) {
                if(rec->votesNeeded > 0) {
                    return COLOR_NEGATIVE;
                } else {
                    return QColor(23, 168, 26);
                }
            }
            return COLOR_BAREADDRESS;
            break;
        case ProposalRole:
            return rec->name;
        case AmountRole:
            return qint64(rec->amount);
        case StartDateRole:
            return rec->start_epoch;
        case EndDateRole:
            return rec->end_epoch;
        case TotalPaymentCountRole:
            return rec->totalPaymentCount;
        case RemainingPaymentCountRole:
            return rec->remainingPaymentCount;
        case YesVotesRole:
            return rec->yesVotes;
        case NoVotesRole:
            return rec->noVotes;
        case AbstainVotesRole:
            return rec->abstainVotes;
        case VotesNeededRole:
            return rec->votesNeeded;
        case ProposalUrlRole:
            return rec->url;
        case ProposalHashRole:
            return rec->hash;
    }
    return QVariant();
}

QVariant ProposalTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
        {
            return columns[section];
        }
        else if (role == Qt::TextAlignmentRole)
        {
            return Qt::AlignVCenter;
        } 
        else if (role == Qt::ToolTipRole)
        {
            switch(section)
            {
                case Proposal:
                    return tr("Proposal Name");
                case StartDate:
                    return tr("Block that the proposal starts");
                case EndDate:
                    return tr("Block that the proposal ends");
                case TotalPaymentCount:
                    return tr("Total payment count");
                case RemainingPaymentCount:
                    return tr("Remaining payment count");
                case YesVotes:
                    return tr("Current yes votes");
                case NoVotes:
                    return tr("Current no votes");
                case AbstainVotes:
                    return tr("Current abstain votes");
                case Amount:
                    return tr("Proposed amount");
                case VotesNeeded:
                    return tr("Additional yes votes needed to pass");
            }
        }
    }
    return QVariant();
}

QModelIndex ProposalTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if(row >= 0 && row < proposalRecords.size()) {
        ProposalRecord *rec = proposalRecords[row];
        return createIndex(row, column, rec);
    }

    return QModelIndex();
}
