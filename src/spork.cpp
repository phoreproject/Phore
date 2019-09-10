// Copyright (c) 2014-2016 The Dash developers
// Copyright (c) 2016-2017 The PIVX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "consensus/validation.h"
#include "main.h"
#include "masternode-budget.h"
#include "net.h"
#include "spork.h"
#include "sporkdb.h"

class CSporkMessage;
class CSporkManager;

CSporkManager sporkManager;

std::map<uint256, CSporkMessage> mapSporks;

void CSporkManager::Clear()
{
    strMasterPrivKey = "";
    mapSporksActive.clear();
}

// Phore: on startup load spork values from previous session if they exist in the sporkDB
void CSporkManager::LoadSporksFromDB()
{
    for (int i = SPORK_START; i <= SPORK_END; ++i) {
        // Since not all spork IDs are in use, we have to exclude undefined IDs
        std::string strSpork = sporkManager.GetSporkNameByID(i);
        if (strSpork == "Unknown") {
            LogPrintf("%s : unknown spork ID %d\n", __func__, i);
            continue;
        }
        // attempt to read spork from sporkDB
        CSporkMessage spork;
        if (!pSporkDB->ReadSpork(i, spork)) {
            LogPrintf("%s : no previous value for %s found in database\n", __func__, strSpork);
            continue;
        }

        // add spork to memory
        mapSporks[spork.GetHash()] = spork;
        mapSporksActive[spork.nSporkID] = spork;
        std::time_t result = spork.nValue;
        // If SPORK Value is greater than 1,000,000 assume it's actually a Date and then convert to a more readable format
        if (spork.nValue > 1000000) {
            LogPrintf("%s : loaded spork %s with value %d : %s", __func__,
                      sporkManager.GetSporkNameByID(spork.nSporkID), spork.nValue,
                      std::ctime(&result));
        } else {
            LogPrintf("%s : loaded spork %s with value %d\n", __func__,
                      sporkManager.GetSporkNameByID(spork.nSporkID), spork.nValue);
        }
    }
}

void CSporkManager::ProcessSpork(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)
{
    if (fLiteMode || chainActive.Tip() == nullptr) return; // disable all obfuscation/masternode related functionality

    if (strCommand == "spork") {

        CSporkMessage spork;
        vRecv >> spork;

        // Ignore spork messages about unknown/deleted sporks
        std::string strSpork = sporkManager.GetSporkNameByID(spork.nSporkID);
        if (strSpork == "Unknown") return;

        uint256 hash = spork.GetHash();
        {
            LOCK(cs);
            if (mapSporksActive.count(spork.nSporkID)) {
                // spork is active
                if (mapSporksActive[spork.nSporkID].nTimeSigned >= spork.nTimeSigned) {
                    // spork in memory has been signed more recently
                    if (fDebug) LogPrintf("%s : seen %s block %d \n", __func__, hash.ToString(), chainActive.Tip()->nHeight);
                    return;
                } else {
                    // update active spork
                    if (fDebug) LogPrintf("%s : got updated spork %s block %d \n", __func__, hash.ToString(), chainActive.Tip()->nHeight);
                }
            } else {
                // spork is not active
                if (fDebug) LogPrintf("%s : got new spork %s block %d \n", __func__, hash.ToString(), chainActive.Tip()->nHeight);
            }
        }

        LogPrintf("spork - new %s ID %d Time %d bestHeight %d\n", hash.ToString(), spork.nSporkID, spork.nValue, chainActive.Tip()->nHeight);

        bool fRequireNew = spork.nTimeSigned >= Params().NewSporkStart();
        if (!spork.CheckSignature(fRequireNew)) {
            LOCK(cs_main);
            LogPrintf("%s : Invalid Signature\n", __func__);
            Misbehaving(pfrom->GetId(), 100);
            return;
        }

        {
            LOCK(cs);
            mapSporks[hash] = spork;
            mapSporksActive[spork.nSporkID] = spork;
        }
        spork.Relay();

        // Phore: add to spork database.
        pSporkDB->WriteSpork(spork.nSporkID, spork);
    }
    if (strCommand == "getsporks") {
        LOCK(cs);
        std::map<int, CSporkMessage>::iterator it = mapSporksActive.begin();

        while (it != mapSporksActive.end()) {
            pfrom->PushMessage(NetMsgType::SPORK, it->second);
            it++;
        }
    }
}

