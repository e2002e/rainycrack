# rainycrack
a word generator with a special algorithm.

For each character in the word but the first one:  
	-selected character is modified with a value 'rotate', and passed through a modulo that is the length of the set.
	-'rotate' is divided by the number of characters in the set.  

The when the word is done, 'rotate' is set to the words global counter; 
