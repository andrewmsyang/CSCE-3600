# Andrew Yang
# EUID: ay0123
# ID: 11229926
# CSCE 3600 Minor1

s/682/800/
s/[)(]//g
s/([[:digit:]]+)\s([[:digit:]]+)-([[:digit:]]+)/\1-\2-\3/
s/([[:digit:]]{5})-4321/\1-1234/
s/Lois Lane/Lois Kent/
s/Lane/Ln/
s/([[:digit:]]{3})-([[:digit:]]{3})/\2-\1/
s/@/#/
s/;/,/g
$a p.s. your winnings must be claimed by February 29th.