bool CSporkManager::UpdateSpork(int nSporkID, int64_t nValue)
{

    CSporkMessage spork = CSporkMessage(nSporkID, nValue, GetTime());

    if(spork.Sign(strMasterPrivKey)){
        spork.Relay();
        LOCK(cs);
        mapSporks[spork.GetHash()] = spork;
        mapSporksActive[nSporkID] = spork;
        return true;
    }

    return false;
}

// grab the spork value, and see if it's off
bool CSporkManager::IsSporkActive(int nSporkID)
{
    int64_t r = GetSporkValue(nSporkID);
    if (r == -1) return false;
    return r < GetAdjustedTime();
}

// grab the value of the spork on the network, or the default
int64_t CSporkManager::GetSporkValue(int nSporkID)
{
    LOCK(cs);
    int64_t r = -1;

    if (mapSporksActive.count(nSporkID)) {
        r = mapSporksActive[nSporkID].nValue;
    } else {
        if (nSporkID == SPORK_2_SWIFTTX) r = SPORK_2_SWIFTTX_DEFAULT;
        if (nSporkID == SPORK_3_SWIFTTX_BLOCK_FILTERING) r = SPORK_3_SWIFTTX_BLOCK_FILTERING_DEFAULT;
        if (nSporkID == SPORK_5_MAX_VALUE) r = SPORK_5_MAX_VALUE_DEFAULT;
        if (nSporkID == SPORK_7_MASTERNODE_SCANNING) r = SPORK_7_MASTERNODE_SCANNING_DEFAULT;
        if (nSporkID == SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT) r = SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT_DEFAULT;
        if (nSporkID == SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT) r = SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT_DEFAULT;
        if (nSporkID == SPORK_10_MASTERNODE_PAY_UPDATED_NODES) r = SPORK_10_MASTERNODE_PAY_UPDATED_NODES_DEFAULT;
        if (nSporkID == SPORK_13_ENABLE_SUPERBLOCKS) r = SPORK_13_ENABLE_SUPERBLOCKS_DEFAULT;
        if (nSporkID == SPORK_14_NEW_PROTOCOL_ENFORCEMENT) r = SPORK_14_NEW_PROTOCOL_ENFORCEMENT_DEFAULT;
        //if (nSporkID == SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2) r = SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2_DEFAULT;
        if (nSporkID == SPORK_16_ZEROCOIN_MAINTENANCE_MODE) r = SPORK_16_ZEROCOIN_MAINTENANCE_MODE_DEFAULT;
        if (nSporkID == SPORK_17_SEGWIT_ACTIVATION) r = SPORK_17_SEGWIT_ACTIVATION_DEFAULT;
        //if (nSporkID == SPORK_18_NEW_PROTOCOL_ENFORCEMENT_3) r = SPORK_18_NEW_PROTOCOL_ENFORCEMENT_3_DEFAULT;
        if (nSporkID == SPORK_19_SEGWIT_ON_COINBASE) r = SPORK_19_SEGWIT_ON_COINBASE_DEFAULT;

        if (r == -1) LogPrintf("GetSpork::Unknown Spork %d\n", nSporkID);
    }

    return r;
}

int CSporkManager::GetSporkIDByName(std::string strName)
{

    if (strName == "SPORK_2_SWIFTTX") return SPORK_2_SWIFTTX;
    if (strName == "SPORK_3_SWIFTTX_BLOCK_FILTERING") return SPORK_3_SWIFTTX_BLOCK_FILTERING;
    if (strName == "SPORK_5_MAX_VALUE") return SPORK_5_MAX_VALUE;
    if (strName == "SPORK_7_MASTERNODE_SCANNING") return SPORK_7_MASTERNODE_SCANNING;
    if (strName == "SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT") return SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT;
    if (strName == "SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT") return SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT;
    if (strName == "SPORK_10_MASTERNODE_PAY_UPDATED_NODES") return SPORK_10_MASTERNODE_PAY_UPDATED_NODES;
    if (strName == "SPORK_13_ENABLE_SUPERBLOCKS") return SPORK_13_ENABLE_SUPERBLOCKS;
    if (strName == "SPORK_14_NEW_PROTOCOL_ENFORCEMENT") return SPORK_14_NEW_PROTOCOL_ENFORCEMENT;
    //if (strName == "SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2") return SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2;
    if (strName == "SPORK_16_ZEROCOIN_MAINTENANCE_MODE") return SPORK_16_ZEROCOIN_MAINTENANCE_MODE;
    if (strName == "SPORK_17_SEGWIT_ACTIVATION") return SPORK_17_SEGWIT_ACTIVATION;
    //if (strName == "SPORK_18_NEW_PROTOCOL_ENFORCEMENT_3") return SPORK_18_NEW_PROTOCOL_ENFORCEMENT_3;
    if (strName == "SPORK_19_SEGWIT_ON_COINBASE") return SPORK_19_SEGWIT_ON_COINBASE_DEFAULT;

    return -1;
}

