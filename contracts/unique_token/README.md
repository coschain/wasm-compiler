# COS Unique Token Standard v1.0  
## Foreward
The non-homogeneous digital assets is a type of asset compared with traditional currency homogeneous assets. It applied to distributed ledger network with unique assets instance, which have different data items and contents except for the unique identifiers, and those of the same type cannot be directly combined and cannot be compartmentalized.   
  
  Taking a more realistic asset, Bitcoin, as an example, this typical homogeneous digital asset has no difference and can be replaced with each other. If an account receives a Bitcoin while transferring another one, the balance, as well as the value, will remain the same, which shows other characteristics of homogeneous assets, divisible and combinable.   
  
  In contrast, with the specific attributes at the beginning of production,non-homogeneous digital assets are unique and cannot be split or simply merged, especially for those under COS Unique Token Standard.

## The Significance of Non-Homogeneous Digital Assets to Social industry

  Usually famous live host have many fans and those fans want to have a deeper interaction with host, so host need to find a way to do it more than transfer cos, COS unique token is the solution, host issue token that each one is unique and it's precious gift for his fans, so it's valuable.

## Abstract

The unique token is a no-fungible token that represent unique object. For example, unique token can represent stamp which every one has it's own unique number and yo can track the ownership of each one separately, certainly you can exchange stamp with others, but stamps can not add or sub because they are unique.

## Motivation

This manual was provided for users who working on COS blockchain contract development. Especialy for those who want to issue unique token on COS blockchain network. 

## Specification

Combining the needs of Social industry, we reviewed a number of existing non-homogeneous digital asset standards and defined COS unique token to be the one to standardize all non-homogeneous digital assets that published and circulated on the COS chain.  

## Token Detail

**Unique Token Identifiers**

Every unique token is identified by a unique `uint64` ID inside the smart contract. This identifying number shall not change for the life of the contract. While some smart contracts may find it convenient to start with ID 0 and simply increment by one for each new token, The pair (contract owner, contract name, uint64 tokenId) will then be a globally unique and fully-qualified identifier for a specific asset on an COS chain, callers must treat the ID as a "black box".

**Name**

Unique Token has a name attribute, it represent the full name of token, such as "alice fans coin"

**symbol**

Unique Token has a symbol attribute, it represent the symbol of token, for example "AFC" is the symbol name for "alice fans coin".

**Total Supply**

Unique Token has a total_supply attribute, it represent all token has been mint.

**Payload**

Unique token has a data attribute that can carry user define data to demand specific request, may be a json string to describe a cat or something else. Unique token only store user define data, the usage of data is depend on dapp logical.

**Transfer**

Transfer can be initiated by owner of token, this transfer unique token's owrnership from origin owner to new owner.

**Token Expire**

Every unique token can set a expire time, token will be destroyed after this time, this mechanism provide powerful extra functions for various scenes.
