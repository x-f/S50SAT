$50SAT (Eagle-2 / MO-76) RFM22B data packet receiver 
 
Not proven to actually work! 
 
Arduino + RFM22B + 70cm LNA + suitable antenna 
 
Register values taken from the $50SAT ground station code – "$50sat ground station command and receive V25.bas" from the $50SAT Dropbox (http://50dollarsat.info/). 

S50SAT_receiver_test_tx.ino can be used as a test transmitter, it has the same register values as the receiver code, the packets from it are received and decoded, but I'm not sure I've got all the RFM22B registers for $50SAT right anyway. 


Receiver code has ``rf22.setPromiscuous(true);`` - it should print all packets, even if they are meant for someone else.