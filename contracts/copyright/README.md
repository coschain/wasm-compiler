# copyright contract
## background
  user pushlish post on contentos chain, but how can we know a post is original or plagiarized? so we need a method to 
  distinguish copyright. this contract is a tool to do it.
## brief
  this contract is aim to set post's copyright,a post will be excluded from reward share if copyright is infringement, a post's default copyright is `0`,and can be change to `1` or `2`. This contract's owner must be `contentos`.
  
  |copyright|Description|
  |---|---|
  |0|CopyrightUnkown, means default copyright status|
  |1|CopyrightInfringement, means this post is infringement|
  |2|CopyrightConfirmation, means this post's copyright is confirmed|
## usage
  1.bp account call proposal method to suggest a agent, this agent responsible for later setcopyright operation.only one proposal is permitted in a period(86400 blocks),when last proposal expired, bp can proposal new one.
  ```
  create initminer contentos
  stake initminer contentos 1.000000
  deploy contentos copyright /your_path/copyright.wasm /your_path/copyright.abi false
  call any_bp_account_name contentos copyright proposal [\"proposal_account\"]
  ```
  
  2.all bp accounts can call vote method to agree the proposal. once over 2/3 bp voted the proposal become effective.
  ```
  call any_bp_account_name contentos copyright vote []
  ```
  
  3.then agent account can call setcopyright to set post's copyright.
  ```
  call proposal_account contentos copyright setcopyright "[postid,copyright_type,\"description of copyright\"]"
  ```
