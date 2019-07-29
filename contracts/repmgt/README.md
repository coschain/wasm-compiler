# reputation contract
## background
  every user has unique actions on contentos chain, some users do good while some do evil, we want to encourage good user and suppress evil user, so we introduce reputation system to grade different users, this contract is a tool to mark user's reputation.If user's reputation down to `0`: 
  
* The user's vote operation can not generate benifit for post author
* The user can not be a BP
* The user's post and reply can not gain reward

## brief
  this contract is aim to set account's reputation,a account will lost right if account's reputation is `0`
  ,a accounts's default reputation is `100`,the max reputation is `10000`.This contract's owner must be `contentos`.
  
## usage
  1.bp account call proposal method to suggest a agent, this agent responsible for later setrep operation.only one proposal is permitted in a period(86400 blocks),when last proposal expired, bp can proposal new one.
  ```
  create initminer contentos
  stake initminer contentos 1.000000
  deploy contentos reputation /your_path/reputation.wasm /your_path/reputation.abi false
  call any_bp_account_name contentos reputation proposal [\"proposal_account\"]
  ```
  
  2.all bp accounts can call vote method to agree the proposal. once over 2/3 bp voted the proposal become effective.
  ```
  call any_bp_account_name contentos reputation vote []
  ```
  
  3.then agent account can call setrep to set account's reputation.
  ```
  call proposal_account contentos reputation setrep "[\"reputation_account\",reputation_num,\"description of reputation\"]"
  ``
