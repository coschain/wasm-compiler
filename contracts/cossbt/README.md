# Contentos SBT Service Contract

Soulbound Tokens (SBT) are special NFTs. They are designed to be badges that are attached to their owners at mint time and cannot be transferred after that.

This contract implements an SBT service. It supports the management of multiple SBT families. Contentos users can call the same SBT service contract to issue their own SBTs without any coding.

The official account `contentos` will provide the public SBT service by deploying this contract with the name `cossbt`.

## How to use

This section demonstrates how to use the contract through Contentos `wallet_cli`. Developers can also make the same contract calls using Contentos SDKs in their own dAPPs.

### Issue a token family

A token family is a group of similar SBTs. 

Before really creating SBTs, one need to issue a token family, which is the same thing as your writing and publishig an [ERC-5484](https://eips.ethereum.org/EIPS/eip-5484) contract on Ethereum. Here at Contentos, just make a single call to the service contract,

```bash
> call <YOUR_ACCOUNT> contentos cossbt issue [\"MYSBT\",\"Wooow!\",\"http://<YOUR_SITE>\"]
```

The above command issued a new token family named `MYSBT` with a description of `Wooow!` and a URI of `http://<YOUR_SITE>`. It's actually calling the contract method,

```C++
void issue(const std::string& symbol, const std::string& desc, const std::string& uri);
```

- `symbol` is the unique ID of the token family, consisting of 3 to 8 upper-cased letters or decimal digits.
- `desc` is the description text which contains 128 chars at most.
- `uri` is the URI of the token family, which should give a token's meta information at `<uri>?token=<token_id>`, e.g. http://mysbt.com/?token=12345.

You can list all issued token families by querying the contract's `tokens` table.

```
> table contentos cossbt tokens symbol "" 100
queryTable detail: 

[
    {
        "symbol": "MYSBT",
        "desc": "Wooow!",
        "uri": "https://mysbt.com/",
        "minted_count": 0,
        "burned_count": 0,
        "issuer": "someone",
        "issued_at": 1632477858
    }
]
```

### Mint tokens

Once a token family was issued, the issuer can mint arbitary number of SBTs into the family. 

```
> call <YOUR_ACCOUNT> contentos cossbt mint [\"MYSBT\",\"TOKEN1\",\"issuer\",\"charlie\"]
```

The above command atomically minted a SBT into the token family `MYSBT` with a token ID of `TOKEN1`, marked it as burnable-by-the-issuer-only and attached it to account `charlie`. The command calls the contract method,

```C++
void mint(const std::string& symbol, const std::string& token_id, const std::string& burn_auth, const cosio::name& receiver);
```

- `symbol` is the token family's symbol.
- `token_id` is the token id, which is a non-empty string containing at most 128 upper-cased letters or digits.
- `burn_auth` is the token burn authorization, must be one of 
  - `issuer`, only the issuer can burn this token
  - `owner`, only the owner can burn this token
  - `issuer_owner`, both the issuer and the owner can burn this token
  - `none`, nobody can burn this token
- `receiver` is the receiver account of minted token.

All tokens of the same family must have different token IDs. It's the caller's duty to ensure the uniqueness of `token_id` passed to `mint()` method. If a duplicate ID is found, `mint()` will fail.

To check the ownership of a specific token, query the `holdings` table,

```
> table contentos cossft holdings global_id \"MYSBT:TOKEN1\" 1
queryTable detail: 

[
    {
        "global_id": "MYSBT:TOKEN1",
        "symbol": "MYSBT",
        "token": "TOKEN1",
        "owner": "charlie",
        "burn_auth": "issuer"
    }
]
```

Or list all holdings of a specific account,

```
> table contentos cossbt holdings owner \"charlie\" 100
queryTable detail: 

[
    {
        "global_id": "MYSBT:TOKEN1",
        "symbol": "MYSBT",
        "token": "TOKEN1",
        "owner": "charlie"
        "burn_auth": "issuer"
    }
]
```


### Burn tokens

SBTs can be burned by the issuer and/or the owner according to their burn authorization. See the `burn_auth` parameter in [Mint tokens](#mint-tokens). 

```
> call <YOUR_ACCOUNT> contentos cossbt burn [\"MYSBT\",\"TOKEN1\"] 
```

The above command burned `TOKEN1` of `MYSBT` family. It calls the contract method,

```C++
void burn(const std::string& symbol, const std::string& token_id);
```

- `symbo` is the family symbol of the token.
- `token_id` is the token ID.

When a caller is not authorized, token burning will fail.


## Service Management

Though it's recommended to use the public SBT service provided by `contentos` account, any Contentos user can publish their own SBT service by deploying this contract. Once deployed the contract, you become the manager of your SBT service and have access to the management methods of the contract.

### Enable or disable your service

```
> call <YOUR_ACCOUNT> <YOUR_ACCOUNT> cossbt enable [true]
```

The service is disabled by default. The service manager need to enable it first, otherwise, calls to `issue()`, `mint()`, `burn()` will be refused.

### Price your service

The service is payable but it's totally free by default. The service manager can set prices for calls to `issue()`, `mint()` and `burn()`. 

```
call <YOUR_ACCOUNT> <YOUR_ACCOUNT> cossbt set_fee [1000000,2500000,34500000]
```

The above command sets fees of `issue()`, `mint()`, and `burn()` to 1.0 COS, 2.5 COS and 3.45 COS respectively.

When your service is not free, calls to your contract must carry enough COS tokens to pay the fees. e.g. 

```
> call xxxxxx <YOUR_ACCOUNT> cossbt issue [\"MYSBT\",\"Wooow!\",\"http://<YOUR_SITE>\"] -f 1000000
```

where `-f 1000000` will transfer 1.0 COS from caller `xxxxxx` to your `cossbt` contract if the contract call succeeds. If the caller has not carried enough COS tokens with the call, or his/her balance can't afford the fee, his/her calls will be refused.

Note that when a call carries more COS tokens than required, the contract won't return changes to the caller's account.

### Withdraw service fees

To check your contract's balance,

```
> query Contract {\"owner\":\"<YOUR_ACCOUNT>\",\"cname\":\"cossbt\"}
{
    "id": {
        "owner": "<YOUR_ACCOUNT>",
        "cname": "cossbt"
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
> call <YOUR_ACCOUNT> <YOUR_ACCOUNT> cossbt withdraw_fee [\"charlie\",1000000,\"nice!\"]
```

The above command transfers 1.0 COS from your service contract to account `charlie` with a memo of `nice!`.

