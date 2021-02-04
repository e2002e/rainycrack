# rainycrack
Generates words and cracks md5:  
Treats all the lengths together.  

Tacking:  
-the bruteforce spans from the two sides, doubling current moving characters.  
-the current moving characters slide along the word.  
-the symbols picked from the se 

Rain:  
This is total mangling, you better use this one. Will find less password that markov based algorithm but can crack insane ones.  

I'm not matching jtr/hashcat's speed as not using simd.  
Also there is a big drop in perf when the hash file is 'too big'.  
Please use my fork of JohnTheRipper featuring the 'rain' alonside the 'tacking' method. 




