# freeze contract
## breif
  this contract is aim to freeze or unfreeze a account,a account can not send transaction if account is freezed.
## usage
  1.bp account call proposalfreeze method to suggest a account group that wait to be freeze or unfreeze,this method will generate a
  proposal id.
  
  2.all bp accounts can call vote method with a proposal id to agree the proposal. once over 2/3 bp voted the proposal become effective,
  these accounts status change to freezed.
