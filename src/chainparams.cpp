// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2017-2018 The Phore developers
// Copyright (c) 2018 The ODIN developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "libzerocoin/Params.h"
#include "chainparams.h"
#include "random.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7 * 24 * 60 * 60;
    for (unsigned int i = 0; i < count; i++) {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

//   What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions


/**
 * MAIN NET CHECKPOINTS
 * 
 */
static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of
     (    0, uint256("0x31ca29566549e444cf227a0e2e067aed847c2acc541d3bbf9ca1ae89f4fd57d7"))
     (  200, uint256("0x000000ab78415e3e4cb423061911332f6e1748fd17a5694afd8927f0668d580a"))
     ( 1000, uint256("0xa039641b0d00eb71b3ff0ac7c9cfe4b782908bddaf7e16e10b24760263185fbc"))
     ( 3000, uint256("0x71d1e02eef9f59adb3ec14fdba2aa5806b1740e0a0297f318831e960c68b21e2"))
     ( 4995, uint256("0x1bc38e92d23938cd8448b0aa14480f03a8ea899a8a2200cfc68bb69f264543c8"))
    ( 88061, uint256("0x198ccea2db058e2726593655d8a0761d3c8d237f29b2ee0453f038a473d5f2b4"));

static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1543391873, // * UNIX timestamp of last checkpoint block
    180986,     // * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the SetBestChain debug.log lines)
    2880        // * estimated number of transactions per day after checkpoint
};


/**
 * TEST NET CHECKPOINTS
 * 
 */
static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of
    (    0, uint256("0x2c4b736ccfcc296274c342092b1c6f9d82f0f036a94d57057ee0eed400372106"));

static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
    1537276237, // * UNIX timestamp of last checkpoint block
    0,          // * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the SetBestChain debug.log lines)
    3600        // * estimated number of transactions per day after checkpoint
};


/**
 * REGRESSION TEST NET CHECKPOINTS
 * 
 */
static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of
    (    0, uint256("0x4382c714d146c6fdde6042fbc7b85be7cc8ed474bb343ddd86736f3241b7ea7e"));

static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
    1537276691, // * UNIX timestamp of last checkpoint block
    0,          // * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the SetBestChain debug.log lines)
    3600        // * estimated number of transactions per day after checkpoint
};


