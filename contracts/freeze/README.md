# freeze contract
## breif
  this contract is aim to freeze(1) or unfreeze(0) a account,a account can not send transaction if account is freezed.
## usage
  1.bp account call proposalfreeze method to suggest a account group that wait to be freeze or unfreeze,this method will generate a
  proposal id, this id auto increment as proposalfreeze called. there can many proposals at same time.
  ```
  create initminer contentos
  stake initminer contentos 1.000000
  deploy contentos freeze /your_path/freeze.wasm /your_path/freeze.abi false
  call bp_account contentos freeze proposalfreeze "[[\"account1\",\"account2\"],1,[\"you are bad\",\"you are too bad\"]]"
  ```
  
  2.all bp accounts can call vote method with a proposal id to agree the proposal. once over 2/3 bp voted the proposal become effective,
  these proposal accounts status change to freezed.
  ```
  call bp_account contentos freeze vote [proposal_id]
  ```
