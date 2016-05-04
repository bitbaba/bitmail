Usage: 

BitmailClient \<mode\> [options]

Mode enumeration:

	--generate: generate certificates, and private key

	--getid:    get identifier of a certificate

	--sign:     sign a message

	--verify:   verify a signature

	--encrypt:  encrypt a message with buddy's certificate

	--decrypt:  decrypt encrypted message with your own private key

	--sendmsg:  send a signed message, if possible, encrypt it

	--recvmsg:  receive messages and poll it out

	--decrypt:  print this help

Options:

	-B: buddy index file

	-b: buddy certificate

	-C: common name of certificate

	-E: email address of certificate

	-P: passphrase for private key

	-p: certificate for your profile

	-i: interval in seconds to receive message

	-k: private key file of your profile

	-m: message file

	-t: buddy's email address

	-s: signature file to verify

	-u: poll url to notify received messages 

	-x: network configuration while sending or receiving

Examples:

	[Generate]

	  BitmailClient  --generate 

			 -C commonName -E emailAddress -P passphrase

	[GetId]   

	  BitmailClient  --getid    

			 -b buddyCertFile

	[Sign]    

	  BitmailClient  --sign     

			 -p profile_cert_file 

			 -k profile_key_file 

			 -P passphrase 

			 -m msg_file

	[Verify]  

	  BitmailClient  --verify   

			 -b buddy_cert_file   

			 -s sig_file

	[Encrypt] 

	  BitmailClient  --encrypt  

			 -b buddy_cert_file   

			 -m msg_file

	[Decrypt] 

	  BitmailClient  --decrypt  

			 -p profile_cert_file 

			 -k profile_key_file          

			 -P passphrase               

			 -m encrypted_msg_file

	[Send]    

	  BitmailClient  --sendmsg  

			 -x net_config_file   

			 -b buddy_cert_file(optional) 

			 -t email_of_buddy(optional) 

			 -p profile_cert_file 

			 -k profile_key_file 

			 -P passphrase 

			 -m msg_file

	[Receive] 

	  BitmailClient  --recvmsg  

			 -x net_config_file   

			 -B buddy_index_file 

			 -p profile_cert_file 

			 -k profile_key_file          

			 -P passphrase               

			 -i interval          

			 -u poll_url