libzerocoin::ZerocoinParams* CChainParams::Zerocoin_Params() const
{
    assert(this);
    static CBigNum bnTrustedModulus;
    bnTrustedModulus.SetDec(zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParams = libzerocoin::ZerocoinParams(bnTrustedModulus);

    return &ZCParams;
}

libzerocoin::ZerocoinParams* CChainParams::OldZerocoin_Params() const
{
    assert(this);
    static CBigNum bnTrustedModulus;
    bnTrustedModulus.SetHex(zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParams = libzerocoin::ZerocoinParams(bnTrustedModulus);

    return &ZCParams;
}

class CMainParams : public CChainParams
{
    public:
    CMainParams()
    {
        networkID     = CBaseChainParams::MAIN;
        strNetworkID  = "main";

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0x52;
        pchMessageStart[1] = 0x96;
        pchMessageStart[2] = 0x79;
        pchMessageStart[3] = 0x37;

        vAlertPubKey = ParseHex("046820882af87cbd9d6728b5c386374229544f79d0afe23750aa217269b1b892fc2516d609d6757c1453412bfac500993782b18bde56b4b61d5e3fa2f8cce7a935");

        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;

        // Primary configurations
        nDefaultPort            = 22100; // Main (mainnet) P2P Port
        nMaxReorganizationDepth = 100;
        nMinerThreads           = 0;
        nTargetTimespan         = 60 * 1; // Odin: 1 day
        nTargetSpacing          = 60 * 1; // Odin: 1 minute
        nMaturity               = 10; // Transaction maturity
        nMasternodeCountDrift   = 20;
        nMaxMoneyOut            = 20000000 * COIN; // Max 20kk Ø per transaction
        bnProofOfWorkLimit      = ~uint256(0) >> 1;
        nLastPOWBlock           = 200; // Last Proof-of-Work block
        nModifierUpdateBlock    = 200;
        nMinStakeAge            = 60 * 60 * 1; // 1 hour
        
        // MODIFIER_INTERVAL: time to elapse before new modifier is computed
        // MODIFIER_RATIO: ratio of group interval length between the last group and the first group
        nModifierInterval = 60;
        nModifierIntervalRatio = 3;
        
        // BUDGET_PERCENT: % of block reward that goes to community proposals
        // BUDGET_COLLATERAL: amount of Ø required to submit a community proposal
        // MASTERNODE_REWARD: % of block reward that goes to masternodes
        // MASTERNODE_COLLATERAL: amount of Ø required for a masternode
        nBudgetPercent                = 10; 
        nBudgetSubmissionCollateral   = 50;
        nMasternodeRewardPercent      = 60;
        nRequiredMasternodeCollateral = 25000 * COIN;

        // Zerocoin Configurations
        nZerocoinLastOldParams            = 99999999; // Updated to defer zerocoin v2 for further testing.
        nMaxZerocoinSpendsPerTransaction  = 7;        // Assume about 20kb each
        nMinZerocoinMintFee               = 1 * CENT; // high fee required for zerocoin mints
        nMintRequiredConfirmations        = 20;       // the maximum amount of confirmations until accumulated in 19
        nRequiredAccumulation             = 1;
        nDefaultSecurityLevel             = 100;      // full security level for accumulators
        nZerocoinHeaderVersion            = 4;        // Block headers must be this version once zerocoin is active
        nBudgetFeeConfirmations           = 6;        // Number of confirmations for the finalization fee
        nZerocoinStartHeight              = 143446;   // Aim for Yggdrasil Release

        /**
         * Build the genesis block. Note that the output of the genesis coinbase cannot
         * be spent as it did not originally exist in the database.
         *
         * CBlock(hash=00000ffd590b14, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=e0028e, nTime=1390095618, nBits=1e0ffff0, nNonce=28917698, vtx=1)
         *   CTransaction(hash=e0028e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
         *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d01044c5957697265642030392f4a616e2f3230313420546865204772616e64204578706572696d656e7420476f6573204c6976653a204f76657273746f636b2e636f6d204973204e6f7720416363657074696e6720426974636f696e73)
         *     CTxOut(nValue=50.00000000, scriptPubKey=0xA9037BAC7050C479B121CF)
         *   vMerkleTree: e0028e
         */
        const char* pszTimestamp = "http://odinblockchain.org/2018-09-18-odin-blockchain-genesis/";

        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        // txNew.vout[0].SetEmpty();
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("0402063889d534fb2b51e521c6f730586f86db8577cad991fb0f07c0c54cbc0cac32d8e42ca3cc6b98a76c2216e727ddb298b2489cf392dda006c551acf2f2e991") << OP_CHECKSIG;

        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock   = 0;
        genesis.hashMerkleRoot  = genesis.BuildMerkleTree();
        genesis.nVersion        = 1;
        genesis.nTime           = 1537275388; // Sept 18, 2018 (date +%s)
        genesis.nBits           = 0x207fffff;
        genesis.nNonce          = 0;

        hashGenesisBlock = genesis.GetHash();

        assert(hashGenesisBlock == uint256("0x31ca29566549e444cf227a0e2e067aed847c2acc541d3bbf9ca1ae89f4fd57d7"));
        assert(genesis.hashMerkleRoot == uint256("0x4e80fa44e71d494568525ac04c84c876f11124e59699ba3b59744e44fce232ac"));

        vSeeds.push_back(CDNSSeedData("nyc1.odinblockchain.org", "nyc1.odinblockchain.org")); // MN 01
        vSeeds.push_back(CDNSSeedData("lon1.odinblockchain.org", "lon1.odinblockchain.org")); // MN 02
        vSeeds.push_back(CDNSSeedData("fra1.odinblockchain.org", "fra1.odinblockchain.org")); // MN 03
        vSeeds.push_back(CDNSSeedData("sfo1.odinblockchain.org", "sfo1.odinblockchain.org")); // MN 04
        vSeeds.push_back(CDNSSeedData("blr1.odinblockchain.org", "blr1.odinblockchain.org")); // MN 05
        vSeeds.push_back(CDNSSeedData("ams1.odinblockchain.org", "ams1.odinblockchain.org")); // MN 06
        vSeeds.push_back(CDNSSeedData("tyo1.odinblockchain.org", "tyo1.odinblockchain.org")); // MN 07
        vSeeds.push_back(CDNSSeedData("chi1.odinblockchain.org", "chi1.odinblockchain.org")); // MN 08
        vSeeds.push_back(CDNSSeedData("syd1.odinblockchain.org", "syd1.odinblockchain.org")); // MN 09
        vSeeds.push_back(CDNSSeedData("sgp1.odinblockchain.org", "sgp1.odinblockchain.org")); // MN 10

        // value prefixed to addresses that consist of public keys
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 115);

        // affixed to addresses that are the hashes of scripts
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 57);

        // value prefix for the private half of the pair
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 138);

        /*
          four-byte prefixes for the so-called “stealth addresses” that got introduced in Bitcoin version 0.9, which allow fun things like addresses that can be used to generate new keys that can receive payments but not spend them. These two should have the same first byte but must differ in the other three bytes.
        */
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x27)(0x56)(0x18)(0x72).convert_to_container<std::vector<unsigned char> >();

        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x27)(0x25)(0x67)(0x46).convert_to_container<std::vector<unsigned char> >();

        // 	BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x08)(0x34).convert_to_container<std::vector<unsigned char> >();

        bech32_hrp = "odin";

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers            = true;
        fAllowMinDifficultyBlocks       = false;
        fDefaultConsistencyChecks       = false;
        fRequireStandard                = true;
        fMineBlocksOnDemand             = false;
        fSkipProofOfWorkCheck           = false;
        fTestnetToBeDeprecatedFieldRPC  = false;
        fHeadersFirstSyncingActive      = false;

        nPoolMaxTransactions = 3;
        vSporkKey = ParseHex("0431eabf7a9d86fb7403e4bab03645b55777801790b30f49858663222c096283965ce89e92c4089c045a66507c168bc8633042a369b73ad33073179c8a10d3d698");

	      strObfuscationPoolDummyAddress = "oPjJyuGDrFfT3vmiv1SHzFf9bgFwheKxpx";
        //nStartMasternodePayments = genesis.nTime + 500; //Wed, 25 Jun 2014 20:36:16 GMT

        /** Zerocoin */
        zerocoinModulus = "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784406918290641249515082189298559149176184502808489120072844992687392807287776735971418347270261896375014971824691165077613379859095700097330459748808428401797429100642458691817195118746121515172654632282216869987549182422433637259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133844143603833904414952634432190114657544454178424020924616515723350778707749817125772467962926386356373289912154831438167899885040445364023527381951378636564391212010397122822120720357";
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";

        pchMessageStart[0] = 0x92;
        pchMessageStart[1] = 0x42;
        pchMessageStart[2] = 0x25;
        pchMessageStart[3] = 0x84;

        vAlertPubKey = ParseHex("04b9fb4c92f20b633da21eb532edc892bf63b3c0460bd7addf53c7d8911e831201d75d8f12dc70fe82179ed9e09d86e506c09e609dd259b55d09ad35265872507c");

        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;

        // Primary configurations
        nDefaultPort            = 12100;    // Main (mainnet) P2P Port
        nMaxReorganizationDepth = 100;
        nMinerThreads           = 0;
        nMaturity               = 5;        // Transaction maturity
        nMasternodeCountDrift   = 4;
        bnProofOfWorkLimit      = ~uint256(0) >> 1;
        nLastPOWBlock           = 200;      // Last Proof-of-Work block
        nModifierUpdateBlock    = 200;
        nMinStakeAge            = 60 * 60;  // 1 minute

        // Modifier interval: time to elapse before new modifier is computed
        // Set to 3-hour for production network and 20-minute for test network
        // MODIFIER_INTERVAL: time to elapse before new modifier is computed
        nModifierInterval = 60;
        nModifierIntervalRatio = 3;

        // Zerocoin Configurations
        nZerocoinLastOldParams            = 99999999; // Updated to defer zerocoin v2 for further testing.
        nMaxZerocoinSpendsPerTransaction  = 7;        // Assume about 20kb each
        nMinZerocoinMintFee               = 1 * CENT; // high fee required for zerocoin mints
        nMintRequiredConfirmations        = 20;       // the maximum amount of confirmations until accumulated in 19
        nRequiredAccumulation             = 1;
        nDefaultSecurityLevel             = 100;      // full security level for accumulators
        nZerocoinHeaderVersion            = 4;        // Block headers must be this version once zerocoin is active
        nBudgetFeeConfirmations           = 3;        // Number of confirmations for the finalization fee
        nZerocoinStartHeight              = 339124;

        //! Test-net Genesis (nTime a bit in the future)
        genesis.hashMerkleRoot  = genesis.BuildMerkleTree();
        genesis.nVersion        = 1;
        genesis.nTime           = 1537276237; // Sept 18, 2018 (date +%s)
        genesis.nBits           = 0x207fffff;
        genesis.nNonce          = 0;

        // Set hash genesis block
        hashGenesisBlock = genesis.GetHash();

        // Ensure validity
        assert(hashGenesisBlock == uint256("0x2c4b736ccfcc296274c342092b1c6f9d82f0f036a94d57057ee0eed400372106"));
        assert(genesis.hashMerkleRoot == uint256("0x4e80fa44e71d494568525ac04c84c876f11124e59699ba3b59744e44fce232ac"));

        // Remove seeding nodes
        vFixedSeeds.clear();
        vSeeds.clear();

        vSeeds.push_back(CDNSSeedData("tor1-testnet.odinblockchain.org", "tor1-testnet.odinblockchain.org")); // MN 00

        // Testnet odin addresses start with 'x'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 137);

        // Testnet odin script addresses start with '8' or '9'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 19);

        // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 239);

        // Testnet odin BIP32 pubkeys start with 'DRKV'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x3a)(0x80)(0x61)(0xa0).convert_to_container<std::vector<unsigned char> >();

        // Testnet odin BIP32 prvkeys start with 'DRKP'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x3a)(0x80)(0x58)(0x37).convert_to_container<std::vector<unsigned char> >();

        // Testnet odin BIP44 coin type is '1' (All coin's testnet default)
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x01)(0x00)(0x00)(0x80).convert_to_container<std::vector<unsigned char> >();

        bech32_hrp = "odt";

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers            = true;
        fAllowMinDifficultyBlocks       = true;
        fDefaultConsistencyChecks       = false;
        fRequireStandard                = false;
        fMineBlocksOnDemand             = false;
        fTestnetToBeDeprecatedFieldRPC  = true;

        nPoolMaxTransactions = 2;

        vSporkKey = ParseHex("046c501d82ed3624c1d902c96e265b7f59aeeeef075e3ae44f08cf0a73f5b8179a32acf37004bb0565215119626beae9cd85faa81e8817e56719d7181f0557397c");

        strObfuscationPoolDummyAddress = "PCYiHgGJJ6xGHqivmdZrYjRnhaYf6AJ2Mp";
        //nStartMasternodePayments = 1505224800; //Fri, 09 Jan 2015 21:05:58 GMT
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";

        pchMessageStart[0] = 0x23;
        pchMessageStart[1] = 0x11;
        pchMessageStart[2] = 0x98;
        pchMessageStart[3] = 0x83;

        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;

        // Primary configurations
        nMaturity               = 0;
        nMinStakeAge            = 60; // 1 minute

        // Zerocoin Configurations
        nZerocoinLastOldParams  = 499;  // TODO - Updated to defer zerocoin v2 for further testing.
        nZerocoinStartHeight    = 100;  // When zODIN becomes available

        //! Regtest-net Genesis (nTime a bit in the future)
        genesis.hashMerkleRoot  = genesis.BuildMerkleTree();
        genesis.nVersion        = 1;
        genesis.nTime           = 1537276691; // Sept 18, 2018 (date +%s)
        genesis.nBits           = 0x207fffff;
        genesis.nNonce          = 6;

        // Set hash genesis block
        hashGenesisBlock = genesis.GetHash();

         // Ensure validity
        assert(hashGenesisBlock == uint256("0x4382c714d146c6fdde6042fbc7b85be7cc8ed474bb343ddd86736f3241b7ea7e"));
        assert(genesis.hashMerkleRoot == uint256("0x4e80fa44e71d494568525ac04c84c876f11124e59699ba3b59744e44fce232ac"));

        // Remove seeding nodes
        vFixedSeeds.clear();
        vSeeds.clear();
      
        bech32_hrp = "odr";

        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams
{
public:
    CUnitTestParams()
    {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 12100;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Unit test mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    //! Published setters to allow changing values in unit test cases
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority) { nEnforceBlockUpgradeMajority = anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority) { nRejectBlockOutdatedMajority = anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority) { nToCheckBlockUpgradeMajority = anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks) { fDefaultConsistencyChecks = afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) { fAllowMinDifficultyBlocks = afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams* pCurrentParams = 0;

CModifiableParams* ModifiableParams()
{
    assert(pCurrentParams);
    assert(pCurrentParams == &unitTestParams);
    return (CModifiableParams*)&unitTestParams;
}

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    case CBaseChainParams::UNITTEST:
        return unitTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