std::string CSporkManager::GetSporkNameByID(int id)
{
    if (id == SPORK_2_SWIFTTX) return "SPORK_2_SWIFTTX";
    if (id == SPORK_3_SWIFTTX_BLOCK_FILTERING) return "SPORK_3_SWIFTTX_BLOCK_FILTERING";
    if (id == SPORK_5_MAX_VALUE) return "SPORK_5_MAX_VALUE";
    if (id == SPORK_7_MASTERNODE_SCANNING) return "SPORK_7_MASTERNODE_SCANNING";
    if (id == SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT) return "SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT";
    if (id == SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT) return "SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT";
    if (id == SPORK_10_MASTERNODE_PAY_UPDATED_NODES) return "SPORK_10_MASTERNODE_PAY_UPDATED_NODES";
    if (id == SPORK_13_ENABLE_SUPERBLOCKS) return "SPORK_13_ENABLE_SUPERBLOCKS";
    if (id == SPORK_14_NEW_PROTOCOL_ENFORCEMENT) return "SPORK_14_NEW_PROTOCOL_ENFORCEMENT";
    if (id == SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2) return "SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2";
    if (id == SPORK_16_ZEROCOIN_MAINTENANCE_MODE) return "SPORK_16_ZEROCOIN_MAINTENANCE_MODE";

    return "Unknown";
}

bool CSporkManager::SetPrivKey(std::string strPrivKey)
{
    CSporkMessage spork;

    spork.Sign(strPrivKey);

    const bool fRequireNew = GetTime() >= Params().NewSporkStart();
    if (spork.CheckSignature(fRequireNew)) {
        LOCK(cs);
        // Test signing successful, proceed
        LogPrintf("%s : -- Successfully initialized as spork signer\n", __func__);
        strMasterPrivKey = strPrivKey;
        return true;
    }

    return false;
}

std::string CSporkManager::ToString() const
{
    LOCK(cs);
    return strprintf("Sporks: %llu", mapSporksActive.size());
}

bool CSporkMessage::Sign(std::string strSignKey)
{
    std::string strMessage = std::to_string(nSporkID) + std::to_string(nValue) + std::to_string(nTimeSigned);

    CKey key;
    CPubKey pubkey;
    std::string errorMessage = "";

    if (!obfuScationSigner.SetKey(strSignKey, errorMessage, key, pubkey)) {
        return error("%s : SetKey error: '%s'\n", __func__, errorMessage);
    }

    if (!obfuScationSigner.SignMessage(strMessage, errorMessage, vchSig, key)) {
        return error("%s : Sign message failed", __func__);
    }

    if (!obfuScationSigner.VerifyMessage(pubkey, vchSig, strMessage, errorMessage)) {
        return error("%s : Verify message failed", __func__);
    }

    return true;
}

bool CSporkMessage::CheckSignature(bool fRequireNew)
{
    //note: need to investigate why this is failing
    std::string strMessage = std::to_string(nSporkID) + std::to_string(nValue) + std::to_string(nTimeSigned);
    CPubKey pubkeynew(ParseHex(Params().SporkPubKey()));
    std::string errorMessage = "";

    bool fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, vchSig, strMessage, errorMessage);

    if (fRequireNew && !fValidWithNewKey)
        return false;

    // See if window is open that allows for old spork key to sign messages
    if (!fValidWithNewKey && GetAdjustedTime() < Params().RejectOldSporkKey()) {
        CPubKey pubkeyold(ParseHex(Params().SporkPubKeyOld()));
        return obfuScationSigner.VerifyMessage(pubkeyold, vchSig, strMessage, errorMessage);
    }
    return fValidWithNewKey;
}

void CSporkMessage::Relay()
{
    CInv inv(MSG_SPORK, GetHash());
    RelayInv(inv);
}

