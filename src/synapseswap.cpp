// This file is included in init.cpp directly to avoid touching the build system

#include "leveldbwrapper.h"
#include "uint256.h"
#include "coins.h"
#include "hash.h"
#include "wallet.h"
#include "txdb.h"
#include "script/sign.h"
#include "univalue.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>

extern CWallet* pwalletMain;

namespace synapseswap {
	
// Only UTXOs in the blocks between the two values are included
constexpr int maxUtxoBlockHeight = 999999999;
constexpr int minUtxoBlockHeight = 0;

struct UtxoProofNode
{
	bool left;
	uint256 hash;
};
using ProofList = std::vector<UtxoProofNode>;

struct UtxoIteratorItem
{
	uint256 txid;
	CCoins coins;
	
	bool isValid() const {
		return ! txid.IsNull();
	}
};

class UtxoIterator
{
public:
	explicit UtxoIterator(CLevelDBWrapper * db);
	
	UtxoIteratorItem next();

private:
    CLevelDBWrapper * db;
    std::unique_ptr<leveldb::Iterator> iterator;
	int debugCounter;
};

std::string binToHex(const void * p, const size_t length)
{
	char psz[1024 * 2 + 1];
	for(size_t i = 0; i < length; i++)
		sprintf(psz + i * 2, "%02x", ((unsigned char*)p)[i]);
	return std::string(psz, psz + length * 2);
}

std::string binToReversedHex(const void * p, const size_t length)
{
	char psz[1024 * 2 + 1];
	for(size_t i = 0; i < length; i++)
		sprintf(psz + i * 2, "%02x", ((unsigned char*)p)[length - i - 1]);
	return std::string(psz, psz + length * 2);
}

template <typename C>
std::string binToHex(const C & c)
{
	return binToHex(&c[0], c.size());
}

template <typename C>
std::string binToReversedHex(const C & c)
{
	return binToReversedHex(&c[0], c.size());
}

CAmount getUnspentAmount(const CCoins & coin)
{
	CAmount amount = 0;
	
	for(const auto & txOut : coin.vout) {
		if(! txOut.IsNull() && txOut.nValue > 0) {
			amount += txOut.nValue;
		}
	}
	
	return amount;
}

uint256 computeHashes(const uint256 & left, const uint256 & right)
{
	return Hash(left.begin(), left.end(), right.begin(), right.end());
}

std::string getHashString(const uint256 & hash)
{
	return hash.GetHex();
	//return hash.ToStringReverseEndian();
}

UtxoIterator::UtxoIterator(CLevelDBWrapper * db)
	:
		db(db),
		iterator(db->NewIterator())
{
	iterator->SeekToFirst();
	
	debugCounter = 0;
}

UtxoIteratorItem UtxoIterator::next()
{
	for(; iterator->Valid(); iterator->Next()) {
		if(iterator->key().size() != 33) {
			continue;
		}

        CDataStream keyStream(iterator->key().data(), iterator->key().data() + iterator->key().size(), SER_DISK, CLIENT_VERSION);
        std::pair<char, uint256> keyPair;
        keyStream >> keyPair;
		
		if(keyPair.first != 'c') {
			continue;
		}

        CDataStream valueStream(iterator->value().data(), iterator->value().data() + iterator->value().size(), SER_DISK, CLIENT_VERSION);
        CCoins coins;
        valueStream >> coins;
        
        iterator->Next();
        
        if(getUnspentAmount(coins) <= 0) {
			continue;
		}
		
		if(coins.nHeight < minUtxoBlockHeight || coins.nHeight > maxUtxoBlockHeight) {
			continue;
		}
        
        return UtxoIteratorItem {
			keyPair.second,
			coins
		};
	}
	
	return UtxoIteratorItem();
}

// This is a hack to access protected member 'db' in CCoinsViewDB
// It's UB according to C++ standard, but it should work in all compilers.
class HackCoinsViewDB : public CCoinsViewDB
{
public:
	CLevelDBWrapper * getDb() {
		return &db;
	}
};

struct KeyItem
{
	CKeyID id;
	CPubKey publicKey;
	CKey key;
};

/*
To verify an UnlockItem,
1. Use txid, scriptPubKey and amount to calculate the leaf hash, similar how SynapseSwap::computeHashTxOut does
2. Verify the hash in the merkle tree.
3. Verify the signature of redeemScript, similar as SynapseSwap::signTxOut
*/
struct UnlockItem
{
	uint256 txid;
	int out;
	CScript scriptPubKey;
	CAmount amount;
	CScript redeemScript;
};

class SynapseSwap
{
private:
	using HashList = std::vector<uint256>;

public:
	SynapseSwap(CCoinsViewDB * coinsViewDB);

