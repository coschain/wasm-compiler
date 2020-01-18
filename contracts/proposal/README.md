# The Block Producer Proposal Contract

A management contract for block producer proposals.

## What it is

Block producers are representing the common interest of Contentos community because they are fundamental players elected by majority of stakeholders. Block producers have the right and obligation to propose necessary improvements that help the system better meet the requirement of the community. This contract is a utility to manage block producer proposals and votes.

##How it works

Official account `contentos` will deploy this contract with the name `proposal`. Only top block producers who are really producing blocks have access to the contract. 

### Make a proposal

Block producers can make a proposal by calling the `propose()` method,

```c++
/// make a proposal.
/// @param id       proposal identifier, [_0-9a-zA-Z]+
/// @param contents proposal contents 
void propose(const std::string& id, const std::string& contents);
```

- block producers can propose as many as they want.
- proposals are not editable, i.e. no one can modify a proposal after its original publish.

### Vote for a proposal

Block producers can vote for a proposal by calling the `agree()` or `disagree()` methods,

```c++
/// agree with the proposal identified by @id.
void agree(const std::string& id);

/// disagree with the proposal identified by @id.
void disagree(const std::string& id)
```

- votes are not irrevocable, i.e. the voter has no chance to change her mind after voting.

### Approve a proposal

Once a proposal has collected "agree" votes from the majority (say 2/3+) of block producers, it gets approved. Contentos core team can then schedule an update to make it happen.  

