# reputation contract
## breif
  this contract is aim to set account's reputation,a account will lost right if account's reputation is zero
  , a accounts's default reputation is 100, the max reputation is 10000.
  
## usage
  1.bp account call proposal method to suggest a agent, this agent responsible for later setrep operation.
  
  2.all bp accounts can call vote method to agree the proposal. once over 2/3 bp voted the proposal become effective
  
  3.then agent account can call setrep to set account's reputation.