	void debugTest();

	// Compute a merkle tree leaf hash from a CTxOut
	uint256 computeHashTxOut(const uint256 & txid, const CTxOut & out);

	// Compute a merkle tree leaf hash from a CTxOut
	// If the CTxOut is spent, an empty uint256 is returned;
	uint256 checkedComputeHashTxOut(const uint256 & txid, const CTxOut & out);

	// Compute the merkle tree of all valid UTXOs
	// A valid UTXO has coins, and the block height is within [minUtxoBlockHeight, maxUtxoBlockHeight]
	uint256 computeMerkleRoot();

	// Compute a proof path (ProofList) of a give hash.
	// The hash can be obtained by checkedComputeHashTxOut
	ProofList getProof(const uint256 & hash);
	
	std::string proofListToText(const ProofList & proofList);

	// Compute the merkle root from a proof path (ProofList) of a give hash.
	// The hash can be obtained by checkedComputeHashTxOut
	// The result can be compared to the result of `computeMerkleRoot()`
	uint256 computeProofRoot(uint256 hash, const ProofList & proof);

	// Get a list of UnlockItem. The result can be used to transfer coins in current wallet from old chain to new chain.
	// The contract on the new chain will verify the UnlockItem against the merkle tree.
	std::vector<UnlockItem> getUnlockItems();
	
