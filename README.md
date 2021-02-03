# rainycrack
Generates words by tacking, the bruteforce entropy is reduced by two and spans from the two sides.  
Treats all the lengths together.  

!Only works with even length character sets!  

I'm not matching jtr/hashcat's speed as not using simd.  
Also there is a big drop in perf when the hash file is 'too big'.  
Please use my fork of JohnTheRipper featuring the 'rain' alonside the 'tacking' method (I can't yet split task with the rain mode in rainycrack...)  




