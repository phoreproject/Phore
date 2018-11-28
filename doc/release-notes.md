ODIN Core version 1.5.0
==========================

> #### [› ODIN Github Releases](https://github.com/odinblockchain/odin/releases)
> #### [› ODIN Github Issues](https://github.com/odinblockchain/odin/issues)

Please reach out to our various ODIN Communities for assistance:

> #### [› ODIN Official Discord](https://discord.me/odinblockchain)
> #### [› ODIN Official Reddit](https://www.reddit.com/r/OdinBlockchain/)
> #### [› ODIN Official Telegram](https://t.me/ODINchain)


Minor Functionality Improvement
==========================

ODIN-Qt Core v1.5.0 is our second minor release since ODIN officialy launched on the Mainnet. It includes a **minor** version bump as changes have been made to some JSON-RPC functionality to add enhanced support for *OP_RETURN* transactions. In addition, we have also started adding support for an official "Dark" theme for the wallet and have fixed a bug where external themes were not being picked up properly.

## Core Improvements

### RPC Method: `createrawtransaction`
Previously when creating a raw transaction through the `odin-cli` you were expected to provide a `JSON` object of addresses and values. While this was fine, it offered no further means of control when working to include `OP_RETURN` transactions.

**Previous example:**
```
odin-cli createrawtransaction '[{txid: "xxx123", vout: 1}]' '{address:0.5, address2:1.00}'
```

At first the idea was to just allow an `OP_RETURN` to be placed within the `address` bit, and the `HEX` amount/data to be where the otherwise numerical amount is. While this could work, there would be no user control for *how* much Ø would be sent to `OP_RETURN` to be burned as well as the `HEX` code to store on the blockchain.

As of `1.5.0` this method now expects two arrays. The first array has not changed and is still an array of `input` objects. The second array now provides the flexibility for the user to optionally specify the value and data input for an `OP_RETURN`. If no `amount` is specified it will default to `0.0001` Ø. This fee will discourage network abuse and provide a means of balancing the reward system.

**Current example:**
```
odin-cli createrawtransaction '[{txid: "xxx123", vout: 1}]' '[{address: "xxx123", amount: 1.0}, {data:"74657374", amount: 0.5}]'
```

### External Theme Support + Dark Theme
The community asked for it, and we've worked to start supporting a Dark Theme! While looking into supporting multiple themes for the ODIN-Qt wallet, it became apparent that there was some ability to create a `themes/` folder within an `ODIN` application support folder with a custom stylesheet to be applied.

We patched this right up so now it will properly look for stylesheets with the `.css` extension located within your `ODIN` data directory. The theme name will be represented as the name of the file. This is a great feature, as it allowed testing of the Dark theme without having to constantly rebuild the wallet from source! Every saved change is reflected within the wallet application upon an application restart.

**Example Directory Structure:**
```
~/Library/Application\ Support/Odin
  blocks/
  chainstate/
  themes/ << Create this folder
    my-new-theme.css << CSS theme file
```

We'll work on a guide soon on providing more information about how you can start playing around with your own theme and potentially even share it with others in the community!


How to get involved with ODIN
==========================

ODIN Blockchain is a fork of Phore and PIVX, bringing the best features and stability to our community to build with.

We are looking for anyone with an interest and drive in blockchain technology. Whether you are a creative designer, hobby programmer, social media butterfly, or just a casual onlooker you could make your mark in the future of ODIN! Join us on any of our primary digitial communities to begin the conversation.