	void saveHashList(const std::string & fileName);
	
private: // test functions
	void debugDumpUtxo();
	void debugDumpSignatures();

private:
	bool getUtxoCoins(const uint256 & txid, CCoins & coins) const;
	uint256 getProofHash(const HashList & hashList, const int index);
	int buildHashList(HashList & hashList, const uint256 * hashToProof);
	void moveUp(HashList & hashList);
	std::vector<KeyItem> getKeys();
	CWallet * getWallet() const;
	SignatureData signTxOut(const int nOut, const CWalletTx* pcoin);

private:
	CLevelDBWrapper * utxoDb;
};

SynapseSwap::SynapseSwap(CCoinsViewDB * coinsViewDB)
	: utxoDb(static_cast<HackCoinsViewDB *>(coinsViewDB)->getDb())
{
}

uint256 SynapseSwap::computeHashTxOut(const uint256 & txid, const CTxOut & out)
{
	CDataStream stream(0, 0);
	stream << txid;
	stream << out.scriptPubKey;
	stream << out.nValue;
	return Hash(stream.begin(), stream.end());
}

uint256 SynapseSwap::checkedComputeHashTxOut(const uint256 & txid, const CTxOut & out)
{
	if(out.IsNull()) {
		return uint256();
	}
	
	return computeHashTxOut(txid, out);
}

uint256 SynapseSwap::getProofHash(const HashList & hashList, const int index)
{
	if((index & 1) == 0) {
		return (index + 1 >= (int)hashList.size() ? hashList[index] : hashList[index + 1]);
	}
	else {
		return hashList[index - 1];
	}
}

int SynapseSwap::buildHashList(HashList & hashList, const uint256 * hashToProof)
{
	UtxoIterator iterator(utxoDb);
	
	for(;;) {
		UtxoIteratorItem item = iterator.next();
		if(! item.isValid()) {
			break;
		}
		for(const auto & out : item.coins.vout) {
			uint256 hash = checkedComputeHashTxOut(item.txid, out);
			if(!hash.IsNull()) {
				hashList.push_back(hash);
			}
		}
	}
	
	std::sort(hashList.begin(), hashList.end());
	
	int indexToProof = -1;
	if(hashToProof != nullptr) {
		auto it = std::find(hashList.begin(), hashList.end(), *hashToProof);
		if(it != hashList.end()) {
			indexToProof = it - hashList.begin();
		}
	}
	
	return indexToProof;
}

void SynapseSwap::moveUp(HashList & hashList)
{
	int count = (int)hashList.size();
	int halfCount = (count + 1) / 2;
	
	for(int i = 0; i < halfCount; ++i) {
		int leftIndex = i * 2;
		int rightIndex = leftIndex + 1;
		if(rightIndex >= count) {
			rightIndex = leftIndex;
		}
		hashList[i] = computeHashes(hashList[leftIndex], hashList[rightIndex]);
	}
	
	hashList.resize(halfCount);
}

uint256 SynapseSwap::computeMerkleRoot()
{
	HashList hashList;
	buildHashList(hashList, nullptr);
	
	while(hashList.size() > 1) {
		moveUp(hashList);
	}
	
	if(hashList.empty()) {
		return uint256();
	}
	
	return hashList.front();
}

ProofList SynapseSwap::getProof(const uint256 & hash)
{
	HashList hashList;
	uint256 proofHash;
	int index = buildHashList(hashList, &hash);
	if(index < 0) {
		return ProofList();
	}
	
	ProofList proof;

	for(;;) {
		proofHash = getProofHash(hashList, index);

		proof.push_back({
			(index & 1) > 0,
			proofHash
		});
		
		moveUp(hashList);
		
		if(hashList.size() == 1) {
			break;
		}

		index >>= 1;
	}
	return proof;
}

std::string SynapseSwap::proofListToText(const ProofList & proofList)
{
	UniValue value(UniValue::VARR);
	for(const auto & proof : proofList) {
		UniValue item(UniValue::VOBJ);
		item.pushKV("left", proof.left);
		item.pushKV("hash", getHashString(proof.hash));
		value.push_back(item);
	}
	return value.write();
}

UniValue unlockItemToJson(const UnlockItem & item)
{
	UniValue json(UniValue::VOBJ);
	json.pushKV("txid", item.txid.GetHex());
	json.pushKV("out", item.out);
	json.pushKV("scriptPubKey", binToHex(item.scriptPubKey));
	json.pushKV("amount", item.amount);
	json.pushKV("redeemScript", binToHex(item.redeemScript));
	return json;
}

std::string unlockItemToText(const UnlockItem & item)
{
	return unlockItemToJson(item).write();
}

uint256 SynapseSwap::computeProofRoot(uint256 hash, const ProofList & proof)
{
	std::cout << "===== computeProofRoot hash = " << hash.GetHex() << std::endl;
	for(const auto node : proof) {
		if(node.left) {
			hash = computeHashes(node.hash, hash);
		}
		else {
			hash = computeHashes(hash, node.hash);
		}
	}
	
	return hash;
}

bool SynapseSwap::getUtxoCoins(const uint256 & txid, CCoins & coins) const
{
    return utxoDb->Read(make_pair('c', txid), coins);
}

void SynapseSwap::saveHashList(const std::string & fileName)
{
	HashList hashList;
	buildHashList(hashList, nullptr);
	
	std::ofstream file(fileName);
	for(const auto & hash : hashList) {
		file << binToHex(hash.begin(), hash.size()) << std::endl;
	}
	file.close();
}

void SynapseSwap::debugTest()
{
	debugDumpUtxo();
	
	uint256 tx("277de02a6b71ea455061ae3e9898b74cb9142750d5966268aaa5bdb317f7380b");
	CCoins coins;
	if(!getUtxoCoins(tx, coins)) {
		std::cout << "Can't find coins for " << getHashString(tx) << std::endl;
	}
	else {
		for(const auto & out : coins.vout) {
			auto hash = checkedComputeHashTxOut(tx, out);
			if(hash.IsNull()) continue;

			ProofList proof = getProof(hash);
			if(proof.empty()) {
				continue;
			}
			std::cout << "Proof hash: " << hash.GetHex() << std::endl;
			std::cout << "Proof text: " << proofListToText(proof) << std::endl;
			for(const auto & node : proof) std::cout << "path: " << getHashString(node.hash) << " " << (node.left ? "left" : "right") << std::endl;

			uint256 proofRoot = computeProofRoot(hash, proof);
			std::cout << "proofRoot: " << getHashString(proofRoot) << std::endl;
			
			break;
		}
	}

	std::cout << "Root: " << getHashString(computeMerkleRoot()) << std::endl;
	
	std::cout << "Test: " << getHashString(computeHashes(tx, tx)) << std::endl;
	
	std::vector<UnlockItem> itemList = getUnlockItems();
	if(! itemList.empty()) {
		std::cout << "UnlockItem: " << unlockItemToText(itemList.front()) << std::endl;
	}
	//debugDumpSignatures();
	//getKeys();
}

void SynapseSwap::debugDumpUtxo()
{
	UtxoIterator iterator(utxoDb);
	int count = 0;
	for(;;) {
		UtxoIteratorItem item = iterator.next();
		if(! item.isValid()) {
			break;
		}
		++count;
		if(count <= 10) {
			std::cout << getHashString(item.txid) << " " << getUnspentAmount(item.coins) << std::endl;
		}
	}
	std::cout << "Count: " << count << std::endl;
}

void SynapseSwap::debugDumpSignatures()
{
	std::vector<UnlockItem> itemList = getUnlockItems();
	for(const UnlockItem & item : itemList) {
	}

	std::cout << "unlockItemCount: " << itemList.size() << std::endl;
}

SignatureData SynapseSwap::signTxOut(const int nOut, const CWalletTx* pcoin)
{
	CMutableTransaction tx;
	tx.nVersion = CTransaction::CURRENT_VERSION;
	tx.nLockTime = 0;
	tx.vin.push_back(CTxIn(pcoin->GetHash(), nOut));
	const auto & out = pcoin->vout[nOut];
	
	tx.vout.push_back(CTxOut(out.nValue, out.scriptPubKey));
	SignatureData sigData;
	CTransaction ntx(tx);
	if(!ProduceSignature(TransactionSignatureCreator(getWallet(), &ntx, nOut, out.nValue, SIGHASH_ALL), out.scriptPubKey, sigData)) {
		//std::cout << "signTxOut failed " << std::endl;
	}
	else {
		//std::cout << "signTxOut succeeded " << std::endl;
	}
	return sigData;
}

std::vector<UnlockItem> SynapseSwap::getUnlockItems()
{
	std::vector<UnlockItem> itemList;

	CWallet * wallet = getWallet();

	LOCK2(cs_main, wallet->cs_wallet);
	for (map<uint256, CWalletTx>::const_iterator it = wallet->mapWallet.begin(); it != wallet->mapWallet.end(); ++it) {
		const uint256 & wtxid = it->first;
		const CWalletTx* pcoin = &(*it).second;
		
		CCoins coins;
		bool hasCoins = getUtxoCoins(wtxid, coins);
		
		if(! hasCoins) {
			continue;
		}

		int nOut = -1;
		for(const auto & out : coins.vout) {
			++nOut;
			if(out.IsNull()) {
				continue;
			}
			UnlockItem item;
			item.txid = wtxid;
			item.out = nOut;
			item.scriptPubKey = out.scriptPubKey;
			item.amount = out.nValue;
			auto sigData = signTxOut(nOut, pcoin);
			item.redeemScript = sigData.scriptSig;
			
			itemList.push_back(item);
		}
	}

	return itemList;
}

std::vector<KeyItem> SynapseSwap::getKeys()
{
	CWallet * wallet = getWallet();
	std::set<CKeyID> keyIdSet;
	wallet->GetKeys(keyIdSet);
	
	std::vector<KeyItem> keys;
	for(const CKeyID & keyId : keyIdSet) {
		KeyItem item;
		item.id = keyId;
		if(wallet->GetPubKey(keyId, item.publicKey)) {
			if(wallet->GetKey(keyId, item.key)) {
				keys.push_back(item);
			}
		}
	}
	return keys;
}

CWallet * SynapseSwap::getWallet() const
{
	return pwalletMain;
}

} //namespace synapseswap
