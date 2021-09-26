# Contentos NFT Service Contract

This contract is an NaaS (NFT as a Service). It supports the management of multiple NFT families. Contentos users can call the same NFT service contract to issue their own NFTs without any coding.

The official account `contentos` will provide the public NFT service by deploying this contract with the name `cosnft`.

## How to use

This section demonstrates how to use the contract through Contentos `wallet_cli`. Developers can also make the same contract calls using Contentos SDKs in their own dAPPs.

### Issue a token family

A token family is a group of similar Non-Fungible Tokens. For example, CryptoKitties is a token family, and EtherRocks is another different token family. 

Before really creating NFTs, one need to issue a token family, which is the same thing as your writing and publishig an ERC-721 contract on Ethereum. Here at Contentos, just make a single call to the service contract,

```bash
> call <YOUR_ACCOUNT> contentos cosnft issue [\"MYNFT\",\"Wooow!\",\"http://<YOUR_SITE>\"]
```

The above command issued a new token family named `MYNFT` with a description of `Wooow!` and a URI of `http://<YOUR_SITE>`. It's actually calling the contract method,

```C++
void issue(const std::string& symbol, const std::string& desc, const std::string& uri);
```

- `symbol` is the unique ID of the token family, consisting of 3 to 8 upper-cased letters or decimal digits.
- `desc` is the description text which contains 128 chars at most.
- `uri` is the URI of the token family, which should give a token's meta information at `<uri>?token=<token_id>`, e.g. http://mynft.com/?token=12345.

You can list all issued token families by querying the contract's `tokens` table.

```
> table contentos cosnft tokens symbol "" 100
queryTable detail: 

[
    {
        "symbol": "MYNFT",
        "desc": "Wooow!",
        "uri": "https://mynft.com/",
        "minted_count": 0,
        "burned_count": 0,
        "transferred_count": 0,
        "issuer": "someone",
        "issued_at": 1632477858
    }
]
```

### Mint tokens

Once a token family was issued, the issuer can mint arbitary number of NFTs into the family. Newly minted tokens are owned by the issuer.

```
> call <YOUR_ACCOUNT> contentos cosnft mint [\"MYNFT\",\"TOKEN1\"]
```

The above command minted a NFT into the token family `MYNFT` with a token ID of `TOKEN1`. It calls the contract method,

```C++
void mint(const std::string& symbol, const std::string& token_id);
```

- `symbol` is the token family's symbol.
- `token_id` is the token id, which is a non-empty string containing at most 128 upper-cased letters or digits.

All tokens of the same family must have different token IDs. It's the caller's duty to ensure the uniqueness of `token_id` passed to `mint()` method. If a duplicate ID is found, `mint()` will fail.

To check the ownership of a specific token, query the `holdings` table,

```
> table contentos cosnft holdings global_id \"MYNFT:TOKEN1\" 1
queryTable detail: 

[
    {
        "global_id": "MYNFT:TOKEN1",
        "symbol": "MYNFT",
        "token": "TOKEN1",
        "owner": "someone"
    }
]
```

Or list all holdings of a specific account,

```
> table contentos cosnft holdings owner \"someone\" 100
queryTable detail: 

[
    {
        "global_id": "MYNFT:TOKEN1",
        "symbol": "MYNFT",
        "token": "TOKEN1",
        "owner": "someone"
    }
]
```

### Transfer tokens

Tokens owners can transfer their tokens to other Contentos accounts.

```
> call <YOUR_ACCOUNT> contentos cosnft transfer [\"<YOUR_ACCOUNT>\",\"<OTHER_ACCOUNT>\",\"MYNFT\",\"TOKEN1\"]
```

The above command sent `TOKEN1` of `MYNFT` family to a Contentos account `<OTHER_ACCOUNT>`. It calls the contract methods,

```C++
void transfer(const cosio::name& from, const cosio::name& to, const std::string& symbol, const std::string& token_id);
```

- `from` is the owner/sender of the token.
- `to` is the receiver of the token.
- `symbol` is the family to which the token belongs.
- `token_id` is the token ID.

Once the transfer successfully completed, we can observe the ownership change of `MYNFT:TOKEN1`,

```
> table contentos cosnft holdings global_id \"MYNFT:TOKEN1\" 1
queryTable detail: 

[
    {
        "global_id": "MYNFT:TOKEN1",
        "symbol": "MYNFT",
        "token": "TOKEN1",
        "owner": "anotherguy"
    }
]
```

### Burn tokens

Token owners can also burn their tokens. Please think twice before burning your tokens because it's an unrevokable action.

```
> call <YOUR_ACCOUNT> contentos cosnft burn [\"MYNFT\",\"TOKEN1\"] 
```

The above command burned `TOKEN1` of `MYNFT` family. It calls the contract method,

```C++
void burn(const std::string& symbol, const std::string& token_id);
```

- `symbo` is the family symbol of the token.
- `token_id` is the token ID.


## Service Management

Though it's recommended to use the public NFT service provided by `contentos` account, any Contentos user can publish their own NFT service by deploying this contract. Once deployed the contract, you become the manager of your NFT service and have access to the management methods of the contract.

### Enable or disable your service

```
> call <YOUR_ACCOUNT> <YOUR_ACCOUNT> cosnft enable [true]
```

The service is disabled by default. The service manager need to enable it first, otherwise, calls to `issue()`, `mint()`, `transfer()`, `burn()` will be refused.

### Price your service

The service is payable but it's totally free by default. The service manager can set prices for calls to `issue()`, `mint()`, `transfer()` and `burn()`. 

```
call <YOUR_ACCOUNT> <YOUR_ACCOUNT> cosnft set_fee [1000000,2500000,34500000,10000000]
```

The above command sets fees of `issue()`, `mint()`, `transfer()` and `burn()` to 1.0 COS, 2.5 COS, 3.45 COS and 10.0 COS respectively.

When your service is not free, calls to your contract must carry enough COS tokens to pay the fees. e.g. 

```
> call xxxxxx <YOUR_ACCOUNT> cosnft issue [\"MYNFT\",\"Wooow!\",\"http://<YOUR_SITE>\"] -f 1000000
```

where `-f 1000000` will transfer 1.0 COS from caller `xxxxxx` to your `cosnft` contract if the contract call succeeds. If the caller has not carried enough COS tokens with the call, or his/her balance can't afford the fee, his/her calls will be refused.

Note that when a call carries more COS tokens than required, the contract won't return changes to the caller's account.

### Withdraw service fees

To check your contract's balance,

```
> query Contract {\"owner\":\"<YOUR_ACCOUNT>\",\"cname\":\"cosnft\"}
{
    "id": {
        "owner": "<YOUR_ACCOUNT>",
        "cname": "cosnft"
    },
    "balance": "1.000000 COS",
    "created_time": "...",
    "abi": "...",
    "code": "...",
    ...
}
```

The service manager can transfer balance of the contract to other Contentos account.

```
> call <YOUR_ACCOUNT> <YOUR_ACCOUNT> cosnft withdraw_fee [\"charlie\",1000000,\"nice!\"]
```

The above command transfers 1.0 COS from your service contract to account `charlie` with a memo of `nice!`.

