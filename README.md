# rainycrack
A word generator with a special algorithm.  
Garanteed without duplicates and a global mangling.  

This project is included in my fork of john the ripper.  
Also included in thc-hydra by van hauser.  

EDIT:  
We are no longer mangling the words by increasing the choose character value from the set and clamping it with a modulo,  
instead the index of the state strafes, so every character gets treated.  
This is less prone to finding a few tough passwords miraculously, but can be controlled more easily.  
TODO:  
There can be some extra work done to create an hybrid of the two techniques.  
